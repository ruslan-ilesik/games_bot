//
// Created by ilesik on 7/10/24.
//

#include "discord_command_handler_impl.hpp"


namespace gb {

    void Discord_command_handler_impl::bulk_actions() {
        if (_bulk) {
            throw std::runtime_error("Discord_command_handler can not apply bulk as it set to true");
        }
        std::vector<dpp::slashcommand> slash_cmds;
        std::ranges::transform(_command_register_queue, std::back_inserter(slash_cmds),
                               [this](const std::string& c) { return _commands.at(c)->get_command(); });

        _discord_bot->get_bot()->global_bulk_command_create(slash_cmds, [this](const dpp::confirmation_callback_t & event) {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (event.is_error()) {
                _discord_bot->get_bot()->log(dpp::ll_error, "bulk command create failed");
                return;
            }
            for (auto& [k, v] : event.get<dpp::slashcommand_map>()) {
                _commands.at(v.name)->get_command().id = k;
            }
        });
        _command_register_queue.clear();
    }

    void Discord_command_handler_impl::do_command_remove(const std::string& name) {
        if (!_commands.contains(name)) {
            throw std::runtime_error("Discord_command_handler error: command " + name + " cannot be removed as it does not exist");
        }
        _discord_bot->get_bot()->global_command_delete(_commands.at(name)->get_command().id);
    }

    Discord_command_handler_impl::Discord_command_handler_impl()
        : Discord_command_handler("discord_command_handler", {"discord_bot", "admin_terminal"}) {}

    void Discord_command_handler_impl::run() {
        set_bulk(false);
    }

    void Discord_command_handler_impl::stop() {
        _admin_terminal->remove_command("discord_command_handler_bulk_disable");
        _admin_terminal->remove_command("discord_command_handler_bulk_enable");
        _admin_terminal->remove_command("discord_command_handler_run_bulk");
        _admin_terminal->remove_command("discord_command_handler_get_bulk");
    }

    void Discord_command_handler_impl::innit(const Modules &modules) {
        this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_admin_terminal = std::static_pointer_cast<Admin_terminal>(modules.at("admin_terminal"));

        _admin_terminal->add_command(
            "discord_command_handler_get_bulk",
            "Command to get current value of bulk status.",
            "Arguments: no arguments",
            [this](const std::vector<std::string> &arguments) {
                std::cout << "Command discord_command_handler_get_bulk: bulk: " << (_bulk ? "true" : "false") << std::endl;
            }
        );

        _admin_terminal->add_command(
            "discord_command_handler_bulk_disable",
            "Command to change discord command handler bulk create option to false, will register all commands in waiting list.",
            "Arguments: no arguments",
            [this](const std::vector<std::string> &arguments) {
                set_bulk(false);
            }
        );

        _admin_terminal->add_command(
            "discord_command_handler_bulk_enable",
            "Command to change discord command handler bulk create option to true, will start adding commands to buffer to register until run command is called or this option set to false.",
            "Arguments: no arguments.",
            [this](const std::vector<std::string> &arguments) {
                set_bulk(true);
            }
        );

        _admin_terminal->add_command(
            "discord_command_handler_run_bulk",
            "Command to run bulk commands to add or remove waiting, if bulk is false will return error",
            "Arguments: no arguments.",
            [this](const std::vector<std::string> &arguments) {
                if (!is_bulk()) {
                    std::cout << "Command discord_command_handler_run_bulk error: Bulk is set to false, so can not run it" << std::endl;
                    return;
                }
                bulk_actions();
            }
        );

        _admin_terminal->add_command(
            "discord_command_handler_remove_command",
            "Command to remove a command by name.",
            "Arguments: command name.",
            [this](const std::vector<std::string> &arguments) {
                if (arguments.empty()) {
                    std::cout << "discord_command_handler_remove_command command error: no command name were provided" << std::endl;
                    return;
                }
                try {
                    remove_command(arguments[0]);
                } catch (...) {
                    std::cout << "discord_command_handler_remove_command command error: error removing command" << std::endl;
                }
            }
        );
    }

    void Discord_command_handler_impl::remove_command(const std::string& name) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        do_command_remove(name);
    }

    void Discord_command_handler_impl::set_bulk(bool value) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        if (value && _bulk != value) {
            bulk_actions();
        }
        _bulk = value;
    }

    bool Discord_command_handler_impl::is_bulk() {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return _bulk;
    }

    void Discord_command_handler_impl::register_command(const Discord_command_ptr& command) {
        std::unique_lock<std::shared_mutex> lock(_mutex);

        if (_commands.contains(command->get_name())) {
            throw std::runtime_error("Discord_command_handler error: Command " + command->get_name() + " already registered");
        }
        if (_bulk) {
            _command_register_queue.push_back(command->get_name());
        } else {
            _discord_bot->get_bot()->global_command_create(command->get_command());
        }
        _commands.insert({command->get_name(), command});
    }

    void Discord_command_handler_impl::remove_commands() {
        _discord_bot->get_bot()->global_bulk_command_delete();
    }

    void Discord_command_handler_impl::register_commands() {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        std::vector<dpp::slashcommand> slash_cmds;
        std::ranges::transform(_commands | std::views::values, std::back_inserter(slash_cmds),
                               [](const Discord_command_ptr& command) { return command->get_command(); });

        _discord_bot->get_bot()->global_bulk_command_create(slash_cmds, [this](const dpp::confirmation_callback_t & event) {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (event.is_error()) {
                _discord_bot->get_bot()->log(dpp::ll_error, "bulk command create failed");
                return;
            }
            for (auto& [k, v] : event.get<dpp::slashcommand_map>()) {
                _commands.at(v.name)->get_command().id = k;
            }
        });
    }

    extern "C" Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_handler_impl>());
    }

} // gb