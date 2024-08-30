//
// Created by ilesik on 7/12/24.
//

#include "discord_command_ping_impl.hpp"
#include "../../../discord/discord.hpp"

namespace gb {
    Discord_command_ping_impl::Discord_command_ping_impl() : Discord_command_ping("discord_command_ping",{"discord_command_handler","discord_bot","discord"}){}


    void Discord_command_ping_impl::run() {

    }

    void Discord_command_ping_impl::init(const Modules &modules) {
        this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_command_handler = std::static_pointer_cast<Discord_command_handler>(
            modules.at("discord_command_handler"));
        auto discord = std::static_pointer_cast<Discord>(modules.at("discord"));

        _discord_bot->add_pre_requirement([this,discord](){
            dpp::slashcommand command("ping","Command to get latency between bot and different components",_discord_bot->get_bot()->me.id);

            _command_handler->register_command(discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void>{
                    command_start();
                    double discord_api_ping = _discord_bot->get_bot()->rest_ping * 1000;
                    dpp::embed embed = dpp::embed().
                        set_color(dpp::colors::green).
                        set_description(std::format(":ping_pong:| Pong! -> {:.02f} ms",discord_api_ping));
                    _discord_bot->reply(event,dpp::message().add_embed(embed));
                    command_end();
                    co_return;
                },
                {
                    "Nothing special, just do what description states :)",
                    {"other"}
                }
            ));
        });
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_command_ping_impl>());
    }
} // gb