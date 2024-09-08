//
// Created by ilesik on 9/4/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

  /**
   * @brief Base class for the Discord Dominoes command.
   *
   * This class serves as a specialized version of the `Discord_game_command` for the Dominoes game.
   */
  class Discord_dominoes_command : public Discord_game_command {
  public:

    /**
     * @brief Constructor for the Discord Dominoes command.
     *
     * @param name The name of the module.
     * @param deps A vector of strings representing dependencies required by the module.
     */
    Discord_dominoes_command(const std::string& name, const std::vector<std::string>& deps)
      : Discord_game_command(name, deps) {};
  };

} // namespace gb
