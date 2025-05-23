//
// Created by ilesik on 7/21/24.
//

#pragma once
#include <dpp/dpp.h>
#include "src/module/module.hpp"

namespace gb {

    /// Type alias for the return type of the button click handler.
    typedef std::pair<dpp::button_click_t, bool> Button_click_return;

    class Discord_button_click_handler : public Module {
    public:
        /**
         * @brief Constructor initializes the handler with the necessary dependencies.
         *
         * @param name The name of the handler module.
         * @param dependencies A list of module dependencies required by this handler.
         */
        Discord_button_click_handler(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies) {}

        /**
         * @brief Destructor for Discord_select_menu_handler.
         */
        virtual ~Discord_button_click_handler() = default;

        /**
         * @brief Waits for a user interaction with the select menu within a specified timeout.
         *
         * This pure virtual function must be implemented by derived classes to handle
         * interactions with select menus in Discord messages.
         *
         * @param m The DPP message containing the select menu component.
         * @param users A list of user IDs that are allowed to interact with the select menu.
         * @param timeout The maximum time to wait for an interaction.
         * @param generate_ids Flag to make hides ids of message or not (for example if we need await same message
         * second time we will not hide id).
         * @param clear_ids Flag to clear ids after event awaiting finished (should note that in case of timeouts ids
         * cleared anyway).
         * @return A dpp::task<Button_click_return> containing the result of the interaction.
         */
        virtual dpp::task<Button_click_return> wait_for(dpp::message &m, const std::vector<dpp::snowflake> &users,
                                                        time_t timeout, bool generate_ids = true,
                                                        bool clear_ids = true) = 0;

        /**
         * @brief Waits for a user interaction with the select menu within a specified timeout and automatically replies
         * to message if not timeout ensuring event will be valid. In case reply failed, it retries awaiting.
         *
         * This pure virtual function must be implemented by derived classes to handle
         * interactions with select menus in Discord messages.
         *
         * @param m The DPP message containing the select menu component.
         * @param users A list of user IDs that are allowed to interact with the select menu.
         * @param timeout The maximum time to wait for an interaction.
         * @param generate_ids Flag to make hides ids of message or not (for example if we need await same message
         * second time we will not hide id).
         * @param clear_ids Flag to clear ids after event awaiting finished (should note that in case of timeouts ids
         * cleared anyway).
         * @return A dpp::task<Button_click_return> containing the result of the interaction.
         */
        virtual dpp::task<Button_click_return> wait_for_with_reply(dpp::message &m,
                                                                   const std::vector<dpp::snowflake> &users,
                                                                   time_t timeout, bool generate_ids = true,
                                                                   bool clear_ids = true) = 0;

        /**
         * @brief Clears ids from hash in specific message.
         *
         * @param m message which ids should be removed from cache.
         */
        virtual void clear_ids(const dpp::message &m) = 0;
    };
    typedef std::shared_ptr<Discord_button_click_handler> Discord_button_click_handler_ptr;

} // namespace gb
