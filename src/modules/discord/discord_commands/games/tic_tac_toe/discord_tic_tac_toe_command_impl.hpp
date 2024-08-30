//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "./discord_tic_tac_toe_command.hpp"
#include "src/modules/discord/discord/discord.hpp"
#include "src/modules/discord/discord_command_handler/discord_command_handler.hpp"

namespace gb {

    /**
     * @class Discord_tic_tac_toe_command_impl
     * @brief Implements the Tic-Tac-Toe game command for the Discord bot.
     *
     * This class handles the setup, execution, and teardown of the Tic-Tac-Toe game command in a Discord environment.
     * It inherits from the Discord_tic_tac_toe_command base class and overrides the init and stop methods.
     */
    class Discord_tic_tac_toe_command_impl : public Discord_tic_tac_toe_command {
        Discord_command_handler_ptr _command_handler; ///< Pointer to the command handler module.
        Discord_ptr _discord; ///< Pointer to the Discord module.

    public:
        /**
         * @brief Constructs a Discord_tic_tac_toe_command_impl object.
         *
         * Initializes the Tic-Tac-Toe command with required dependencies.
         */
        Discord_tic_tac_toe_command_impl();

        /**
         * @brief Initializes the command with the necessary modules.
         *
         * This method sets up the command handler, Discord module, and image processing.
         * It also registers the Tic-Tac-Toe command with the Discord bot.
         *
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the command and performs cleanup.
         *
         * This method removes the Tic-Tac-Toe command from the command handler
         * and clears the cached image generators.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a Discord_tic_tac_toe_command_impl module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
