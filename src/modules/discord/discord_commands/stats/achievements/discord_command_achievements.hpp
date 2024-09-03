//
// Created by ilesik on 8/30/24.
//

#pragma once
#include <src/modules/discord/discord_commands/discord_general_command.hpp>

namespace gb {

  /**
   * @class Discord_command_achievements
   * @brief Abstract base class for handling Discord achievements command functionality.
   *
   * This class extends the Module class and provides an interface for implementing
   * achievements commands that can be used in a Discord bot to show users information about their achievements
   */
  class Discord_command_achievements : public Discord_general_command {
  public:
    /**
     * @brief Constructor for Discord_command_achievements.
     *
     * Initializes the base Module class with the given name and dependencies.
     *
     * @param name The name of the module.
     * @param dependencies A vector of module names that this module depends on.
     */
    Discord_command_achievements(const std::string& name, const std::vector<std::string>& dependencies)
        : Discord_general_command(name, dependencies) {};

  };

} // gb
