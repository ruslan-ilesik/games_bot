//
// Created by ilesik on 8/30/24.
//

#pragma once

#include <src/modules/discord/discord_achievements_processing/discord_achievements_processing.hpp>
#include "./discord_command_achievements.hpp"
#include <src/modules/discord/discord_bot/discord_bot.hpp>
#include <src/modules/discord/discord_command_handler/discord_command_handler.hpp>

namespace gb {

  /**
   * @class Discord_command_achievements_impl
   * @brief Implements the `/achievements` command for Discord to manage and display user achievements.
   *
   * Integrates with `Discord_achievements_processing`, `Discord_command_handler`, and `Discord_bot` modules.
   */
  class Discord_command_achievements_impl : public Discord_command_achievements {
    Discord_achievements_processing_ptr _achievements_processing; ///< Pointer to achievements processing module.

  public:
    /**
     * @brief Constructs the implementation, initializing dependencies.
     */
    Discord_command_achievements_impl();

    /**
     * @brief Initializes the command and sets up command handling.
     *
     * @param modules Reference to the system modules.
     */
    void init(const Modules &modules) override;

    /**
     * @brief Starts the module's operation (currently does nothing).
     */
    void run() override;
  };

  /**
   * @brief Factory function to create an instance of `Discord_command_achievements_impl`.
   *
   * @return Module_ptr A shared pointer to the created instance.
   */
  extern "C" Module_ptr create();

} // namespace gb
