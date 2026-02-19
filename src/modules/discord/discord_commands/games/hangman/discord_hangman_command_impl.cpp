//
// Created by ilesik on 10/3/24.
//
#include "discord_hangman_command_impl.hpp"


#include <src/modules/discord/discord_games/hangman/discord_hangman_game.hpp>

namespace gb {
    dpp::task<void> Discord_hangman_command_impl::_command_callback(const dpp::slashcommand_t &event) {

        std::vector<dpp::snowflake> players = {event.command.usr.id};
        long players_amount = 1;

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

        parameter = event.get_parameter("player4");
        if (std::holds_alternative<dpp::snowflake>(parameter)) {
            players.push_back(std::get<dpp::snowflake>(parameter));
        }

        parameter = event.get_parameter("player5");
        if (std::holds_alternative<dpp::snowflake>(parameter)) {
            players.push_back(std::get<dpp::snowflake>(parameter));
        }

        parameter = event.get_parameter("players_amount");
        if (std::holds_alternative<long>(parameter)) {
            players_amount = std::get<long>(parameter);
        }

        // check for fail conditions
        if (players_amount < 1 || players_amount > 6) {
            _bot->reply(event, dpp::message().add_embed(
                                   dpp::embed()
                                       .set_color(dpp::colors::red)
                                       .set_title("Error!")
                                       .set_description("players amount should be between 1 and 6, while you gave " +
                                                        std::to_string(players_amount))));
        } else {
            PREMIUM_STATUS status = co_await _premium->get_users_premium_status(event.command.usr.id);
            time_t last_played = co_await _games_manager->get_seconds_since_last_game("hangman", event.command.usr.id);
            if (status == PREMIUM_STATUS::NO_SUBSCRIPTION && (players_amount > 2 || players.size() > 2)) {
                // to many players for regular user
                auto m = dpp::message()
                             .set_flags(dpp::m_ephemeral)
                             .add_embed(dpp::embed()
                                            .set_color(dpp::colors::red)
                                            .set_description(
                                                std::format("Playing hangman with more than 2 players is a premium "
                                                            "only feature! If you wish to play with more players, "
                                                            "please consider supporting us via [patreon]({})!",
                                                            _premium->get_premium_buy_url())))
                             .add_component(dpp::component()
                                                .set_type(dpp::cot_action_row)
                                                .add_component(dpp::component()
                                                                   .set_label("Information!")
                                                                   .set_type(dpp::cot_button)
                                                                   .set_style(dpp::cos_link)
                                                                   .set_url("https://gamesbot.lesikr.com/premium"))
                                                .add_component(dpp::component()
                                                                   .set_label("Subscribe!")
                                                                   .set_type(dpp::cot_button)
                                                                   .set_style(dpp::cos_link)
                                                                   .set_url(_premium->get_premium_buy_url())));
                _bot->reply(event, m);
            } else if (status == PREMIUM_STATUS::NO_SUBSCRIPTION && last_played < 60 * 60 * 24) {
                time_t minutes = (60 * 60 * 24 - last_played) / 60;
                time_t hours = minutes / 60;
                minutes = (minutes % 60) + 1;
                auto m = dpp::message()
                             .set_flags(dpp::m_ephemeral)
                             .add_embed(dpp::embed()
                                            .set_color(dpp::colors::red)
                                            .set_description(std::format(
                                                "You have used up your daily play of hangman, however, you can still "
                                                "join other hangman games! If you wish to play without limitations, "
                                                "please consider supporting us via [patreon]({})!\n\nYou can create a "
                                                "new game of hangman again in {} hours and {} minutes.",
                                                _premium->get_premium_buy_url(), hours, minutes)))
                             .add_component(dpp::component()
                                                .set_type(dpp::cot_action_row)
                                                .add_component(dpp::component()
                                                                   .set_label("Information!")
                                                                   .set_type(dpp::cot_button)
                                                                   .set_style(dpp::cos_link)
                                                                   .set_url("https://gamesbot.lesikr.com/premium"))
                                                .add_component(dpp::component()
                                                                   .set_label("Subscribe!")
                                                                   .set_type(dpp::cot_button)
                                                                   .set_style(dpp::cos_link)
                                                                   .set_url(_premium->get_premium_buy_url())));
                _bot->reply(event, m);
            } else {
                auto d = get_game_data_initialization("hangman");

                if (players_amount == 1 && players.size() == 1) {
                    auto game = std::make_unique<Discord_hangman_game>(d, players);
                    co_await game->run(event);
                } else {
                    Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, players_amount);
                    if (!r.is_timeout) {
                        auto game = std::make_unique<Discord_hangman_game>(d, r.players);
                        co_await game->start_game(r.event);
                    }
                }
            }
        }

        co_return;
    }

    Discord_hangman_command_impl::Discord_hangman_command_impl() :
        Discord_hangman_command("discord_hangman_command", {"premium_manager"}) {
        lobby_title = "Hangman";
        lobby_description = "Hangman is a guessing game for two or more players. One player thinks of a word, phrase, "
                            "or sentence and the other(s) tries to guess it by suggesting letters within a certain "
                            "number of guesses. Originally a paper-and-pencil game, there are now electronic versions.";
        ;
        lobby_image_url = "https://i.ibb.co/HN6V4Jx/11.png";
    }
    void Discord_hangman_command_impl::run() { Discord_hangman_command::run(); }

    void Discord_hangman_command_impl::stop() {
        _command_handler->remove_command("hangman");
        Discord_hangman_command::stop();
        _image_processing->cache_remove(Discord_hangman_game::get_image_generators());
    }

    void Discord_hangman_command_impl::init(const Modules &modules) {
        Discord_hangman_command::init(modules);
        _premium = std::static_pointer_cast<Premium_manager>(modules.at("premium_manager"));
        _image_processing->cache_create(Discord_hangman_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("hangman", "Command to start hangman game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player2", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player3", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player4", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_user, "player5", "Player to play with.", false))
                .add_option(dpp::command_option(dpp::co_integer, "players_amount",
                                                "Amount of players which should join the game (1-6). Default: 1",
                                                false));

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n[External link](https://www.one-to-one.ca/wp-content/uploads/2016/01/Hangman.pdf)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will start the game in your private messages (in case you play alone you will play in chat "
                 "directly). There you will need to play a game by selecting letters you want to try. After you will "
                 "guess the word or fail you will need to wait to other players and bot will show the results in the "
                 "message where game was created.",
                 {"game", "single-player", "multiplayer", "premium"}}));
        });
    }
    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_hangman_command_impl>()); }
} // namespace gb
