//
// Created by ilesik on 7/21/24.
//

#pragma once
#include "./discord_select_menu_handler.hpp"
#include "../id_cache.hpp"
#include "../../discord_bot/discord_bot.hpp"

namespace gb {

    /**
     * @brief Implementation class for handling Discord select menu interactions.
     *
     * The Discord_select_menu_handler_impl class handles interactions with
     * Discord select menus by managing unique component IDs and waiting for
     * user interactions within a specified timeout.
     */
    class Discord_select_menu_handler_impl : public Discord_select_menu_handler {
        /// Pointer to the Discord bot module.
        Discord_bot_ptr _bot;

        /// Cache for managing unique component IDs.
        Id_cache _cache{};

    public:
        /**
         * @brief Constructor initializes the handler with the necessary dependencies.
         */
        Discord_select_menu_handler_impl();

        /**
         * @brief Destructor for Discord_select_menu_handler_impl.
         */
        virtual ~Discord_select_menu_handler_impl() = default;

        /**
         * @brief Waits for a user interaction with the select menu within a specified timeout.
         *
         * This function waits for a user to interact with the select menu component in a given
         * message and checks if the interaction is from the allowed users and within the timeout.
         *
         * @param m The DPP message containing the select menu component.
         * @param users A list of user IDs that are allowed to interact with the select menu.
         * @param timeout The maximum time to wait for an interaction.
         * @return A dpp::task<Select_menu_return> containing the result of the interaction.
         */
        dpp::task<Select_menu_return> wait_for(dpp::message& m, const std::vector<dpp::snowflake>& users, time_t timeout) override;

        /**
         * @brief Runs the select menu handler.
         */
        void run() override;

        /**
         * @brief Initializes the handler with the required modules.
         *
         * This function sets up the handler with the necessary dependencies from the modules.
         *
         * @param modules The modules required by the handler.
         */
        void innit(const Modules& modules) override;

        /**
         * @brief Stops the select menu handler.
         *
         * This function stops the handler and ensures that all cached IDs are cleared.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a new instance of Discord_select_menu_handler_impl.
     *
     * @return A shared pointer to the newly created Discord_select_menu_handler_impl instance.
     */
    extern "C" Module_ptr create();

} // gb
