//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "../discord_game.hpp"

namespace gb {

    class Discord_tic_tac_toe_game: public Discord_game{
    public:
        Discord_tic_tac_toe_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players)
            : Discord_game(_data, players) {
        }
    };

} // gb

