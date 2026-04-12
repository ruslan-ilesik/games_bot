//
// Created by ilesik on 7/9/24.
//

#include "discord_bot_impl.hpp"

namespace gb {

    Discord_bot_impl::Discord_bot_impl() : Discord_bot("discord_bot", {"config", "database", "logging"}) {}

    Discord_bot_impl::~Discord_bot_impl() {}

    void Discord_bot_impl::init(const Modules &modules) {
        _config = std::static_pointer_cast<Config>(modules.at("config"));
        _db = std::static_pointer_cast<Database>(modules.at("database"));
    }

    void Discord_bot_impl::run() {
        if (_bot != nullptr) {
            throw std::runtime_error("Bot variable is not nullptr, memory leak possible");
        }
        _bot = std::make_unique<Discord_cluster>(_config->get_value("discord_bot_token"));

        // Run all pre-requirements.
        {
            std::unique_lock<std::mutex> lock(_mutex);
            for (auto &i: _pre_requirements) {
                i();
            }
            _pre_requirements.clear();
        }

        auto run_command = [this](const std::string &cmd) -> std::string {

            std::array<char, 128> buffer;
            std::string result;

            // Open pipe with stderr redirected to stdout
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen((cmd + " 2>&1").c_str(), "r"), pclose);
            if (!pipe) {
                return "popen() failed!";
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }
            return result;
        };

        auto backup_fn = [this, run_command]() {
            _bot->log(dpp::ll_info, "Doing backup of database");
            if (_config->get_value("db_backup_enabled") == "false") {
                _bot->log(dpp::ll_warning, "Database backup is disabled");
                return;
            }
            auto local_file = _config->get_value("db_backup_path");
            _db->backup(local_file);

            std::string user = _config->get_value("db_backup_scp_user");
            std::string host = _config->get_value("db_backup_scp_host");
            std::string port = _config->get_value("db_backup_scp_port");
            std::string remote_base = _config->get_value("db_backup_scp_base");
            std::string password = _config->get_value("db_backup_scp_password");
            // Get local hostname
            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) != 0) {
                _bot->log(dpp::ll_error, "gethostname");
                return;
            }

            std::string remote_path = remote_base + "/" + std::string(hostname) + "/backup.sql";

            // Make directory on remote server first
            std::string mkdir_cmd = "sshpass -p '" + password + "' ssh -p " + port + " " + user + "@" + host +
                                    " 'mkdir -p " + remote_base + "/" + hostname + "'";
            auto  ret = run_command(mkdir_cmd.c_str());
            if (!ret.empty()) {
                _bot->log(dpp::ll_error, "Error: Failed to create remote directory.\n"+ret+"\n");
                return;
            }

            // SCP the file
            std::string scp_cmd = "sshpass -p '" + password + "' scp -P " + port + " " + local_file + " " + user + "@" +
                                  host + ":" + remote_path;
            ret = run_command(scp_cmd.c_str());
            if (!ret.empty()) {
                _bot->log(dpp::ll_error, "Error: SCP command failed.\n"+ret+"\n");
                return;
            }

            _bot->log(dpp::ll_info, "Backup uploaded successfully to " + remote_path + "\n");
        };

        backup_fn();
        _db_backup_timer = _bot->start_timer(
            [=](const dpp::timer &timer) {
                backup_fn();
            },
            60 * 60 * 24); // once per day

        _bot->start(dpp::st_return);

        std::promise<void> ready_promise;
        std::future<void> ready_future = ready_promise.get_future();
        _bot->on_ready([&](const dpp::ready_t& event) {
            if (dpp::run_once<struct bot_ready_signal>()) {
                _bot->log(dpp::ll_info, "Bot is READY, unblocking main thread");
                ready_promise.set_value();
            }
        });
        ready_future.wait();
    }

    void Discord_bot_impl::stop() {
        if (_bot == nullptr) {
            throw std::runtime_error("Bot is nullptr, no way to stop it");
        }
        _bot->stop_timer(_db_backup_timer);
        _bot->shutdown();
    }

    Discord_cluster *Discord_bot_impl::get_bot() { return _bot.get(); }

    void Discord_bot_impl::add_pre_requirement(const std::function<void()> &func) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_bot) {
            _pre_requirements.push_back(func);
        } else {
            func();
        }
    }

    void Discord_bot_impl::reply(const dpp::slashcommand_t &event, const dpp::message &message,
                                 const dpp::command_completion_event_t &callback) {

        event.reply(message_preprocessing(message), callback);
    }

    dpp::message Discord_bot_impl::message_preprocessing(dpp::message message) {
        for (auto &embed: message.embeds) {
            embed.set_timestamp(time(nullptr));
        }
        return message;
    }

    void Discord_bot_impl::reply(const dpp::select_click_t &event, const dpp::message &message,
                                 const dpp::command_completion_event_t &callback) {
        event.reply(dpp::ir_update_message, message_preprocessing(message), callback);
    }

    void Discord_bot_impl::reply(const dpp::button_click_t &event, const dpp::message &message,
                                 const dpp::command_completion_event_t &callback) {
        event.reply(dpp::ir_update_message, message_preprocessing(message), callback);
    }

    void Discord_bot_impl::reply_new(const dpp::button_click_t &event, const dpp::message &message,
                                     const dpp::command_completion_event_t &callback) {
        event.reply(message_preprocessing(message), callback);
    }

    void Discord_bot_impl::message_edit(const dpp::message &message, const dpp::command_completion_event_t &callback) {
        _bot->message_edit(message_preprocessing(message), callback);
    }

    void Discord_bot_impl::message_create(const dpp::message &message,
                                          const dpp::command_completion_event_t &callback) {
        _bot->message_create(message_preprocessing(message), callback);
    }

    void Discord_bot_impl::event_edit_original_response(const dpp::slashcommand_t &event, const dpp::message &m,
                                                        const dpp::command_completion_event_t &callback) {
        event.edit_original_response(message_preprocessing(m), callback);
    }

    void Discord_bot_impl::event_edit_original_response(const dpp::button_click_t &event, const dpp::message &m,
                                                        const dpp::command_completion_event_t &callback) {
        event.edit_original_response(message_preprocessing(m), callback);
    }

    dpp::task<dpp::confirmation_callback_t> Discord_bot_impl::co_direct_message_create(const dpp::snowflake &user_id,
                                                                                       dpp::message &message) {
        co_return co_await _bot->co_direct_message_create(user_id, message_preprocessing(message));
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_bot_impl>()); }

} // namespace gb
