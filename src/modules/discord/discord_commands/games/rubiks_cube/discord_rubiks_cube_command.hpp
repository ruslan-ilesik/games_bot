//
// Created by ilesik on 9/18/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

class Discord_rubiks_cube_command: public Discord_game_command {
public:
  Discord_rubiks_cube_command(const std::string& name, const std::vector<std::string>& deps)
    : Discord_game_command(name, deps) {};
};

} // gb
