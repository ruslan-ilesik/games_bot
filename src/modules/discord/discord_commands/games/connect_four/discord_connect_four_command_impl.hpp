//
// Created by ilesik on 9/16/24.
//

#pragma once
#include "./discord_connect_four_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord Connect Four command.
     *
     * The `Discord_connect_four_command_impl` class provides the actual implementation of the Connect Four
     * game command in Discord. It includes methods to initialize and stop the game. This class manages the
     * command-specific functionalities, including registration, handling game logic, and user interaction
     * within Discord.
     */
    class Discord_connect_four_command_impl : public Discord_connect_four_command {
    public:
        /**
         * @brief Constructs a new `Discord_connect_four_command_impl` object.
         *
         * This constructor sets up the implementation for the Connect Four command by initializing
         * the title, description, and any other relevant settings for the game.
         */
        Discord_connect_four_command_impl();

        /**
         * @brief Initializes the Connect Four game command with the required modules.
         *
         * This method sets up the necessary environment for the Connect Four command, including registering
         * the command in Discord, and preparing game-related components.
         *
         * @param modules A reference to the `Modules` object containing necessary dependencies
         *                such as the bot, command handler, and other relevant modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the Connect Four game command.
         *
         * This method handles the cleanup of resources associated with the game, unregisters the command
         * from the bot, and performs any required teardown processes.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a new `Discord_connect_four_command_impl` object.
     *
     * This function creates and returns a new instance of the `Discord_connect_four_command_impl` object.
     * It is designed for dynamic loading as a module within the application.
     *
     * @return Module_ptr A pointer to the newly created `Discord_connect_four_command_impl` object.
     */
    extern "C" Module_ptr create();

} // namespace gb
