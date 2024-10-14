//
// Created by ilesik on 10/14/24.
//

#include "discord_puzzle_15_game.hpp"

namespace gb {
    Discord_puzzle_15_game::Discord_puzzle_15_game(Game_data_initialization &_data,
                                                   const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}

    void Discord_puzzle_15_game::prepare_message(dpp::message &message) {
        message.embeds[0]
            .set_title("Puzzle 15 Game")
            .set_description(std::format(
                "Player: {}\nMoves amount: {}\nTimeout: <t:{}:R>", dpp::utility::user_mention(get_current_player()),
                _engine.get_moves_cnt(),
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                        .count() +
                    +60))
            .set_color(dpp::colors::blue);
        auto p_m = _engine.get_possible_moves();
        message.components.clear();
        message
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("0")
                                                  .set_emoji("clear", 1015646216509468683)

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(std::ranges::find(p_m, puzzle_15::UP) == p_m.end())
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("up")
                                                  .set_emoji("⬆️"))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("1")
                                                  .set_emoji("clear", 1015646216509468683)))
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(std::ranges::find(p_m, puzzle_15::LEFT) == p_m.end())
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("left")
                                                  .set_emoji("⬅️")

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("2")
                                                  .set_emoji("clear", 1015646216509468683))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(std::ranges::find(p_m, puzzle_15::RIGHT) == p_m.end())
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("right")
                                                  .set_emoji("➡️")))
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("3")
                                                  .set_emoji("clear", 1015646216509468683)

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(std::ranges::find(p_m, puzzle_15::DOWN) == p_m.end())
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("down")
                                                  .set_emoji("⬇️"))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("4")
                                                  .set_emoji("clear", 1015646216509468683)));

        message.embeds[0].set_image(add_image(message, create_image()));
    }
    Image_ptr Discord_puzzle_15_game::create_image() {
        _img_cnt++;
        auto field = _engine.get_field();
        int image_size = 256;
        int grid_size = 4;
        int cell_size = image_size / grid_size;
        Image_ptr img = _data.image_processing->create_image({image_size, image_size}, {0, 0, 0});
        for (int i = 0; i <= grid_size; ++i) {
            int coord = i * cell_size;
            img->draw_line(Vector2i(coord, 0), Vector2i(coord, image_size), {255, 255, 255}, 2);
            img->draw_line(Vector2i(0, coord), Vector2i(image_size, coord), {255, 255, 255}, 2);
        }
        // Draw the numbers inside the grid cells
        for (int row = 0; row < grid_size; ++row) {
            for (int col = 0; col < grid_size; ++col) {
                int value = field[row][col];
                if (value != 0) {
                    // Assume 0 represents the empty cell
                    std::string text = std::to_string(value);
                    double font_scale = 0.7;
                    int thickness = 2;
                    Vector2i text_size = img->get_text_size(text, font_scale, thickness);
                    Vector2i text_org((col * cell_size) + (cell_size - text_size.x) / 2,
                                      (row * cell_size) + (cell_size + text_size.y) / 2);

                    // Check if the number is in the correct position
                    int correct_row = (value - 1) / grid_size;
                    int correct_col = (value - 1) % grid_size;
                    Color text_color =
                        (row == correct_row && col == correct_col) ? Color(0, 255, 0) : Color(255, 255, 255);

                    img->draw_text(text, text_org, font_scale, text_color, thickness);
                }
            }
        }

        return img;
    }

    dpp::task<void> Discord_puzzle_15_game::run(dpp::slashcommand_t sevent) {
        game_start(sevent.command.channel_id, sevent.command.guild_id);
        _engine.shuffle();
        dpp::message message;
        message.add_embed(dpp::embed());
        message.channel_id = sevent.command.channel_id;
        message.guild_id = sevent.command.guild_id;
        prepare_message(message);
        dpp::task<Button_click_return> button_click_awaitable =
            _data.button_click_handler->wait_for(message, {get_current_player()}, 60);
        _data.bot->reply(sevent, message);
        Button_click_return r;
        while (true) {
            r = co_await button_click_awaitable;
            if (r.second) {
                // timeout
                std::string desc = "If you still want to play, you can create new game.\nPlayer " + dpp::utility::user_mention(
                                               get_current_player()) + " lost his game.\n";
                message.embeds[0].set_color(dpp::colors::red).set_title("Game Timeout.").set_description(desc);
                message.components.clear();
                if (message.id == 0) {
                    _data.bot->message_edit(message);
                }
                else {
                    _data.bot->message_edit(message);
                }

                remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                break;
            }
            dpp::button_click_t event = r.first;
            message.id = event.command.message_id;
            puzzle_15::Direction d;
            switch (event.custom_id[0]) {
                case 'u':
                    d = puzzle_15::UP;
                break;
                case 'd':
                    d = puzzle_15::DOWN;
                break;
                case 'l':
                    d = puzzle_15::LEFT;
                break;
                default:
                    d = puzzle_15::RIGHT;
                break;
            };
            _engine.move(d);
            if (_engine.is_win()) {
                message.components.clear();
                message.embeds[0].set_color(dpp::colors::green)
                        .set_title("Game over!!")
                        .set_description(
                            "Player: " + dpp::utility::user_mention(get_current_player()) +
                            " solved puzzle and **won** the game.");
                message.embeds[0].set_image(add_image(message, create_image()));
                _data.bot->reply(event,message);
                remove_player(USER_REMOVE_REASON::WIN,get_current_player());
                break;
            }
            prepare_message(message);
            button_click_awaitable =
                _data.button_click_handler->wait_for(message, {get_current_player()}, 60);
            _data.bot->reply(event, message);
        }

        nlohmann::json json;
        json["moves_cnt"] = _engine.get_moves_cnt();
        game_stop(json.dump());
        co_return;
    }

    std::vector<std::pair<std::string, image_generator_t>> Discord_puzzle_15_game::get_image_generators() {
        return {};
    };
} // gb