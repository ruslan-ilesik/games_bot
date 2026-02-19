//
// Created by ilesik on 9/16/24.
//

#include "discord_connect_four_game.hpp"

namespace gb {
    Discord_connect_four_game::Discord_connect_four_game(Game_data_initialization &_data,
                                                         const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players, &Discord_connect_four_game::run) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_connect_four_game::get_image_generators() {
        return {};
    }

    Image_ptr Discord_connect_four_game::generate_image() {
        _img_cnt++;
        int size = 256;
        int for_numbers = size / 8;
        int distance_between = 2;
        int circle_size = size / 12 - distance_between / 2;
        Image_ptr img = _data.image_processing->create_image({ size/6*7+distance_between,size+for_numbers}, {0, 0, 255});
        for (int y = 0; y < static_cast<int>(std::size(_board)); y++) {
            for (int x = 0; x < static_cast<int>(std::size(_board[y])); x++) {
                img->draw_circle({circle_size + distance_between + circle_size * x * 2 + distance_between * x,
                                  circle_size + distance_between + circle_size * y * 2 + distance_between * y},
                                 circle_size,
                                 _board[y][x] == " "   ? Color(0, 0, 0)
                                 : _board[y][x] == "r" ? Color(255, 0, 0)
                                                      : Color(204, 204, 0),
                                 -1);
            }
        }

        for (int x = 0; x < static_cast<int>(std::size(_board[0])); x++) {
            img->draw_text(std::to_string(x + 1),
                           {distance_between + circle_size / 2 + circle_size * x * 2 + distance_between * x,
                            size + for_numbers - for_numbers / 3},
                           for_numbers / 24, {255, 255, 255}, for_numbers / 12);
        }
        return img;
    }



    dpp::task<void> Discord_connect_four_game::run(dpp::button_click_t event) {
        dpp::message message;
        message.add_embed(dpp::embed());
        message.id = event.command.message_id;
        message.channel_id = event.command.channel_id;
        message.guild_id = event.command.guild_id;
        dpp::task<Button_click_return> button_click_awaitable;
        Button_click_return r;

        auto end_game = [&]() {
            message.components.clear();
            std::string desc ="Player "+dpp::utility::user_mention(get_current_player())+ " *won* ";
            next_player();
            desc += "player "+dpp::utility::user_mention(get_current_player())+ "in **Connect four**.\n"+dpp::utility::user_mention(get_current_player())+" you will be luckier next time";
            message.embeds[0].set_title("Game over").set_description(desc).set_color(dpp::colors::blue);
            message.embeds[0].set_image(add_image(message, generate_image()));
            _data.bot->event_edit_original_response(event, message);
            remove_player(USER_REMOVE_REASON::LOSE,get_current_player());
            remove_player(USER_REMOVE_REASON::WIN,get_current_player());
        };


        auto send_message = [&]() -> dpp::task<void> {
            message.components.clear();
            message.embeds[0]
                .set_title("Connect four game")
                .set_description(std::format(
                    "Turn: {}\nYour color is {}\nTimeout: <t:{}:R>", dpp::utility::user_mention(get_current_player()),
                    (this->_signs[get_current_player_index()] == "r" ? "**red**" : "**yellow**"),
                    std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                            .count() +
                        60));
            dpp::component row = dpp::component().set_type(dpp::cot_action_row);
            for (int i = 0; i < static_cast<int>(std::size(this->_board[0])); i++) {

                row.add_component(dpp::component()
                                      .set_type(dpp::cot_button)
                                      .set_id(std::to_string(i))
                                      .set_label(std::to_string(i + 1))
                                      .set_disabled(this->_board[0][i] != " ")
                                      .set_style(dpp::cos_primary));
                if (row.components.size() == 5) {
                    message.add_component(row);
                    row = dpp::component().set_type(dpp::cot_action_row);
                }
            }
            message.add_component(row);

            message.embeds[0].set_image(add_image(message, generate_image()));
            button_click_awaitable = _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, 60);
            _data.bot->event_edit_original_response(event, message);
            r = co_await button_click_awaitable;
            co_return;
        };
        event.reply(dpp::ir_update_message,"Game is starting");
        co_await send_message();

        bool end = false;
        while (!end) {
            if (r.second) {
                // timeout
                next_player();
                std::string desc = "If you still want to play, you can create new game.\nPlayer"+dpp::utility::user_mention(get_current_player())+" was automatically selected as the winner.\n";
                next_player();
                desc += "Player "+dpp::utility::user_mention(get_current_player()) + " should think faster next time.";
                message.embeds[0].set_color(dpp::colors::red).set_title("Game Timeout.").set_description(desc);
                message.embeds[0].set_image(add_image(message,generate_image()));
                message.components.clear();
                _data.bot->event_edit_original_response(event,message);
                remove_player(USER_REMOVE_REASON::TIMEOUT,get_current_player());
                remove_player(USER_REMOVE_REASON::WIN,get_current_player());
                break;
            }
            event = r.first;

            // place object
            for (int i = static_cast<int>(std::size(_board) - 1); i >= 0; i--) {
                if (_board[i][std::stoi(event.custom_id)] == " ") {
                    _board[i][std::stoi(event.custom_id)] = _signs[get_current_player_index()];
                    break;
                }
            }
            // check win
            std::string player = _signs[get_current_player_index()];
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 4; c++) {
                    if (_board[r][c] == player && _board[r + 1][c + 1] == player && _board[r + 2][c + 2] == player &&
                        _board[r + 3][c + 3] == player && !end) {
                        end_game();
                        end = true;
                    }
                }
            }
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 7; c++) {
                    if (_board[r][c] == player && _board[r + 1][c] == player && _board[r + 2][c] == player &&
                        _board[r + 3][c] == player && !end) {
                        end_game();
                        end = true;
                    }
                }
            }
            for (int r = 0; r < 6; r++) {
                for (int c = 0; c < 4; c++) {
                    if (_board[r][c] == player && _board[r][c + 1] == player && _board[r][c + 2] == player &&
                        _board[r][c + 3] == player&& !end) {
                        end_game();
                        end = true;
                    }
                }
            }
            for (int r = 5; r > 2; r--) {
                for (int c = 0; c < 4; c++) {
                    if (_board[r][c] == player && _board[r - 1][c + 1] == player && _board[r - 2][c + 2] == player &&
                        _board[r - 3][c + 3] == player&& !end) {
                        end_game();
                        end = true;
                    }
                }
            }

            // usual move
            bool skip = false;
            for (int i = 0; i < static_cast<int>(std::size(_board[0])) && !end; i++) {
                if (_board[0][i] == " ") {
                    next_player();
                    co_await send_message();
                    skip = true;
                    break;
                }
            }
            if (skip) {
                continue;
            }

            if(end) {
                break;
            }

            // draw
            message.components.clear();
            message.embeds[0]
                .set_title("Game over")
                .set_description("Players " + dpp::utility::user_mention(get_players()[0]) + " and " +
                                  dpp::utility::user_mention(get_players()[1]) +
                                 " tried their best, but the game ended in a *DRAW!!!*")
                .set_color(dpp::colors::yellow);

            message.embeds[0].set_image(add_image(message, generate_image()));
            _data.bot->event_edit_original_response(event, message);
            remove_player(USER_REMOVE_REASON::DRAW,get_current_player());
            remove_player(USER_REMOVE_REASON::DRAW,get_current_player());
            break;
        }
        co_return;
    }
} // namespace gb
