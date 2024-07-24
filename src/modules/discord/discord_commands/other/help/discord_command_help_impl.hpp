//
// Created by ilesik on 7/20/24.
//

#pragma once
#include "./discord_command_help.hpp"
#include "../../../discord_command_handler/discord_command_handler.hpp"
#include "../../../discord_bot/discord_bot.hpp"
#include "src/modules/discord/discord/discord.hpp"
#include "src/modules/discord/discord_interactions_handler/discord_select_menu_handler/discord_select_menu_handler.hpp"

namespace gb {

    /**
     * @class Discord_command_help_impl
     * @brief Implementation of the Discord_command_help module for handling the help command in a Discord bot.
     */
    class Discord_command_help_impl: public Discord_command_help {
    private:
        /**
         * @brief Shared pointer to the Discord command handler.
         */
        Discord_command_handler_ptr _command_handler;

        /**
         * @brief Shared pointer to the Discord bot.
         */
        Discord_bot_ptr _discord_bot;

        /**
         * @brief Shared pointer to the Discord select menu handler.
         */
        Discord_select_menu_handler_ptr _select_menu_handler;

        /**
         * @brief Counter for amount of currently running commands.
         */
        std::atomic_uint64_t _running_cnt;

        /**
         * @brief Condition variable for proper work of stop.
         */
        std::condition_variable _cv;

        /**
         * @brief Handles the help command.
         *
         * This function sends an embedded message with available command categories and waits for the user to select one.
         * Then, it displays commands in the selected category, waits for the user to select a command, and displays detailed information about the selected command.
         *
         * @param event The slash command event.
         * @return dpp::task<void> A coroutine task.
         */
        dpp::task<void> help_command(const dpp::slashcommand_t &event);

    public:
        /**
         * @brief Constructor for Discord_command_help_impl.
         */
        Discord_command_help_impl();

        /**
         * @brief Stops the help command module.
         *
         * This function removes the "help" command and waits until all running executions of the command are stopped.
         */
        void stop() override;

        /**
         * @brief Initializes the module with the provided modules.
         *
         * @param modules The modules to initialize with.
         */
        void innit(const Modules &modules) override;

        /**
         * @brief Runs the module.
         */
        void run() override;
    };

    /**
     * @brief Creates an instance of Discord_command_help_impl.
     *
     * @return Module_ptr A pointer to the created module.
     */
    extern "C" Module_ptr create();

} // gb
