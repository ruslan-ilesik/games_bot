//
// Created by ilesik on 9/15/24.
//

#include "discord_chess_command_impl.hpp"

#include <src/modules/discord/discord_games/chess/discord_chess_game.cpp> //because of the way chess lib we use was implemented we should include all cpp and avoid multiple includes of header ourselves. auto keyword is devil :)

namespace gb {
    dpp::task<void> Discord_chess_command_impl::_command_callback(const dpp::slashcommand_t &event) {

        std::vector<dpp::snowflake> players = {};
        auto parameter = event.get_parameter("player");
        if (std::holds_alternative<dpp::snowflake>(parameter)) {
            players.push_back(std::get<dpp::snowflake>(parameter));
        }
        parameter = event.get_parameter("move_timeout");
        long timeout = 120;
        if (std::holds_alternative<long>(parameter)) {
            timeout = std::get<long>(parameter);
        }

        if (timeout < 120 || timeout > 600) {
            dpp::message m;
            m.add_embed(dpp::embed()
                            .set_color(dpp::colors::red)
                            .set_title("Error!")
                            .set_description("move_timeout should be between 120 and 600"));
            _bot->reply(event, m);
        } else {
            Lobby_return r = co_await this->lobby(event, players, event.command.usr.id, 2);
            if (!r.is_timeout) {
                auto d = get_game_data_initialization("chess");
                auto game = std::make_unique<Discord_chess_game>(d, r.players);
                co_await game->run(r.event, timeout);
            }
        }

        co_return;
    }

    Discord_chess_command_impl::Discord_chess_command_impl() : Discord_chess_command("discord_chess_command", {}) {

        lobby_title = "Chess";
        lobby_description =
            "Chess is a board game for two players. It is sometimes called Western chess or international chess to "
            "distinguish it from related games such as xiangqi and shogi. The current form of the game emerged in "
            "Spain and the rest of Southern Europe during the second half of the 15th century after evolving from "
            "chaturanga, a similar but much older game of Indian origin. Today, chess is one of the world's most "
            "popular games, played by millions of people worldwide.";
        lobby_image_url = "https://cdn.discordapp.com/attachments/1010981120554320003/1013870289798570104/chess.png";
    }

    void Discord_chess_command_impl::run() { Discord_chess_command::run(); }

    void Discord_chess_command_impl::stop() {
        _command_handler->remove_command("chess");
        Discord_chess_command::stop();
        _image_processing->cache_remove(Discord_chess_game::get_image_generators());
    }

    void Discord_chess_command_impl::init(const Modules &modules) {
        Discord_chess_command::init(modules);
        _image_processing->cache_create(Discord_chess_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("chess", "Command to start chess game", _bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_user, "player", "Player to play with.", false));
            command.add_option(dpp::command_option(dpp::co_integer, "move_timeout",
                                                   "Time for move in seconds 120 < time < 600 (default 60).", false));

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n[External link](https://www.chesshouse.com/pages/chess-rules)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Initially, the bot will offer you to choose a figure through the buttons. the buttons display the "
                 "position of the figure, as well as its icon (you can read how to determine the position on the field "
                 "in the rules). After choosing a piece, you are offered options for where the piece can make a move. "
                 "(The color under the selected piece is blue. Under the possible options for the move is green, if "
                 "you have been put a check, the red color will be under the king). There is also a 'back' button that "
                 "will take you back to the figure selection. If there is another figure on the cell where you want to "
                 "go, the bot will draw an icon on the button.",
                 {"game", "multiplayer"}}));
        });
    }
    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_chess_command_impl>()); }
} // namespace gb
