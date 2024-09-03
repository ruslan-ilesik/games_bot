//
// Created by ilesik on 9/1/24.
//

#pragma once
#include <src/modules/discord/discord_commands/discord_general_command.hpp>

namespace gb {

// Abstract class declaration for Discord_command_global_stats
class Discord_command_global_stats : public Discord_general_command{
public:
  // Constructor that initializes the command with a name and a list of dependencies
  Discord_command_global_stats(const std::string& name, const std::vector<std::string>& dependencies)
      : Discord_general_command(name, dependencies) {};
};

} // gb
