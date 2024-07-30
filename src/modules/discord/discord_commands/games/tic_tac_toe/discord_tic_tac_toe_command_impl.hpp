//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "./discord_tic_tac_toe_command.hpp"

namespace gb {

    class Discord_tic_tac_toe_command_impl: public Discord_game_command {
    public:
        Discord_tic_tac_toe_command_impl(): Discord_game_command("discord_game_tic_tac_toe",{}){};
    };

} // gb


