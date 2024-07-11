//
// Created by ilesik on 7/10/24.
//

#include "discord_command_impl.hpp"

namespace gb {

    Discord_command_impl::Discord_command_impl(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                                               const Command_data &command_data)
        : _command(command), _handler(handler), _command_data(command_data) {}

    dpp::slashcommand Discord_command_impl::get_command() const {
        return _command;
    }

    slash_command_handler_t Discord_command_impl::get_handler() const {
        return _handler;
    }

    Command_data Discord_command_impl::get_command_data() const {
        return _command_data;
    }

    void Discord_command_impl::operator()(const dpp::slashcommand_t &event) {
        _handler(event);
    }

    std::string Discord_command_impl::get_name() const {
        return _command.name;
    }

} // gb
