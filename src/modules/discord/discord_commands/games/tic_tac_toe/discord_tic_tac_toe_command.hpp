//
// Created by ilesik on 7/31/24.
//

#pragma once

#include "../discord_game_command.hpp"

namespace gb{

    class Discord_tic_tac_toe_command : public Discord_game_command{
    public:
        Discord_tic_tac_toe_command(const std::string& name, const std::vector<std::string>& dependencies): Discord_game_command(name,dependencies){};


    };

} //gb
