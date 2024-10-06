//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/premium_manager/premium_manager.hpp>
#include "./discord_hangman_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord Hangman Command.
     *
     * This class provides the full implementation for the Discord Hangman command.
     * It initializes the game, sets up the slash commands, and manages the game lifecycle.
     * It interacts with the premium manager and other dependencies to manage the game.
     */
    class Discord_hangman_command_impl : public Discord_hangman_command {
      Premium_manager_ptr _premium; /**< A pointer to the Premium Manager for handling premium features. */

    public:
        /**
         * @brief Constructor for Discord_hangman_command_impl.
         *
         * Initializes the base command and sets up any necessary game configurations.
         */
        Discord_hangman_command_impl();

        /**
         * @brief Define destructor for cleanup.
         */
        virtual ~Discord_hangman_command_impl() = default;

        /**
         * @brief Initializes the Hangman command module.
         *
         * Registers the Hangman slash command, sets up game data, and initializes
         * any necessary dependencies for the game.
         *
         * @param modules A reference to the module manager containing other dependent modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the Hangman module.
         *
         * Cleans up any game-related resources and removes the registered slash command.
         */
        void stop() override;

        /**
         * @brief Runs the Hangman game module.
         *
         * Makes the Hangman game available for players and starts the game.
         */
        void run() override;
    };

    /**
     * @brief Factory function for creating the Discord Hangman Command module.
     *
     * @return A shared pointer to the created Hangman command module.
     */
    extern "C" Module_ptr create();

} // namespace gb
