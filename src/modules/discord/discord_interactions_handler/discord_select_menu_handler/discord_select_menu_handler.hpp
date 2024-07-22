//
// Created by ilesik on 7/21/24.
//

#pragma once
#include <dpp/dpp.h>
#include "src/module/module.hpp"

namespace gb {

    /// Type alias for the return type of the select menu handler.
    typedef std::pair<dpp::select_click_t, bool> Select_menu_return;

    /**
     * @brief Abstract base class for handling Discord select menu interactions.
     *
     * The Discord_select_menu_handler class defines an interface for handling
     * interactions with Discord select menus, allowing for customization and
     * extension of functionality.
     */
    class Discord_select_menu_handler : public Module {
    public:
        /**
         * @brief Constructor initializes the handler with the necessary dependencies.
         *
         * @param name The name of the handler module.
         * @param dependencies A list of module dependencies required by this handler.
         */
        Discord_select_menu_handler(const std::string& name, const std::vector<std::string>& dependencies)
            : Module(name, dependencies) {}

        /**
         * @brief Destructor for Discord_select_menu_handler.
         */
        virtual ~Discord_select_menu_handler() = default;

        /**
         * @brief Waits for a user interaction with the select menu within a specified timeout.
         *
         * This pure virtual function must be implemented by derived classes to handle
         * interactions with select menus in Discord messages.
         *
         * @param m The DPP message containing the select menu component.
         * @param users A list of user IDs that are allowed to interact with the select menu.
         * @param timeout The maximum time to wait for an interaction.
         * @return A dpp::task<Select_menu_return> containing the result of the interaction.
         */
        virtual dpp::task<Select_menu_return> wait_for(dpp::message& m, const std::vector<dpp::snowflake>& users, time_t timeout) = 0;
    };

    /// Type alias for a shared pointer to a Discord_select_menu_handler.
    typedef std::shared_ptr<Discord_select_menu_handler> Discord_select_menu_handler_ptr;

} // gb
