//
// Created by ilesik on 9/26/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

  /**
   * @class Discord_sudoku_command
   * @brief A class that handles the Sudoku game command for the Discord bot.
   *
   * This class inherits from Discord_game_command and is responsible for managing
   * the Sudoku game interactions within a Discord server.
   */
  class Discord_sudoku_command: public Discord_game_command {
  public:
    /**
     * @brief Constructs a Discord_sudoku_command object.
     *
     * @param name The name of the module.
     * @param deps A list of module dependencies required for this command.
     */
    Discord_sudoku_command(const std::string& name, const std::vector<std::string>& deps)
        : Discord_game_command(name, deps) {};
  };

} // namespace gb
