//
// Created by ilesik on 9/26/24.
//

#include "./discord_sudoku_game.hpp"

namespace gb {
    Discord_sudoku_game::Discord_sudoku_game(Game_data_initialization &_data,
                                             const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_sudoku_game::get_image_generators() { return {}; }

    void Discord_sudoku_game::prepare_message(dpp::message &message) {
        message.components.clear();
        message.embeds[0]
            .set_title("Sudoku game")
            .set_description(std::format(
                "{}Player: {}\nAvailable mistakes: {}\nTimeout: <t:{}:R>\n{}",
                (_is_mistake ? "**YOU MADE A MISTAKE!!!**\n" : ""), dpp::utility::user_mention(get_current_player()),
                _available_mistakes,
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                        .count() +
                    +_timeout,
                (_state == 0 ? "Choose column to make action on."
                             : (_state == 1 ? "Choose row to make action on." : "Choose number to place."))

                    ));

        if (_state < 2) {
            for (int i = 0; i <= 1; i++) {
                dpp::component row = dpp::component().set_type(dpp::cot_action_row);
                for (int j = 0; j < (i ? 4 : 5); j++) {
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_label(std::to_string(i * 5 + j + 1))
                                          .set_id(std::to_string(i * 5 + j)));
                }
                message.add_component(row);
            }
        } else if (_engine.get_field()[_pos[0]][_pos[1]] == 0) {
            for (int i = 0; i <= 1; i++) {
                dpp::component row = dpp::component().set_type(dpp::cot_action_row);
                for (int j = 0; j < (i ? 4 : 5); j++) {
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_label(std::to_string(i * 5 + j + 1))
                                          .set_id(std::to_string(i * 5 + j + 1)));
                }
                message.add_component(row);
            }
        } else {
            message.add_component(dpp::component().set_type(dpp::cot_action_row));
        }
        if (_state > 0) {
            message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                .set_type(dpp::cot_button)
                                                                                .set_style(dpp::cos_danger)
                                                                                .set_label("back")
                                                                                .set_id("back")
                                                                                .set_emoji("⬅"));
        }
        message.embeds[0].set_image(add_image(message, create_image()));
    }

    Image_ptr Discord_sudoku_game::create_image() {
        _img_cnt++;
        int image_size = 256;
        int distance_between = (image_size) / 256; // px
        float size = (((image_size) -distance_between * 10) / 9.0);
        int numbers_offset = size;
        image_size += numbers_offset;
        Image_ptr img = _data.image_processing->create_image({image_size, image_size}, {255, 255, 255});
        auto field = _engine.get_field();
        // draw lines
        for (int i = 1; i < 9; i++) {
            int p = static_cast<int>(i * size + distance_between * (i) + static_cast<int>(i / 3) * distance_between);
            int width = (i % 3 == 0 ? distance_between * 2 : distance_between);
            img->draw_line({p, 0}, {p, (image_size - numbers_offset)}, {0, 0, 0}, width);
            img->draw_line({0, p}, {(image_size - numbers_offset), p}, {0, 0, 0}, width);
        }
        // draw numbers
        for (int x = 0; x < static_cast<int>(field.size()); x++) {
            for (int y = 0; y < static_cast<int>(field.size()); y++) {
                if (field[x][y]) {
                    img->draw_text(
                        std::to_string(field[x][y]),
                        Vector2i(static_cast<int>(x * size + distance_between * x + int(x / 3) * distance_between +
                                                  size / 8),
                                 static_cast<int>(y * size + distance_between * y + int(y / 3) * distance_between +
                                                  size - size / (4096 / (image_size - numbers_offset)))),
                        size / 28, {0, 0, 0}, size / 26);
                }
            }
        }
        // draw borders for numbers
        img->draw_rectangle(Vector2i(image_size - numbers_offset, 0), Vector2i(image_size, image_size), {0, 0, 0}, -1);
        img->draw_rectangle(Vector2i(0, image_size - numbers_offset), Vector2i(image_size, image_size), {0, 0, 0}, -1);

        // draw numbers on borders;
        for (int i = 0; i < 9; i++) {
            // draw right numbers
            img->draw_text(std::to_string(i + 1),
                           Vector2i(image_size - numbers_offset,
                                    static_cast<int>(i * size + distance_between * i + int(i / 3) * distance_between +
                                                     size - size / (4096 / (image_size - numbers_offset)))),
                           size / 28, _state != 0 ? Color{251, 192, 45} : Color{255, 255, 255}, size / 26);
            img->draw_text(
                std::to_string(i + 1), // text
                Vector2i(static_cast<int>((i - 1) * size + distance_between * i + int(i / 3) * distance_between + size),
                         image_size - size / (4096 / (image_size - numbers_offset))), // top-left position
                size / 28, _state != 1 ? Color(251, 192, 45) : Color(255, 255, 255), // font color
                size / 26);
        }
        // draw selections
        if (_state == 2 || _state == 1) {

            if (_state == 1) {
                int p = static_cast<int>(_pos[0] * size + distance_between * _pos[0] +
                                         int(_pos[0] / 3) * distance_between + distance_between);
                img->draw_rectangle({p, 0}, Vector2i{static_cast<int>(p + size), image_size - numbers_offset},
                                    {0, 0, 255, 0.25}, -1);
            } else {
                for (int y = 0; y < 9; y++) {
                    for (int x = 0; x < 9; x++) {
                        int pos_screen_x = static_cast<int>(x * size + distance_between * x +
                                                            int(x / 3) * distance_between + (x ? distance_between : 0));
                        int pos_screen_y = static_cast<int>(y * size + distance_between * y +
                                                            int(y / 3) * distance_between + (y ? distance_between : 0));

                        if (x == _pos[0] && y == _pos[1]) {
                            img->draw_rectangle({pos_screen_x, pos_screen_y},
                                                Vector2d{pos_screen_x + size, pos_screen_y + size}, {0, 0, 255, 0.5},
                                                -1);
                        } else if (x == _pos[0] || y == _pos[1]) {
                            img->draw_rectangle({pos_screen_x, pos_screen_y},
                                                Vector2d{pos_screen_x + size, pos_screen_y + size}, {0, 0, 255, 0.25},
                                                -1);
                        } else if (field[_pos[0]][_pos[1]] != 0 && field[x][y] == field[_pos[0]][_pos[1]]) {
                            img->draw_rectangle({pos_screen_x, pos_screen_y},
                                                Vector2d{pos_screen_x + size, pos_screen_y + size}, {0, 0, 255, 0.4},
                                                -1);

                        } else if (x / 3 == _pos[0] / 3 && y / 3 == _pos[1] / 3) {
                            img->draw_rectangle({pos_screen_x, pos_screen_y},
                                                Vector2d{pos_screen_x + size, pos_screen_y + size}, {0, 0, 255, 0.25},
                                                -1);
                        }
                    }
                }
            }
        }


        return img;
    }

    dpp::task<void> Discord_sudoku_game::run(dpp::slashcommand_t sevent) {
        game_start(sevent.command.channel_id, sevent.command.guild_id);
        dpp::message message;
        message.add_embed(dpp::embed());
        message.channel_id = sevent.command.channel_id;
        message.channel_id = sevent.command.channel_id;
        message.guild_id = sevent.command.guild_id;
        message.id = 0;

        Button_click_return r;
        _engine.create_seed();
        _engine.gen_puzzle();
        prepare_message(message);
        dpp::task<Button_click_return> button_click_awaitable =
            _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, _timeout);
        _data.bot->reply(sevent, message);
        r = co_await button_click_awaitable;
        dpp::button_click_t event;
        while (1) {
            _is_mistake = false;
            if (r.second) {
                // timeout
                _state = -1;
                _engine.end_game();
                message.components.clear();
                message.embeds[0]
                    .set_title("Game time out!")
                    .set_description(std::format("Player {} need to think faster next time",
                                                 dpp::utility::user_mention(get_current_player())))
                    .set_color(dpp::colors::red);
                message.embeds[0].set_image(add_image(message, create_image()));
                if (message.id !=0) {
                    _data.bot->event_edit_original_response(event,message);
                }
                else {
                    _data.bot->event_edit_original_response(sevent,message);
                }

                remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                break;
            }
            event = r.first;
            message.id = event.command.message_id;
            if (event.custom_id == "back") {
                _state--;
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, _timeout);
                _data.bot->event_edit_original_response(event, message);
                r = co_await button_click_awaitable;
                continue;
            }

            if (_state < 2) {
                _pos[_state] = std::stoi(event.custom_id);
                _state++;
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, _timeout);
                _data.bot->event_edit_original_response(event, message);
                r = co_await button_click_awaitable;
                continue;

            } else {
                _state = 0;
                int number = std::stoi(event.custom_id);
                if (!_engine.place(_pos, number)) {
                    _available_mistakes--;
                    if (_available_mistakes == 0) {
                        _state = -1;
                        _engine.end_game();
                        message.components.clear();
                        message.embeds[0]
                            .set_title("Game over!")
                            .set_description(std::format("Player {} made 3 mistakes and lost his game",
                                                         dpp::utility::user_mention(get_current_player())))
                            .set_color(dpp::colors::red);
                        message.embeds[0].set_image(add_image(message, create_image()));
                        _data.bot->event_edit_original_response(event, message);
                        remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                        break;
                    }
                    _is_mistake = true;
                    prepare_message(message);
                    button_click_awaitable =
                        _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, _timeout);
                    _data.bot->event_edit_original_response(event, message);
                    r = co_await button_click_awaitable;
                    continue;
                } else if (_engine.check_win()) {
                    _state = -1;
                    _engine.end_game();
                    message.components.clear();
                    message.embeds[0]
                        .set_title("Game over!")
                        .set_description(std::format("Player {} solved sudoku and **WON** this game",
                                                     dpp::utility::user_mention(get_current_player())))
                        .set_color(dpp::colors::green);
                    if (_available_mistakes == 3) {
                        _data.achievements_processing->activate_achievement("Grandpa", get_current_player(),
                                                                            event.command.channel_id);
                    }

                    message.embeds[0].set_image(add_image(message, create_image()));
                    _data.bot->event_edit_original_response(event, message);
                    remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                    break;
                }
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, _timeout);
                _data.bot->event_edit_original_response(event, message);
                r = co_await button_click_awaitable;
                continue;
            }
        }
        game_stop();
        co_return;
    }
} // namespace gb
