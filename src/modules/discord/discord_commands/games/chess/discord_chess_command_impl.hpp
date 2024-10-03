//
// Created by ilesik on 9/15/24.
//

#pragma once
#include "./discord_chess_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord chess command.
     *
     * The `Discord_chess_command_impl` class provides the actual implementation of the chess
     * game command in Discord. It includes methods to initialize, run, and stop the game.
     * This class integrates with the chess library and manages command-specific functionalities
     * such as registering the command, handling game logic, and user interaction in Discord.
     */
    class Discord_chess_command_impl : public Discord_chess_command {
    public:
        /**
         * @brief Constructs a new `Discord_chess_command_impl` object.
         *
         * This constructor sets up the implementation for the chess command, defining its title,
         * description, and image URL.
         */
        Discord_chess_command_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_chess_command_impl() = default;

        /**
         * @brief Runs the chess game command.
         *
         * This method is responsible for initiating the chess game, registering it in the bot,
         * and managing its state when the command is triggered.
         */
        void run() override;

        /**
         * @brief Stops the chess game command.
         *
         * This method cleans up the chess game resources, unregisters the command from the bot,
         * and removes cached assets related to the chess game.
         */
        void stop() override;

        /**
         * @brief Initializes the chess game command with the required modules.
         *
         * This method sets up the necessary environment for the chess command, including registering
         * the command in Discord, caching image generators, and setting up command options like
         * player selection and timeout parameters.
         *
         * @param modules A reference to the `Modules` object that contains necessary dependencies
         *                such as the bot, command handler, and image processor.
         */
        void init(const Modules &modules) override;
    };

    /**
     * @brief Factory function to create a new `Discord_chess_command_impl` object.
     *
     * This function is used to create and return a new instance of the `Discord_chess_command_impl` object.
     * It is designed for dynamic loading as a module in the application.
     *
     * @return Module_ptr A pointer to the newly created `Discord_chess_command_impl` object.
     */
    extern "C" Module_ptr create();

} // namespace gb
