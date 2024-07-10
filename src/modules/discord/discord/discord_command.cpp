//
// Created by ilesik on 7/10/24.
//

#include "discord_command.hpp"

namespace gb {
    class Discord_command_impl: public Discord_command {
    private:

    dpp::slashcommand _command;

    slash_command_handler_t _handler;

    Command_data _command_data;

    public:
        Discord_command_impl(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                        const Command_data &command_data) : _command(command), _handler(handler),
                                                            _command_data(command_data) {}

        dpp::slashcommand get_command() const override{
            return _command;
        }

        slash_command_handler_t get_handler() const override{
            return _handler;
        }

        Command_data get_command_data() const override{
            return _command_data;
        }

        void operator()(const dpp::slashcommand_t &event) override{
            _handler(event);
        }

        std::string get_name() const override{
            return _command.name;
        }

    };

} // gb