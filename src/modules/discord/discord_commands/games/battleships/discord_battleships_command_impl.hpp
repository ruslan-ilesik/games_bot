//
// Created by ilesik on 9/9/24.
//

#pragma once

#include "./discord_battleships_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord Battleships command.
     *
     * The `Discord_battleships_command_impl` class provides the actual implementation of the Battleships
     * game command in Discord. It includes methods to run, initialize, and stop the game. This class manages the
     * command-specific functionalities, including registration, handling game logic, and user interaction
     * within Discord.
     */
    class Discord_battleships_command_impl : public Discord_battleships_command {
    public:
        /**
         * @brief Constructs a new `Discord_battleships_command_impl` object.
         *
         * This constructor sets up the implementation for the Battleships command by initializing
         * the game title, description, and any other relevant settings for the game.
         */
        Discord_battleships_command_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_battleships_command_impl() = default;

        /**
         * @brief Runs the Battleships game command.
         *
         * This method starts the Battleships game logic, handling player interactions and moves
         * within the Discord environment.
         */
        void run() override;

        /**
         * @brief Initializes the Battleships game command with the required modules.
         *
         * This method sets up the necessary environment for the Battleships command, including registering
         * the command in Discord, and preparing game-related components.
         *
         * @param modules A reference to the `Modules` object containing necessary dependencies
         *                such as the bot, command handler, and other relevant modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the Battleships game command.
         *
         * This method handles the cleanup of resources associated with the game, unregisters the command
         * from the bot, and performs any required teardown processes.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a new `Discord_battleships_command_impl` object.
     *
     * This function creates and returns a new instance of the `Discord_battleships_command_impl` object.
     * It is designed for dynamic loading as a module within the application.
     *
     * @return Module_ptr A pointer to the newly created `Discord_battleships_command_impl` object.
     */
    extern "C" Module_ptr create();

} // namespace gb
