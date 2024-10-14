//
// Created by ilesik on 10/14/24.
//

#include "discord_puzzle_15_command_impl.hpp"

#include <src/modules/discord/discord_games/puzzle_15/discord_puzzle_15_game.hpp>

namespace gb {
    Discord_puzzle_15_command_impl::Discord_puzzle_15_command_impl() :
        Discord_puzzle_15_command("discord_puzzle_15_command", {"premium_manager"}) {
        lobby_title = "puzzle 15";
        lobby_description =
            "The 15 Puzzle (also called Gem Puzzle, Boss Puzzle, Game of Fifteen, Mystic Square and more) is a sliding "
            "puzzle. It has 15 square tiles numbered 1 to 15 in a frame that is 4 tile positions high and 4 tile "
            "positions wide, with one unoccupied position. Tiles in the same row or column of the open position can be "
            "moved by sliding them horizontally or vertically, respectively. The goal of the puzzle is to place the "
            "tiles in numerical order (from left to right, top to bottom).";

        lobby_image_url = "https://i.ibb.co/MBqBBM1/15-puzzle-magical-svg.png";
    }

    void Discord_puzzle_15_command_impl::init(const Modules &modules) {
        Discord_puzzle_15_command::init(modules);
        _premium = std::static_pointer_cast<Premium_manager>(modules.at("premium_manager"));
        _image_processing->cache_create(Discord_puzzle_15_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("puzzle_15", "Command to start puzzle 15 game", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    this->command_start();
                    auto p_status = co_await _premium->get_users_premium_status(event.command.usr.id);
                    if (p_status == PREMIUM_STATUS::NO_SUBSCRIPTION) {
                        auto m =
                            dpp::message()
                                .set_flags(dpp::m_ephemeral)
                                .add_embed(
                                    dpp::embed()
                                        .set_color(dpp::colors::red)
                                        .set_description(std::format("Puzzle 15 game is premium "
                                                                     "only feature! If you wish to play it, "
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
                    } else {
                        auto d = get_game_data_initialization("puzzle_15");
                        auto game = std::make_unique<Discord_puzzle_15_game>(
                            d, std::vector<dpp::snowflake>{event.command.usr.id});
                        co_await game->run(event);
                    }

                    this->command_end();
                    co_return;
                },
                {"__**Rules**__:\n Tiles in the same row or column of the open position can be moved by sliding them "
                 "horizontally or vertically, respectively. The goal of the puzzle is to place the tiles in numerical "
                 "order (from left to right, top to bottom)."
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will start a game with randomised field (field guaranteed to be solvable) and give you controls "
                 "which represent direction of sliding. so UP key will move element below empty cell up and so on.",
                 {"game", "single-player", "premium"}}));
        });
    }
    void Discord_puzzle_15_command_impl::stop() {
        _command_handler->remove_command("puzzle_15");
        Discord_puzzle_15_command::stop();
        _image_processing->cache_remove(Discord_puzzle_15_game::get_image_generators());
    }

    void Discord_puzzle_15_command_impl::run() {
        Discord_puzzle_15_command::run();
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_puzzle_15_command_impl>());
    };
} // namespace gb
