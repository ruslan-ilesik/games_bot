//
// Created by ilesik on 9/15/24.
//

#include "discord_chess_game.hpp"


namespace gb {

    Vector2i Discord_chess_game::chess_board_cords_to_numbers(std::string to_convert) {
        return {static_cast<int>(to_convert[0]) - 97, 8 - (to_convert[1] - '0')};
    }

    Image_ptr Discord_chess_game::generate_image() {
        _img_cnt++;
        int size = 512;
        int border = size / 12;
        int offset_y = border;
        int offset_x = border;
        int img_size = (size - border) / 8;
        int add = size / 256;
        Image_ptr img = _data.image_processing->create_image({size, size}, {0, 0, 0});
        Image_ptr board = _data.image_processing->cache_get("chess_board", {size - border, size - border});
        img->overlay_image(board, {offset_x, offset_y});
        // draw selected figure
        if (!_selected_figure.empty()) {
            Vector2i pos = chess_board_cords_to_numbers(_selected_figure);
            if (get_current_player_index()) {
                Vector2d s(size - (pos.x * img_size + add * 2), size - (pos.y * img_size + add * 2));
                img->draw_rectangle(s - Vector2i{img_size, img_size}, s, Color(0, 0, 255), -1);
            } else {
                Vector2d s(offset_x + pos.x * img_size + add, offset_y + pos.y * img_size + add);
                img->draw_rectangle(s, s + Vector2i{img_size, img_size}, Color(0, 0, 255), -1);
            }

            // draw moves
            for (std::string &i: _possible_places_to_go) {
                pos = chess_board_cords_to_numbers(i);
                if (get_current_player_index()) {
                    Vector2d s(size - (pos.x * img_size + add * 2), size - (pos.y * img_size + add * 2));
                    img->draw_rectangle(s - Vector2i{img_size, img_size}, s, Color(0, 255, 0), -1);
                } else {
                    Vector2d s(offset_x + pos.x * img_size + add, offset_y + pos.y * img_size + add);
                    img->draw_rectangle(s, s + Vector2i{img_size, img_size}, Color(0, 255, 0), -1);
                }
            }
        }

        // draw figures
        int x = 0;
        int y = 0;
        std::string t_b = std::string(_board);
        if (get_current_player_index()) {
            std::reverse(t_b.begin(), t_b.end());
        }
        for (auto &i: t_b) {
            if (i == '\n') {
                y++;
                x = 0;
            }

            if (std::isalpha(i)) {
                if ((i == (get_current_player_index() ? 'k' : 'K')) && _board.is_check()) {
                    Vector2d s(offset_x + x / 2 * img_size + add, offset_y + y * img_size + add);
                    img->draw_rectangle(s, s + Vector2i{img_size, img_size}, Color(255, 0, 0), -1);
                }
                std::string cache_name =
                    "chess_" + (toupper(i) == i ? std::string(1, tolower(i)) + "_" : std::string(1, i));
                Image_ptr tmp = _data.image_processing->cache_get(cache_name, {img_size, img_size});
                img->overlay_image(tmp, {offset_x + x / 2 * img_size + add, offset_y + y * img_size + add});
            }
            x++;
        }


        // draw numbers
        for (int i = 8; i > 0; i--) {
            img->draw_text(std::to_string(get_current_player_index() ? 9 - i : i),
                           {border / 3, border + (9 - i) * img_size - img_size / 4}, border / 25, {220, 220, 220},
                           border / 25);
        }

        // draw letters
        x = 0;
        for (const std::string &i: {"a", "b", "c", "d", "e", "f", "g", "h"}) {
            img->draw_text(i,
                           {img_size / 4 + border + (get_current_player_index() ? 7 - x : x) * img_size,
                            static_cast<int>(border / 1.5)},
                           border / 25, {220, 220, 220}, border / 25);
            x++;
        }
        _choose_figure = !_choose_figure;
        return img;
    }

    Discord_chess_game::Discord_chess_game(Game_data_initialization &_data,
                                           const std::vector<dpp::snowflake> &players) : Discord_game(_data, players) {}

    dpp::task<void> Discord_chess_game::run(dpp::button_click_t event, int timeout) {
        game_start(event.command.channel_id, event.command.guild_id);
        dpp::message message;
        message.add_embed(dpp::embed());
        message.id = event.command.message_id;
        message.channel_id = event.command.channel_id;
        message.guild_id = event.command.guild_id;
        dpp::task<Button_click_return> button_click_awaitable;
        Button_click_return r;
        time_t clock = time(nullptr) + timeout;

        auto send_message = [&]() -> dpp::task<void> {

            message.components.clear();
            message.embeds[0].set_title("Chess game");
            dpp::component row = dpp::component().set_type(dpp::cot_action_row);

            if (_choose_figure) {
                message.embeds[0].set_description(
                    std::format("Turn: {}\nYour color is {}\nChoose figure to move\nTimeout: <t:{}:R>",
                                dpp::utility::user_mention(get_current_player()),
                                (get_current_player_index() ? "**black**" : "**white**"),
                                std::chrono::duration_cast<std::chrono::seconds>(
                                    std::chrono::system_clock::now().time_since_epoch())
                                        .count() +
                                    (clock - time(nullptr))));

                std::vector<std::string> used_options;
                for (chess::Move i: _board.legal_moves()) {
                    std::string uci = i.uci().substr(0, 2);
                    auto peace = _board.piece_at(
                        std::distance(chess::SQUARE_NAMES,
                                      std::find(std::begin(chess::SQUARE_NAMES), std::end(chess::SQUARE_NAMES), uci)));
                    auto emojie = chess::emojis[peace ? peace->symbol() : '.'];
                    if (std::find(used_options.begin(), used_options.end(), uci) == used_options.end()) {
                        row.add_component(dpp::component()
                                              .set_type(dpp::cot_button)
                                              .set_id(uci)
                                              .set_label(uci)
                                              .set_style(dpp::cos_secondary)
                                              .set_emoji(std::get<0>(emojie), std::get<1>(emojie)));
                        if (row.components.size() == 5) {
                            message.add_component(row);
                            row = dpp::component().set_type(dpp::cot_action_row);
                        }
                        used_options.push_back(uci);
                    }
                }
                if (row.components.size()) {
                    message.add_component(row);

                }
                message.embeds[0].set_image(add_image(message, generate_image()));
                button_click_awaitable = _data.button_click_handler->wait_for_with_reply(
                    message, {get_current_player()},
                    (clock - time(nullptr)));
                _data.bot->event_edit_original_response(event, message);
                r = co_await button_click_awaitable;
            } else {
                message.embeds[0].set_description(
                    std::format("Turn: {}\nYour color is {}\nChoose where to place figure\nTimeout: <t:{}:R>",
                                dpp::utility::user_mention(get_current_player()),
                                (get_current_player_index() ? "**black**" : "**white**"),
                                std::chrono::duration_cast<std::chrono::seconds>(
                                    std::chrono::system_clock::now().time_since_epoch())
                                        .count() +
                                    (clock - time(nullptr))));
                _possible_places_to_go.clear();
                auto p_m = _board.legal_moves();
                int counter = 0;

                for (chess::Move i: p_m) {
                    std::string u = i.uci();
                    if (u.find(_selected_figure) == 0) {
                        counter++;
                        _possible_places_to_go.push_back(u.substr(2, 2));
                        if ((counter <= 23 && !_next) || (counter > 23 && _next)) {
                            auto peace = _board.piece_at(std::distance(
                                chess::SQUARE_NAMES, std::find(std::begin(chess::SQUARE_NAMES),
                                                               std::end(chess::SQUARE_NAMES), u.substr(2, 2))));
                            auto emojie = chess::emojis[peace ? peace->symbol() : '.'];
                            if (u.size() == 5) {
                                emojie = chess::emojis[u[4]];
                            }
                            row.add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_id(u)
                                                  .set_label(u.substr(2, 2))
                                                  .set_style(dpp::cos_primary)
                                                  .set_emoji(std::get<0>(emojie), std::get<1>(emojie)));
                            if (row.components.size() == 5) {
                                message.add_component(row);
                                row = dpp::component().set_type(dpp::cot_action_row);
                            }
                        }
                    }
                }
                if (row.components.size() != 5) {
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_id("back")
                                          .set_label("Back")
                                          .set_emoji("🔙")
                                          .set_style(dpp::cos_danger));
                    if (_possible_places_to_go.size() > 24 && !_next) {

                        if (row.components.size() == 5) {
                            message.add_component(row);
                            row = dpp::component().set_type(dpp::cot_action_row);
                        }
                        row.add_component(dpp::component()
                                              .set_type(dpp::cot_button)
                                              .set_id("next")
                                              .set_label("Next")
                                              .set_emoji("➡️")
                                              .set_style(dpp::cos_primary));
                    }
                    message.add_component(row);
                } else {
                    message.add_component(row);
                    row = dpp::component().set_type(dpp::cot_action_row);
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_id("back")
                                          .set_label("Back")
                                          .set_emoji("🔙")
                                          .set_style(dpp::cos_danger));
                    if (_possible_places_to_go.size() > 24 && !_next) {
                        row.add_component(dpp::component()
                                              .set_type(dpp::cot_button)
                                              .set_id("next")
                                              .set_label("Next")
                                              .set_emoji("➡️")
                                              .set_style(dpp::cos_primary));
                    }
                    message.add_component(row);
                }
                message.embeds[0].set_image(add_image(message, generate_image()));
                button_click_awaitable = _data.button_click_handler->wait_for_with_reply(
                    message, {get_current_player()},
                    (clock - time(nullptr)));
                _data.bot->event_edit_original_response(event, message);
                r = co_await button_click_awaitable;
            }
            co_return;
        };
        event.reply(dpp::ir_update_message,"Game is starting");
        co_await send_message();

        while (1) {
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


            if (event.custom_id == "back") {
                if (_next) {
                    _next = false;
                    _choose_figure = !_choose_figure;
                } else {
                    _choose_figure = true;
                    _selected_figure = "";
                }
                co_await send_message();
            } else if (event.custom_id == "next") {
                _next = true;
                _choose_figure = !_choose_figure;
                co_await send_message();
            } else if (_choose_figure) {
                _next = false;
                _selected_figure = "";
               // std::cout << event.custom_id << '\n';
                _board.push(chess::Move::from_uci(event.custom_id));

                if (!_board.legal_moves().count()) {
                    if (!_board.is_check()) {
                        // draw
                        message.components.clear();
                        message.embeds[0]
                            .set_title("Game over")
                            .set_description("Players " + dpp::utility::user_mention(get_players()[0]) + " and " +
                                             dpp::utility::user_mention(get_players()[1]) +
                                             " tried their best, but the game ended in a *DRAW!!!*")
                            .set_color(dpp::colors::yellow);
                        _data.bot->event_edit_original_response(event, message);
                        remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
                        remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
                        break;
                    } else {
                        _data.achievements_processing->activate_achievement(
                            "Chess.com wait for me", get_current_player(), event.command.channel_id);
                        if (_moves_amount < 20) {
                            _data.achievements_processing->activate_achievement("Blitzkrieg", get_current_player(),
                                                                                event.command.channel_id);
                        }
                        message.components.clear();
                        std::string desc ="Player "+dpp::utility::user_mention(get_current_player())+ " *won* the game.";
                        next_player();
                        desc += "\nPlayer "+dpp::utility::user_mention(get_current_player())+" will be luckier next time";
                        message.embeds[0].set_title("Game over").set_description(desc).set_color(dpp::colors::blue);
                        message.embeds[0].set_image(add_image(message,generate_image()));
                        _data.bot->event_edit_original_response(event,message);
                        remove_player(USER_REMOVE_REASON::LOSE,get_current_player());
                        remove_player(USER_REMOVE_REASON::WIN,get_current_player());
                        break;
                    }
                } else if (_board.is_fivefold_repetition() || _board.is_insufficient_material()) {
                    // draw
                    message.components.clear();
                    message.embeds[0]
                        .set_title("Game over")
                        .set_description("Players " + dpp::utility::user_mention(get_players()[0]) + " and " +
                                         dpp::utility::user_mention(get_players()[1]) +
                                         " tried their best, but the game ended in a *DRAW!!!*")
                        .set_color(dpp::colors::yellow);
                    message.embeds[0].set_image(add_image(message,generate_image()));
                    _data.bot->event_edit_original_response(event, message);
                    remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
                    remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
                    break;
                } else {
                    _moves_amount++;
                    clock = time(nullptr) + timeout;
                    next_player();
                    co_await send_message();
                }
            } else {
                _selected_figure = event.custom_id;
                co_await send_message();
            }
        }
        game_stop();
        co_return;
    }

    std::vector<std::pair<std::string, image_generator_t>> Discord_chess_game::get_image_generators() {
        std::vector<std::pair<std::string, image_generator_t>> generators;
        for (const char &i: {'b', 'k', 'n', 'p', 'q', 'r'}) {
            for (const std::string &s: {"", "_"}) {
                std::string file_name = "./data/img/chess/" + std::string(1, i) + s + ".png";
                std::string gen_name = "chess_" + std::string(1, i) + s;
                generators.emplace_back(gen_name,
                                        [=](const Image_processing_ptr &image_processing, const Vector2i &resolution) {
                                            Image_ptr img = image_processing->create_image(file_name);
                                            img->resize(resolution);
                                            return img;
                                        });
            }
        }

        generators.emplace_back("chess_board",
                                [=](const Image_processing_ptr &image_processing, const Vector2i &resolution) {
                                    Image_ptr img = image_processing->create_image("./data/img/chess/board.png");
                                    img->resize(resolution);
                                    return img;
                                });
        return generators;
    }
} // namespace gb
