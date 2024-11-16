//
// Created by ilesik on 7/9/24.
//

#include "discord_bot_impl.hpp"

namespace gb {

    Discord_bot_impl::Discord_bot_impl() : Discord_bot("discord_bot", {"config","database"}) {}

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
        auto fp = _config->get_value("db_backup_path");
        _db->backup(fp);
        _bot->direct_message_create(_config->get_value("owner_discord_id"),dpp::message("backup").add_file("backup.sql",dpp::utility::read_file(fp)));
        _db_backup_timer = _bot->start_timer([this](const dpp::timer& timer) {
            _bot->log(dpp::ll_info,"Doing backup of database");
            auto fp = _config->get_value("db_backup_path");
            _db->backup(fp);
            _bot->direct_message_create(_config->get_value("owner_discord_id"),dpp::message("backup").add_file("backup.sql",dpp::utility::read_file(fp)));
        },60*60*24); // once per day

        _bot->start(dpp::st_return);
    }

    void Discord_bot_impl::stop() {
        if (_bot == nullptr) {
            throw std::runtime_error("Bot is nullptr, no way to stop it");
        }
        _bot->stop_timer(_db_backup_timer);
        _bot->shutdown();
    }

    Discord_cluster *Discord_bot_impl::get_bot() {
        return _bot.get();
    }

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

    void Discord_bot_impl::reply_new(
        const dpp::button_click_t &event, const dpp::message &message,
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

    dpp::task<dpp::confirmation_callback_t> Discord_bot_impl::co_direct_message_create(const dpp::snowflake &user_id,
                                                                                       dpp::message &message) {
        co_return co_await _bot->co_direct_message_create(user_id,message_preprocessing(message));
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_bot_impl>());
    }

} // gb
