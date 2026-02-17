//
// Created by ilesik on 7/31/24.
//

#include "discord_tic_tac_toe_command_impl.hpp"
#include "src/modules/discord/discord_games/tic_tac_toe/discord_tic_tac_toe_game.hpp"

namespace gb {
    dpp::task<void> Discord_tic_tac_toe_command_impl::_command_callback(const dpp::slashcommand_t &event) {

        std::vector<dpp::snowflake> players = {};
        auto parameter = event.get_parameter("player");
        if (std::holds_alternative<dpp::snowflake>(parameter)) {
            players.push_back(std::get<dpp::snowflake>(parameter));
        }
        Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, 2);
        if (!r.is_timeout) {
            auto d = get_game_data_initialization("tic tac toe");
            auto game = std::make_unique<Discord_tic_tac_toe_game>(d, r.players);
            co_await game->run(r.event);
        }

        co_return;
    }

    Discord_tic_tac_toe_command_impl::Discord_tic_tac_toe_command_impl() :
        Discord_tic_tac_toe_command("discord_game_tic_tac_toe", {}) {
        lobby_title = "Tic Tac Toe";
        lobby_description = "Simple Tic Tac Toe game everyone know (I believe)";
        lobby_image_url = "https://i.ibb.co/wM7sbC9/tic-tac-toe.webp";
    }

    void Discord_tic_tac_toe_command_impl::init(const Modules &modules) {
        Discord_game_command::init(modules);
        _image_processing->cache_create(Discord_tic_tac_toe_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("tic_tac_toe", "Command to start tic tac toe game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false));
            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n1. The game is played on a grid that's 3 squares by 3 squares."
                 "\n\n2. You are X, your friend (or the computer in this case) is O. Players take turns putting"
                 " their marks in empty squares.\n\n3. The first player to get 3 of her marks in a row (up, down,"
                 " across, or diagonally) is the winner.\n\n4. When all 9 squares are full, the game is over. If "
                 "no player has 3 marks in a row, the game ends in a tie."
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot representing game field in buttons, just click on button where you would like to place your "
                 "sign.",
                 {"game", "multiplayer"}}));
        });
    }

    void Discord_tic_tac_toe_command_impl::stop() {
        _command_handler->remove_command("tic_tac_toe");
        Discord_tic_tac_toe_command::stop();
        _image_processing->cache_remove(Discord_tic_tac_toe_game::get_image_generators());
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_tic_tac_toe_command_impl>());
    }
} // namespace gb
