//
// Created by ilesik on 9/15/24.
//

#include "discord_chess_game.hpp"


namespace gb {
    Discord_chess_game::Discord_chess_game(Game_data_initialization &_data,
                                           const std::vector<dpp::snowflake> &players) : Discord_game(_data, players) {}

    dpp::task<void> Discord_chess_game::run(dpp::button_click_t event) {

        co_return;
    }
    std::vector<std::pair<std::string, image_generator_t>> Discord_chess_game::get_image_generators() { return {}; }
} // gb