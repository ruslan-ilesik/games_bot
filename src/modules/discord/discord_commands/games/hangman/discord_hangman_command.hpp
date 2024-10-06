//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

  /**
   * @brief Base class for the Discord Hangman command.
   *
   * This class serves as a specialized version of the `Discord_game_command` for the Hangman game.
   */
  class Discord_hangman_command : public Discord_game_command {
  public:

    /**
     * @brief Constructor for the Discord Hangman command.
     *
     * @param name The name of the module.
     * @param deps A vector of strings representing dependencies required by the module.
     */
    Discord_hangman_command(const std::string& name, const std::vector<std::string>& deps)
      : Discord_game_command(name, deps) {};
  };

} // namespace gb
