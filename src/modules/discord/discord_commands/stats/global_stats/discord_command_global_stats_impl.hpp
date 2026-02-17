//
// Created by ilesik on 9/1/24.
//

#pragma once

#include <src/modules/database/database.hpp>
#include "./discord_command_global_stats.hpp"

namespace gb {

    /**
     * @class Discord_command_global_stats_impl
     * @brief Implementation of the Discord command for retrieving global statistics.
     *
     * This class provides the implementation for the Discord command that allows users
     * to retrieve global statistics such as bot command usage and game statistics across all servers.
     */
    class Discord_command_global_stats_impl : public Discord_command_global_stats {
    private:
        /// Pointer to the database instance.
        Database_ptr _db;

        /// Prepared statement for querying bot commands statistics globally.
        Prepared_statement _bot_commands_stmt;

        /// Prepared statement for querying bot games statistics globally.
        Prepared_statement _bot_games_stmt;

        /// Prepared statement for querying the user's commands statistics globally.
        Prepared_statement _me_commands_stmt;

        /// Prepared statement for querying the user's games statistics globally.
        Prepared_statement _me_games_stmt;

        /**
         * @brief Retrieves and sends the global bot statistics to the user.
         *
         * This function handles the 'bot' subcommand, executing the appropriate queries
         * to retrieve global bot command and game statistics, and sending the results to the user via Discord.
         *
         * @param event The event triggered by the slash command.
         */
        dpp::task<void> select_bot(const dpp::slashcommand_t &event);

        /**
         * @brief Retrieves and sends the global user's statistics to the user.
         *
         * This function handles the 'me' subcommand, executing the appropriate queries
         * to retrieve the user's global command and game statistics, and sending the results to the user via Discord.
         *
         * @param event The event triggered by the slash command.
         */
        dpp::task<void> select_me(const dpp::slashcommand_t &event);

    protected:
        /**
         * Overrides command callback with proper handler.
         * @param event slashcommand event forwarded to handler.
         * @return nothing
         */
        dpp::task<void> _command_callback(const dpp::slashcommand_t &event) override;

    public:
        /**
         * @brief Constructor for Discord_command_global_stats_impl.
         *
         * Initializes the class and prepares the necessary database statements.
         */
        Discord_command_global_stats_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_command_global_stats_impl() = default;

        /**
         * @brief Initializes the command with necessary modules.
         *
         * This function is used to initialize the command with dependencies, such as the database module.
         *
         * @param modules The set of modules required by the command.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Runs the command.
         *
         * This function starts the execution of the command, making it ready to handle Discord events.
         */
        void run() override;

        /**
         * @brief Stops the command.
         *
         * This function stops the execution of the command, cleaning up any resources or ongoing operations.
         */
        void stop() override;
    };

    /**
     * @brief Factory function for creating an instance of the module.
     *
     * This function is used to create a new instance of the Discord_command_global_stats_impl class.
     *
     * @return Module_ptr A pointer to the newly created module instance.
     */
    extern "C" Module_ptr create();

} // namespace gb
