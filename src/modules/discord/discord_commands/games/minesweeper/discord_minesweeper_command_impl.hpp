//
// Created by ilesik on 9/19/24.
//

#pragma once

#include "./discord_minesweeper_command.hpp"

namespace gb {

    /**
     * @class Discord_minesweeper_command_impl
     * @brief Implements the Minesweeper game command for the Discord bot.
     *
     * This class handles the setup, execution, and teardown of the Minesweeper game command
     * in a Discord environment. It inherits from the Discord_minesweeper_command base class
     * and overrides the necessary methods such as `run`, `init`, and `stop`.
     */
    class Discord_minesweeper_command_impl: public Discord_minesweeper_command {

    public:
        /**
         * @brief Constructs a Discord_minesweeper_command_impl object.
         *
         * Initializes the Minesweeper command with the required dependencies.
         */
        Discord_minesweeper_command_impl();

        /**
         * @brief Executes the Minesweeper command.
         *
         * This method contains the logic for running the Minesweeper game within the Discord environment.
         */
        void run() override;

        /**
         * @brief Initializes the command with the necessary modules.
         *
         * This method sets up the command handler, required modules, and other resources
         * necessary for the Minesweeper game to run. It also registers the game command
         * with the Discord bot.
         *
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the command and performs cleanup.
         *
         * This method removes the Minesweeper game command from the command handler
         * and performs necessary cleanup tasks such as deallocating resources.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a Discord_minesweeper_command_impl module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
