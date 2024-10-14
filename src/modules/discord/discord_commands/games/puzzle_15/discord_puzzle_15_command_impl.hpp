//
// Created by ilesik on 10/14/24.
//

#pragma once

#include <src/modules/discord/premium_manager/premium_manager.hpp>
#include "./discord_puzzle_15_command.hpp"

namespace gb {

    /**
     * @class Discord_puzzle_15_command_impl
     * @brief Implements the Puzzle 15 game command for the Discord bot.
     *
     * This class handles the initialization, execution, and stopping of the Puzzle 15 game command
     * in the Discord environment. It extends the `Discord_puzzle_15_command` base class and overrides
     * key methods like `init`, `run`, and `stop` to manage the game within the bot's framework.
     */
    class Discord_puzzle_15_command_impl : public Discord_puzzle_15_command {

        Premium_manager_ptr _premium; /**< Pointer to the premium manager, used for premium features. */

    public:
        /**
         * @brief Constructs a Discord_puzzle_15_command_impl object.
         *
         * Initializes the Puzzle 15 command implementation.
         */
        Discord_puzzle_15_command_impl();

        /**
         * @brief Defines the destructor.
         */
        virtual ~Discord_puzzle_15_command_impl() = default;

        /**
         * @brief Initializes the Puzzle 15 command with necessary modules.
         *
         * This method sets up the command with required modules, registers the Puzzle 15 game
         * with the bot, and prepares the premium features if necessary.
         *
         * @param modules A map of module names to module pointers, providing dependencies needed for initialization.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the Puzzle 15 game command and performs cleanup.
         *
         * This method unregisters the Puzzle 15 command from the Discord bot and frees any resources used.
         */
        void stop() override;

        /**
         * @brief Executes the Puzzle 15 game.
         *
         * This method triggers the Puzzle 15 game, managing the game state and interaction within Discord.
         */
        void run() override;
    };

    /**
     * @brief Factory function to create a `Discord_puzzle_15_command_impl` module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
