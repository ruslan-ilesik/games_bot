//
// Created by ilesik on 9/4/24.
//

#include "discord_dominoes_command_impl.hpp"

#include <src/modules/discord/discord_games/dominoes/discord_dominoes_game.hpp>

namespace gb {
    Discord_dominoes_command_impl::Discord_dominoes_command_impl() :
        Discord_dominoes_command("discord_dominoes_command", {}) {
        lobby_title = "Dominoes";
        lobby_description =
            "Dominoes is a family of tile-based games played with gaming pieces, commonly known as dominoes.";
        lobby_image_url = "https://media.discordapp.net/attachments/1010981120554320003/1016363239430430850/605993.png";
    }

    void Discord_dominoes_command_impl::init(const Modules &modules) {
        Discord_dominoes_command::init(modules);
        _image_processing->cache_create(Discord_dominoes_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("dominoes", "Command to start dominoes game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player2", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player3", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_integer, "players_amount",
                                                "Amount of players which should join the game (2-4). Default: 2",
                                                false));

            _command_handler->register_command(_discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    this->command_start();
                    std::vector<dpp::snowflake> players = {};
                    long players_amount = 2;

                    auto parameter = event.get_parameter("player");
                    if (std::holds_alternative<dpp::snowflake>(parameter)) {
                        players.push_back(std::get<dpp::snowflake>(parameter));
                    }
                    parameter = event.get_parameter("player2");
                    if (std::holds_alternative<dpp::snowflake>(parameter)) {
                        players.push_back(std::get<dpp::snowflake>(parameter));
                    }
                    parameter = event.get_parameter("player3");
                    if (std::holds_alternative<dpp::snowflake>(parameter)) {
                        players.push_back(std::get<dpp::snowflake>(parameter));
                    }
                    parameter = event.get_parameter("players_amount");
                    if (std::holds_alternative<long>(parameter)) {
                        players_amount = std::get<long>(parameter);
                    }

                    if (players_amount < 2 || players_amount > 4) {
                        dpp::embed embed;
                        embed.set_color(dpp::colors::red)
                            .set_title("Error!")
                            .set_description("players amount should be between 2 and 4, while you gave " +
                                             std::to_string(players_amount));

                        _bot->reply(event, dpp::message().add_embed(embed).set_flags(dpp::m_ephemeral));
                    } else {
                        Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, players_amount);
                        if (!r.is_timeout) {
                            auto d = get_game_data_initialization("dominoes");
                            auto game = std::make_unique<Discord_dominoes_game>(d, r.players);
                            co_await game->run(r.event);
                        }
                    }

                    this->command_end();
                    co_return;
                },
                {"__**Rules**__:\n[External "
                 "link](https://hungry-bags.ru/en/domino/kak_igrat_v_domino_pravila_igry_v_domino_kozel/)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will generate game and will automatically make first turn. Then, when it is yours turn bot will "
                 "allow to choose you which dominoes you can place. If there is a 2 places where you can place "
                 "dominoes "
                 "(and sides are not same) you will be able to choose side. If you do not have dominoes to place you "
                 "will have 'Take dominoes' button.",
                 {"game", "multiplayer"}}));
        });
    }


    void Discord_dominoes_command_impl::stop() {
        _command_handler->remove_command("dominoes");
        Discord_dominoes_command::stop();
        _image_processing->cache_remove(Discord_dominoes_game::get_image_generators());
    }
    void Discord_dominoes_command_impl::run() { Discord_dominoes_command::run(); }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_dominoes_command_impl>()); }
} // namespace gb
