//
// Created by ilesik on 9/29/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

  /**
   * @brief Represents a command to initiate a 2048 game within Discord.
   *
   */
  class Discord_2048_command : public Discord_game_command {
  public:

    /**
     * @brief Constructs a new `Discord_2048_command` object.
     *
     * @param name The name of the command.
     * @param deps A vector of strings representing dependencies required for the command.
     */
    Discord_2048_command(const std::string& name, const std::vector<std::string>& deps)
        : Discord_game_command(name, deps) {};
  };

} // namespace gb
