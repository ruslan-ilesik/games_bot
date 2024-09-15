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
        return img;
    }

    Discord_chess_game::Discord_chess_game(Game_data_initialization &_data,
                                           const std::vector<dpp::snowflake> &players) : Discord_game(_data, players) {}

    dpp::task<void> Discord_chess_game::run(dpp::button_click_t event) {
        dpp::message message;
        message.add_embed(dpp::embed());
        message.id = event.command.message_id;
        message.channel_id = event.command.channel_id;
        message.guild_id = event.command.guild_id;
        while (1) {
        }
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
