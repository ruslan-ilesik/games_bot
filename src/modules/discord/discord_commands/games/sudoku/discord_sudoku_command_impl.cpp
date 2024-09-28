//
// Created by ilesik on 9/26/24.
//

#include "./discord_sudoku_command_impl.hpp"

#include "src/modules/discord/discord_games/sudoku/discord_sudoku_game.hpp"

namespace gb {
    Discord_sudoku_command_impl::Discord_sudoku_command_impl() : Discord_sudoku_command("discord_sudoku_command", {}) {
        lobby_title = "Sudoku";
        lobby_description = "The sudoku a puzzle in which missing numbers are to be filled into a 9 by 9 grid of "
                            "squares which are subdivided into 3 by 3 boxes so that every row, every column, and every "
                            "box contains the numbers 1 through 9";

        lobby_image_url =
            "https://media.discordapp.net/attachments/1010981120554320003/1089267097252659363/images_1.png";
    }


    void Discord_sudoku_command_impl::stop() {
        _command_handler->remove_command("sudoku");
        Discord_sudoku_command::stop();
        _image_processing->cache_remove(Discord_sudoku_game::get_image_generators());
    }

    void Discord_sudoku_command_impl::init(const Modules &modules) {
        Discord_sudoku_command::init(modules);
        _image_processing->cache_create(Discord_sudoku_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("sudoku", "Command to start sudoku game", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    this->command_start();
                    auto d = get_game_data_initialization("sudoku");
                    auto game =
                        std::make_unique<Discord_sudoku_game>(d, std::vector<dpp::snowflake>{event.command.usr.id});
                    co_await game->run(event);
                    this->command_end();
                    co_return;
                },
                {"__**Rules**__:\n[External link](https://www.sudokuonline.io/tips/sudoku-rules)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will give you buttons to choose column, then buttons to choose row, after that it will show you "
                 "buttons with numbers to place on this location.",
                 {"game", "single-player"}}));
        });
    }

    void Discord_sudoku_command_impl::run() { Discord_sudoku_command::run(); }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_sudoku_command_impl>()); }

} // namespace gb
