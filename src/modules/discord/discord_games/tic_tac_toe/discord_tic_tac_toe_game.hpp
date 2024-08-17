//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "../discord_game.hpp"

namespace gb {
    class Discord_tic_tac_toe_game : public Discord_game {
        enum class SIGNS { X = 0, O = 1, EMPTY };

        std::array<std::array<SIGNS, 3>, 3> board = {
            {
                {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
                {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
                {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY}
            }
        };


        void create_image(dpp::message &m, dpp::embed &embed) {
            auto base = _data.image_processing->cache_get("tic_tac_toe_base", {256, 256});

            embed.set_image(add_image(m, base));
        }

        void create_components(dpp::message &m) {
            for (int i = 0; i < static_cast<int>(std::size(this->board)); i++) {
                dpp::component row = dpp::component().set_type(dpp::cot_action_row);
                for (int j = 0; j < static_cast<int>(std::size(this->board[i])); j++) {
                    row.add_component(dpp::component().
                        set_type(dpp::cot_button).
                        set_label(this->board[i][j] == SIGNS::EMPTY
                                      ? "\u200F"
                                      : (this->board[i][j] == SIGNS::O ? "0" : "X")).
                        set_id(std::to_string(i) + std::to_string(j)).
                        set_disabled(this->board[i][j] != SIGNS::EMPTY).
                        set_style(this->board[i][j] == static_cast<SIGNS>(get_current_player_index())
                                      ? dpp::cos_success
                                      : this->board[i][j] == SIGNS::EMPTY
                                            ? dpp::cos_primary
                                            : dpp::cos_danger)
                    );
                }
                m.add_component(row);
            }
        }

    public:
        Discord_tic_tac_toe_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t> > get_image_generators() {
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

        dpp::task<void> run(const dpp::button_click_t &_event) {
            game_start();
            dpp::button_click_t event = _event;
            while(1) {
                dpp::message m;
                dpp::embed embed;
                embed.set_color(dpp::colors::blue)
                    .set_title("Tic Tac Toe game.")
                    .set_description(std::format("Timeout: <t:{}:R>\nTurn: {}",
                                                 std::chrono::duration_cast<std::chrono::seconds>(
                                                     std::chrono::system_clock::now().time_since_epoch()).count() + 60,
                                                 dpp::utility::user_mention(get_current_player())
                    ));
                create_image(m, embed);
                create_components(m);
                m.add_embed(embed);
                auto button_click_awaiter = _data.button_click_handler->wait_for(m,{get_current_player()},60);
                _data.bot->reply(event, m);
                Button_click_return r = co_await button_click_awaiter;
                if (r.second) {
                    break;
                }
                event = r.first;

            }
            get_current_player();
            game_stop();
        }
    };
} // gb
