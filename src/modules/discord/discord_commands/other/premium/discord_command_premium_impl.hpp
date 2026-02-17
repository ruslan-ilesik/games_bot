//
// Created by ilesik on 10/3/24.
//

#pragma once

#include <src/modules/discord/premium_manager/premium_manager.hpp>
#include "./discord_command_premium.hpp"

namespace gb {

    /**
     * @class Discord_command_premium_impl
     * @brief Implementation of the premium command for Discord.
     *
     * This class implements the premium command for Discord, which interacts with the
     * Premium Manager to check, modify, and manage premium subscription statuses of users.
     */
    class Discord_command_premium_impl : public Discord_command_premium {
        Premium_manager_ptr _premium; ///< Pointer to the Premium Manager module for managing premium status.

    protected:
        /**
         * Overrides command callback with proper handler.
         * @param event slashcommand event forwarded to handler.
         * @return nothing
         */
        dpp::task<void> _command_callback(const dpp::slashcommand_t &event) override;

    public:
        /**
         * @brief Constructor for Discord_command_premium_impl.
         *
         * Initializes the Discord premium command implementation, allowing for interaction with
         * the premium system in the bot.
         */
        Discord_command_premium_impl();

        /**
         * @brief Virtual destructor for Discord_command_premium_impl.
         */
        virtual ~Discord_command_premium_impl() = default;

        /**
         * @brief Stops the premium command module.
         *
         * This method stops any running processes related to the premium command, ensuring
         * proper shutdown of operations.
         */
        void stop() override;

        /**
         * @brief Initializes the premium command with the required modules.
         *
         * This method sets up the necessary dependencies for the premium command by
         * initializing the Premium Manager module and other required components.
         *
         * @param modules A reference to the collection of modules required by this command.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Placeholder, do nothing.
         *
         */
        void run() override;
    };

    /**
     * @brief Function to create a new instance of the Discord_command_premium_impl module.
     * @return A shared pointer to the newly created Discord_command_premium_impl module.
     */
    extern "C" Module_ptr create();

} // namespace gb
