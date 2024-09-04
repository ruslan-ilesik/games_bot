//
// Created by ilesik on 9/4/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

class Discord_dominoes_command: public Discord_game_command{
public:

  Discord_dominoes_command(const std::string& name, const std::vector<std::string>& deps): Discord_game_command(name,deps){};
};

} // gb
