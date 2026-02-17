//
// Created by ilesik on 9/9/24.
//

#include "discord_battleships_command_impl.hpp"

#include <src/modules/discord/discord_games/battleships/discord_battleships_game.hpp>

namespace gb {
    dpp::task<void> Discord_battleships_command_impl::_command_callback(const dpp::slashcommand_t &event) {

        std::vector<dpp::snowflake> players = {};
        auto parameter = event.get_parameter("player");
        if (std::holds_alternative<dpp::snowflake>(parameter)) {
            players.push_back(std::get<dpp::snowflake>(parameter));
        }
        Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, 2);
        if (!r.is_timeout) {
            auto d = get_game_data_initialization("battleships");
            auto game = std::make_unique<Discord_battleships_game>(d, r.players);
            co_await game->run(r.event);
        }
        co_return;
    }

    Discord_battleships_command_impl::Discord_battleships_command_impl() :
        Discord_battleships_command("discord_battleships_command", {}) {
        lobby_title = "Battleships";
        lobby_description = "Battleship (also known as Battleships or Sea Battle) is a strategy type guessing game "
                            "for two players. It is played on ruled grids (paper or board) on which each player's "
                            "fleet of warships are marked. The locations of the fleets are concealed from the "
                            "other player. Players alternate turns calling \"shots\" at the other player's ships, "
                            "and the objective of the game is to destroy the opposing player's fleet.";
        lobby_image_url =
            "https://media.discordapp.net/attachments/1010981120554320003/1146219816609398914/battleship.png";
    }

    void Discord_battleships_command_impl::run() { Discord_battleships_command::run(); }

    void Discord_battleships_command_impl::stop() {
        _command_handler->remove_command("battleships");
        Discord_battleships_command::stop();
        _image_processing->cache_remove(Discord_battleships_game::get_image_generators());
    }

    void Discord_battleships_command_impl::init(const Modules &modules) {
        srand(time(nullptr));
        Discord_battleships_command::init(modules);
        _image_processing->cache_create(Discord_battleships_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("battleships", "Command to start battleships game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false));
            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n[External link](https://www.cs.nmsu.edu/~bdu/TA/487/brules.htm)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will start the game in your private messages. There you will need to place your ships. Select "
                 "ship you want to place or edit, than select column and raw, rotate it if needed and place. After you "
                 "will place all ships, press ready button. After that in the main chat, bot will give you buttons to "
                 "choose column, then buttons to choose row, after that you will have attack button.",
                 {"game", "multiplayer"}}));
        });
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_battleships_command_impl>());
    }
} // namespace gb
