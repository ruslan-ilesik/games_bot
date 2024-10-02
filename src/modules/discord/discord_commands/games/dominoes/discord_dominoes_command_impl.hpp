//
// Created by ilesik on 9/4/24.
//

#pragma once

#include "./discord_dominoes_command.hpp"

namespace gb {

    /**
     * @brief Implementation of the Discord Dominoes Command.
     *
     * This class provides the full implementation for the Discord Dominoes command.
     * It initializes the game, sets up the slash commands, and manages the game lifecycle.
     * It uses the image processing module for caching game assets and interacts with
     * the bot to manage lobby creation and gameplay.
     */
    class Discord_dominoes_command_impl : public Discord_dominoes_command {
    public:
        /**
         * @brief Constructor for Discord_dominoes_command_impl.
         *
         * Initializes the base command and sets the lobby title, description, and image URL
         * for the Dominoes game. This constructor also prepares any necessary game assets.
         */
        Discord_dominoes_command_impl();


        /**
         * @breif Define destructor.
         */
        virtual ~Discord_dominoes_command_impl() = default;

        /**
         * @brief Initializes the module and sets up the Dominoes command.
         *
         * Registers the Dominoes slash command, pre-requirements, and initializes the image
         * cache for the game. This function also sets up game-related data and command options
         * like player count and game start parameters.
         *
         * @param modules Reference to the module manager containing other dependent modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the module and removes the Dominoes command.
         *
         * This function removes the registered Dominoes slash command, cleans up resources
         * related to the game, and removes cached game images.
         */
        void stop() override;

        /**
         * @brief Runs the Dominoes game module.
         *
         * This function starts the module, making the game available and ready for execution.
         */
        void run() override;
    };

    /**
     * @brief Factory function for creating the Discord Dominoes Command module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
