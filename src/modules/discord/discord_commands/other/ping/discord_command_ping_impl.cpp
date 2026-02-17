//
// Created by ilesik on 7/12/24.
//

#include "discord_command_ping_impl.hpp"
#include "../../../discord/discord.hpp"

namespace gb {
    dpp::task<void> Discord_command_ping_impl::_command_callback(const dpp::slashcommand_t &event) {
        double discord_api_ping = _bot->get_bot()->rest_ping * 1000;
        dpp::embed embed =
            dpp::embed()
                .set_color(dpp::colors::green)
                .set_description(std::format(":ping_pong:| Pong! -> {:.02f} ms", discord_api_ping));
        _bot->reply(event, dpp::message().add_embed(embed));
        co_return;
    }

    Discord_command_ping_impl::Discord_command_ping_impl() : Discord_command_ping("discord_command_ping",{}){}


    void Discord_command_ping_impl::run() {

    }

    void Discord_command_ping_impl::init(const Modules &modules) {
        Discord_command_ping::init(modules);

        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("ping", "Command to get latency between bot and different components",
                                      _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"Nothing special, just do what description states :)", {"other"}}));
        });
    }
    void Discord_command_ping_impl::stop() {
        _command_handler->remove_command("ping");
        Discord_command_ping::stop();
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_ping_impl>());
    }
} // gb