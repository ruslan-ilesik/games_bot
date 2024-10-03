//
// Created by ilesik on 9/29/24.
//

#pragma once
#include "./discord_2048_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord 2048 command.
     *
     * The `Discord_2048_command_impl` class provides the actual implementation of the 2048
     * game command in Discord. It includes methods to initialize, run, and stop the game.
     * This class manages the command-specific functionalities, including game logic,
     * user interaction, and command registration in Discord.
     */
    class Discord_2048_command_impl : public Discord_2048_command {
    public:
        /**
         * @brief Constructs a new `Discord_2048_command_impl` object.
         *
         * This constructor sets up the implementation for the 2048 command by initializing
         * the game settings, including title, description, and other relevant game configurations.
         */
        Discord_2048_command_impl();

        /**
        * @breif Define destructor.
        */
        virtual ~Discord_2048_command_impl() = default;

        /**
         * @brief Initializes the 2048 game command with the required modules.
         *
         * This method sets up the necessary environment for the 2048 command, including
         * registering the command in Discord, and preparing game-related components.
         *
         * @param modules A reference to the `Modules` object containing necessary dependencies
         *                such as the bot, command handler, and other relevant modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Runs the 2048 game command.
         *
         * This method starts the 2048 game, handling player interactions, moves, and
         * the display of the game grid within the Discord environment.
         */
        void run() override;

        /**
         * @brief Stops the 2048 game command.
         *
         * This method handles the cleanup of resources associated with the game,
         * unregisters the command from the bot, and performs any required teardown processes.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a new `Discord_2048_command_impl` object.
     *
     * This function creates and returns a new instance of the `Discord_2048_command_impl` object.
     * It is designed for dynamic loading as a module within the application.
     *
     * @return Module_ptr A pointer to the newly created `Discord_2048_command_impl` object.
     */
    extern "C" Module_ptr create();

} // namespace gb
