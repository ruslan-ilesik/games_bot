//
// Created by ilesik on 9/16/24.
//

#include "discord_connect_four_command_impl.hpp"

#include <src/modules/discord/discord_games/connect_four/discord_connect_four_game.hpp>

namespace gb {
    Discord_connect_four_command_impl::Discord_connect_four_command_impl() :
        Discord_connect_four_command("discord_connect_four_command", {}) {
        lobby_title = "Connect four";
        lobby_description = "Also known as Connect 4, Four Up, Plot Four, Find Four, Captain's Mistress, Four in a "
                            "Row, Drop Four, and Gravitrips in the Soviet Union)";
        lobby_image_url =
            "https://cdn.discordapp.com/attachments/1010981120554320003/1012315739782393907/4_in_a_row.png";
    }
    void Discord_connect_four_command_impl::init(const Modules &modules) {
        Discord_connect_four_command::init(modules);
        _image_processing->cache_create(Discord_connect_four_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("connect_four", "Command to start connect four game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false));
            _command_handler->register_command(_discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    command_start();
                    std::vector<dpp::snowflake> players = {};
                    auto parameter = event.get_parameter("player");
                    if (std::holds_alternative<dpp::snowflake>(parameter)) {
                        players.push_back(std::get<dpp::snowflake>(parameter));
                    }
                    Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, 2);
                    if (!r.is_timeout) {
                        auto d = get_game_data_initialization("connect_four");
                        auto game = std::make_unique<Discord_connect_four_game>(d, r.players);
                        co_await game->run(r.event);
                    }
                    command_end();
                    co_return;
                },
                {"__**Rules**__:\n1.On your turn, drop one of your checkers down any of the slots in the top of the "
                 "grid.\n\n2.Players alternates until one player gets 4 checkers of his color in a row."
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot gives you ability to choose column where you would like to place your figure. They are "
                 "numerated from 1 to 7. You can see number of column under it on picture in the message. Buttons "
                 "also have this numbers. Click on button with number of column you would like to place your sign.",
                 {"game", "multiplayer"}}));
        });
    }
    void Discord_connect_four_command_impl::stop() {
        _command_handler->remove_command("connect_four");
        Discord_connect_four_command::stop();
        _image_processing->cache_remove(Discord_connect_four_game::get_image_generators());
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_connect_four_command_impl>());
    }


} // namespace gb
