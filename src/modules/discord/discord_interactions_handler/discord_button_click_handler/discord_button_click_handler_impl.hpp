//
// Created by ilesik on 7/21/24.
//

#pragma once
#include "./discord_button_click_handler.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"
#include "src/modules/discord/discord_interactions_handler/id_cache.hpp"

namespace gb {

    /**
     * @class Discord_button_click_handler_impl
     * @brief Implementation of the Discord button click handler.
     *
     * This class handles the interaction with buttons in Discord messages, waiting for
     * user interactions and managing component IDs.
     */
    class Discord_button_click_handler_impl : public Discord_button_click_handler {

        /// Pointer to the Discord bot module.
        Discord_bot_ptr _bot;

        /// Cache for managing unique component IDs.
        Id_cache _cache{};

    public:
        /**
         * @brief Constructor for Discord_button_click_handler_impl.
         *
         * Initializes the handler with the necessary dependencies.
         */
        Discord_button_click_handler_impl();;

        /**
         * @brief Destructor for Discord_button_click_handler_impl.
         */
        virtual ~Discord_button_click_handler_impl() = default;

        /**
        * @brief Waits for a user interaction with a button within a specified timeout.
        *
        * This function waits for an interaction with a button in a Discord message. If an interaction
        * occurs within the timeout period, it returns the interaction event; otherwise, it times out.
        *
        * @param m The DPP message containing the button component.
        * @param users A list of user IDs that are allowed to interact with the button.
        * @param timeout The maximum time to wait for an interaction.
        * @param generate_ids Flag to make hides ids of message or not (for example if we need await same message second time we will not hide id).
        * @param clear_ids Flag to clear ids after event awaiting finished (should note that in case of timeouts ids cleared anyway).
        * @return A dpp::task<Button_click_return> containing the result of the interaction.
        */
        dpp::task<Button_click_return> wait_for(dpp::message& m, const std::vector<dpp::snowflake>& users, time_t timeout, bool generate_ids = true, bool clear_ids = true) override;

        /**
        * @brief Clears ids from hash in specific message.
        *
        * @param m message which ids should be removed from cache.
        */
        void clear_ids(const dpp::message& m) override;

        /**
         * @brief Starts the handler's operation.
         */
        void run() override;

        /**
         * @brief Initializes the handler with the necessary modules.
         *
         * @param modules A map of module names to their shared pointers.
         */
        void init(const Modules& modules) override;

        /**
         * @brief Stops the handler's operation.
         *
         * This function stops the handler, ensuring that all running executions are finished.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a new instance of Discord_button_click_handler_impl.
     *
     * @return A shared pointer to the newly created Discord_button_click_handler_impl instance.
     */
    extern "C" Module_ptr create();

} // gb
