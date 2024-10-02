//
// Created by ilesik on 9/26/24.
//

#pragma once

#include "./discord_sudoku_command.hpp"

namespace gb {

    /**
     * @class Discord_sudoku_command_impl
     * @brief Implements the Sudoku game command for the Discord bot.
     *
     * This class manages the setup, execution, and teardown of the Sudoku game command
     * in a Discord environment. It inherits from the Discord_sudoku_command base class
     * and overrides essential methods such as `run`, `init`, and `stop`.
     */
    class Discord_sudoku_command_impl : public Discord_sudoku_command {

    public:
        /**
         * @brief Constructs a Discord_sudoku_command_impl object.
         *
         * Initializes the Sudoku command with the necessary dependencies.
         */
        Discord_sudoku_command_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_sudoku_command_impl() = default;

        /**
         * @brief Executes the Sudoku command.
         *
         * Placeholder to implement abstract class.
         */
        void run() override;

        /**
         * @brief Initializes the command with the necessary modules.
         *
         * This method sets up the command handler, required modules, and other resources
         * necessary for the Sudoku game to function. It also registers the game command
         * with the Discord bot.
         *
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the command and performs cleanup.
         *
         * This method removes the Sudoku game command from the command handler
         * and performs necessary cleanup tasks, including deallocating resources.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a Discord_sudoku_command_impl module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
