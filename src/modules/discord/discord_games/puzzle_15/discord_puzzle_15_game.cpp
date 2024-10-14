//
// Created by ilesik on 10/14/24.
//

#include "discord_puzzle_15_game.hpp"

namespace gb {
    Discord_puzzle_15_game::Discord_puzzle_15_game(Game_data_initialization &_data,
                                                   const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_puzzle_15_game::get_image_generators() {
        return {};
    };
} // gb