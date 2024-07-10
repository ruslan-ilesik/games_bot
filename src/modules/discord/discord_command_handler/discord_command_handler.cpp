//
// Created by ilesik on 7/10/24.
//

#include "discord_command_handler.hpp"
#include "../discord_bot/discord_bot.hpp"
#include "../../admin_terminal/admin_terminal.hpp"

#include <ranges>

namespace gb {

    class Discord_command_handler_impl : public Discord_command_handler {
    private:

        std::atomic_bool _bulk = true;

        std::shared_mutex _mutex;

        Discord_bot_ptr _discord_bot;

        Admin_terminal_ptr _admin_terminal;

        std::vector<std::string> _command_register_queue;

        std::map<std::string,Discord_command_ptr> _commands;

        void bulk_actions() {
            if (_bulk) {
                throw std::runtime_error("Discord_command_handler can not apply bulk as it set to true");
            }
            std::vector<dpp::slashcommand> slash_cmds;
            std::ranges::transform(_command_register_queue, std::back_inserter(slash_cmds),
                                   [this](const std::string& c) { return _commands.at(c)->get_command(); });

            _discord_bot->get_bot()->global_bulk_command_create(slash_cmds);
            _command_register_queue.clear();

        }

    public:

        Discord_command_handler_impl() : Discord_command_handler("discord_command_handler",
                                                                 {"discord_bot", "admin_terminal"}) {};

        virtual  ~Discord_command_handler_impl() = default;

        void run() override {
            set_bulk(false);
        }

        void stop() override {
            _admin_terminal->remove_command("discord_command_handler_bulk_disable");
            _admin_terminal->remove_command("discord_command_handler_bulk_enable");
            _admin_terminal->remove_command("discord_command_handler_run_bulk");
            _admin_terminal->remove_command("discord_command_handler_get_bulk");
        }

        void innit(const Modules &modules) override {
            this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
            this->_admin_terminal = std::static_pointer_cast<Admin_terminal>(modules.at("admin_terminal"));

            _admin_terminal->add_command(

                "discord_command_handler_get_bulk",
                "Command to get current value of bulk status.",
                "Arguments: no arguments",
                [this](const std::vector<std::string> &arguments) {
                    std::cout << "Command discord_command_handler_get_bulk: bulk: " << (_bulk?"true":"false") << std::endl;
                }
            );

            _admin_terminal->add_command(

                "discord_command_handler_bulk_disable",
                "Command to change discord command handler bulk create option to false, will register all commands in waiting list, also same applies for deleting.",
                "Arguments: no arguments",
                [this](const std::vector<std::string> &arguments) {
                    set_bulk(false);
                }
            );

            _admin_terminal->add_command(

                "discord_command_handler_bulk_enable",
                "Command to change discord command handler bulk create option to true, will start adding commands to buffer to register or delete until run command is called or this option set to false.",
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
                        std::cout
                            << "Command discord_command_handler_run_bulk error: Bulk is set to false, so can not run it"
                            << std::endl;
                        return;
                    }
                    bulk_actions();

                }
            );

        }

        void set_bulk(bool value) override {
            if (value && _bulk != value) {
                bulk_actions();
            }
            _bulk = value;
        }

        bool is_bulk() override {
            return _bulk;
        }

        virtual void register_command(const Discord_command_ptr& command) override{
            std::unique_lock<std::shared_mutex> lock (_mutex);

            if (_commands.contains(command->get_name())){
                throw std::runtime_error("Discord_command_handler error: Command " + command->get_name() + "already registered");
            }
            if (_bulk){
                _command_register_queue.push_back(command->get_name());
            }
            else{
                _discord_bot->get_bot()->global_command_create(command->get_command());
            }
            _commands.insert({command->get_name(),command});
        }

    };

    /**
     * @brief Function to create a new instance of the Discord_command_handler_impl module.
     * @return A shared pointer to the newly created Discord_command_handler_impl module.
     */
    extern "C" Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_handler_impl>());
    }
} // gb