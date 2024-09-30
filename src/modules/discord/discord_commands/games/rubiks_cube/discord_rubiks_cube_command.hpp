//
// Created by ilesik on 9/18/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

  /**
   * @class Discord_rubiks_cube_command
*

   */
  class Discord_rubiks_cube_command: public Discord_game_command {
  public:
    /**
     * @brief Constructs a Discord_rubiks_cube_command object.
     *
     * @param name The name of the module.
     * @param deps A list of module dependencies required for this command.
     */
    Discord_rubiks_cube_command(const std::string& name, const std::vector<std::string>& deps)
      : Discord_game_command(name, deps) {};
  };

} // namespace gb
