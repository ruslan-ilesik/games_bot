//
// Created by ilesik on 7/31/24.
//

#include "discord_tic_tac_toe_game.hpp"

namespace gb {
    void Discord_tic_tac_toe_game::create_image(dpp::message &m, dpp::embed &embed) {
        _img_cnt++;
        double size = 256;
        auto base = _data.image_processing->cache_get("tic_tac_toe_base", {256, 256});
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                base->draw_text(
                    this->board[y][x] == SIGNS::EMPTY ? " " : (this->board[y][x] == SIGNS::O ? "0" : "X"),
                    {static_cast<int>(size / 24 + (x * (size / 3))), static_cast<int>(size / 3.5 + y * (size / 3))},
                    size / 85,
                    this->board[y][x] == static_cast<SIGNS>(get_current_player_index()) ? Color{0, 255, 0}
                                                                                        : Color{255, 0, 0},
                    size / 47);
            }
        }
        embed.set_image(add_image(m, base));
    }

    void Discord_tic_tac_toe_game::create_components(dpp::message &m) {
        for (int i = 0; i < static_cast<int>(std::size(this->board)); i++) {
            dpp::component row = dpp::component().set_type(dpp::cot_action_row);
            for (int j = 0; j < static_cast<int>(std::size(this->board[i])); j++) {
                row.add_component(dpp::component()
                                      .set_type(dpp::cot_button)
                                      .set_label(this->board[i][j] == SIGNS::EMPTY
                                                     ? "\u200F"
                                                     : (this->board[i][j] == SIGNS::O ? "0" : "X"))
                                      .set_id(std::to_string(i) + std::to_string(j))
                                      .set_disabled(this->board[i][j] != SIGNS::EMPTY)
                                      .set_style(this->board[i][j] == static_cast<SIGNS>(get_current_player_index())
                                                     ? dpp::cos_success
                                                 : this->board[i][j] == SIGNS::EMPTY ? dpp::cos_primary
                                                                                     : dpp::cos_danger));
            }
            m.add_component(row);
        }
    }
    dpp::message Discord_tic_tac_toe_game::win(bool timeout) {
        dpp::message m;
        dpp::embed embed;
        embed.set_color(dpp::colors::green).set_title("Game over!");

        if (timeout) {
            embed.set_description(std::format("Player {} won the game!\n{} Will be luckier next time.",
                                              dpp::utility::user_mention(next_player()),
                                              dpp::utility::user_mention(next_player())));
            next_player();
            create_image(m, embed);
            remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
            remove_player(USER_REMOVE_REASON::WIN, get_current_player());
        } else {
            _data.achievements_processing->activate_achievement("Blind opponent => your win", get_current_player(),
                                                                _event.command.channel_id);
            next_player();
            _data.achievements_processing->activate_achievement("Blindness happens", get_current_player(),
                                                                _event.command.channel_id);
            next_player();

            embed.set_description(std::format("Player {} won the game!\n{} will be luckier next time.",
                                              dpp::utility::user_mention(get_current_player()),
                                              dpp::utility::user_mention(next_player())));
            next_player();
            create_image(m, embed);
            remove_player(USER_REMOVE_REASON::WIN, get_current_player());
            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
        }
        return m.add_embed(embed);
    }

    dpp::message Discord_tic_tac_toe_game::draw() {
        dpp::message m;
        dpp::embed embed;
        embed.set_color(dpp::colors::yellow).set_title("Game over!");
        create_image(m, embed);
        embed.set_description(std::format("Players {} and {} played a draw!",
                                          dpp::utility::user_mention(get_current_player()),
                                          dpp::utility::user_mention(next_player())));

        remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
        remove_player(USER_REMOVE_REASON::DRAW, next_player());
        return m.add_embed(embed);
    }

    Discord_tic_tac_toe_game::Discord_tic_tac_toe_game(Game_data_initialization &_data,
                                                       const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_tic_tac_toe_game::get_image_generators() {
        image_generator_t base = [](const Image_processing_ptr &image_processing, const Vector2i &resolution) {
            auto image = image_processing->create_image(resolution, {0, 0, 0});
            if (resolution.x != resolution.y) {
                throw std::runtime_error("tic tac toe base image supports only squared image");
            }
            int size = resolution.x;
            for (int y = 0; y < 3; y++) {
                image->draw_line({0, (y * size / 3)}, {size, (y * size / 3)}, {255, 255, 255}, size / 47);
                image->draw_line({(y * size / 3), 0}, {(y * size / 3), size}, {255, 255, 255}, size / 47);
            }
            image->draw_line({0, size}, resolution, {255, 255, 255}, size / 47);
            image->draw_line({size, 0}, resolution, {255, 255, 255}, size / 47);
            return image;
        };
        return {{"tic_tac_toe_base", base}};
    }
    dpp::task<void> Discord_tic_tac_toe_game::run(const dpp::button_click_t &event) {
        game_start(event.command.channel_id,event.command.guild_id);
        _event = event;
        while (1) {
            dpp::message m;
            dpp::embed embed;
            embed.set_color(dpp::colors::blue)
                .set_title("Tic Tac Toe game.")
                .set_description(std::format("Timeout: <t:{}:R>\nTurn: {}\nYour sign: **{}**\nSelect where to "
                                             "place your sign.",
                                             std::chrono::duration_cast<std::chrono::seconds>(
                                                 std::chrono::system_clock::now().time_since_epoch())
                                                     .count() +
                                                 60,
                                             dpp::utility::user_mention(get_current_player()),
                                             static_cast<SIGNS>(get_current_player_index()) == SIGNS::O ? "0" : "X"));
            create_image(m, embed);
            create_components(m);
            m.add_embed(embed);
            auto button_click_awaiter = _data.button_click_handler->wait_for(m, {get_current_player()}, 60);
            _data.bot->reply(_event, m);
            Button_click_return r = co_await button_click_awaiter;
            if (r.second) {
                dpp::message m = win(true);
                m.id = _event.command.message_id;
                m.channel_id = _event.command.channel_id;
                _data.bot->event_edit_original_response(_event,m);
                break;
            }
            _event = r.first;

            std::string id = _event.custom_id;
            if (!std::isdigit(_event.custom_id[0]) || !std::isdigit(_event.custom_id[1])) {
                id = "00";
            }
            board[static_cast<int>(id[0]) - '0'][static_cast<int>(id[1]) - '0'] =
                static_cast<SIGNS>(get_current_player_index());
            bool ended = false;
            for (auto i: board) {
                if (i[0] == i[1] && i[1] == i[2] && i[0] != SIGNS::EMPTY) {
                    // won
                    _data.bot->reply(_event, win());
                    ended = true;
                    break;
                }
            }
            for (int i = 0; i < static_cast<int>(std::size(board)) && !ended; i++) {
                if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[2][i] != SIGNS::EMPTY) {
                    // won
                    _data.bot->reply(_event, win());
                    ended = true;
                    break;
                }
            }

            if (!ended && (((board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
                            (board[0][2] == board[1][1] && board[0][2] == board[2][0])) &&
                           board[1][1] != SIGNS::EMPTY)) {
                // won
                _data.bot->reply(_event, win());
                ended = true;
                break;
            }

            if (!ended) {
                bool has_empty = false;
                for (auto &i: board) {
                    for (auto &j: i) {
                        has_empty |= j == SIGNS::EMPTY;
                    }
                }
                if (!has_empty) {
                    // draw
                    _data.bot->reply(_event, draw());
                    ended = true;
                    break;
                }
            }
            if (ended) {
                break;
            }

            // game has not finished
            next_player();
        }
        game_stop();
        co_return;
    }
} // namespace gb
