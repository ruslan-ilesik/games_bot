//
// Created by ilesik on 9/19/24.
//

#include "discord_minesweeper_command_impl.hpp"

#include <src/modules/discord/discord_games/minesweeper/discord_minesweeper_game.hpp>

namespace gb {

    dpp::task<void> Discord_minesweeper_command_impl::_command_callback(const dpp::slashcommand_t &event) {
        auto parameter = event.get_parameter("difficulty");
        long difficulty = 1;
        if (std::holds_alternative<long>(parameter)) {
            difficulty = std::get<long>(parameter);
        }

        if (difficulty < 1 || difficulty > 3) {
            dpp::embed embed;
            embed.set_color(dpp::colors::red)
                .set_title("Error!")
                .set_description("Difficulty should be between 1 and 3, while you gave " + std::to_string(difficulty));

            _bot->reply(event, dpp::message().add_embed(embed).set_flags(dpp::m_ephemeral));
        } else {
            auto d = get_game_data_initialization("minesweeper");
            auto game = std::make_unique<Discord_minesweeper_game>(d, std::vector<dpp::snowflake>{event.command.usr.id},
                                                                   difficulty);
            co_await game->run(event);
        }
        co_return;
    }

    Discord_minesweeper_command_impl::Discord_minesweeper_command_impl() :
        Discord_minesweeper_command("discord_minesweeper_command", {}) {
        lobby_title = "Minesweeper";
        lobby_description = "Minesweeper is a logic puzzle video game genre generally played on personal computers. "
                            "The game features a grid of clickable squares, with hidden \"mines\" scattered throughout "
                            "the board. The objective is to clear the board without detonating any mines, with help "
                            "from clues about the number of neighboring mines in each field.";
        lobby_image_url =
            "https://cdn.discordapp.com/attachments/1010981120554320003/1073728205321666701/pngwing.com_2.png";
    }

    void Discord_minesweeper_command_impl::stop() {
        _command_handler->remove_command("minesweeper");
        Discord_minesweeper_command::stop();
        _image_processing->cache_remove(Discord_minesweeper_game::get_image_generators());
    }

    void Discord_minesweeper_command_impl::run() { Discord_minesweeper_command::run(); }

    void Discord_minesweeper_command_impl::init(const Modules &modules) {
        Discord_minesweeper_command::init(modules);
        _image_processing->cache_create(Discord_minesweeper_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("minesweeper", "Command to start minesweeper game", _bot->get_bot()->me.id);
            command.add_option(
                dpp::command_option(dpp::co_integer, "difficulty", "Difficulty of the game between 1 and 3", false));
            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n[External link](https://minesweepergame.com/strategy/how-to-play-minesweeper.php)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will give you buttons to choose column, then buttons to choose row, after that it will show you "
                 "buttons with possible actions: dig, place flag, remove flag, ect.",
                 {"game", "single-player"}}));
        });
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_minesweeper_command_impl>());
    }


} // namespace gb
