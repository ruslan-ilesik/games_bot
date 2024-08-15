//
// Created by ilesik on 7/31/24.
//

#include "discord_tic_tac_toe_game.hpp"

namespace gb {
    Discord_tic_tac_toe_game::Discord_tic_tac_toe_game(Game_data_initialization &_data,
        const std::vector<dpp::snowflake> &players): Discord_game(_data, players) {
    }
} // gb