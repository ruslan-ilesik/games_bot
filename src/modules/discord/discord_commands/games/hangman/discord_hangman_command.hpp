//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

class Discord_hangman_command: public Discord_game_command {
public:
  Discord_hangman_command(const std::string& name, const std::vector<std::string>& deps)
      : Discord_game_command(name, deps) {};
};

} // gb
