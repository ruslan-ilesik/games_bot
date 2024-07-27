//
// Created by ilesik on 7/10/24.
//

#pragma once

#include "discord_command_handler.hpp"
#include "../discord_bot/discord_bot.hpp"
#include "../../admin_terminal/admin_terminal.hpp"
#include "src/modules/database/database.hpp"

#include <shared_mutex>
#include <map>

namespace gb {

    /**
     * @brief Implementation of the Discord_command_handler module.
     * This class manages the registration, removal, and bulk operations of Discord commands.
     */
    class Discord_command_handler_impl : public Discord_command_handler {
    private:
        bool _bulk = true; ///< Indicates if bulk operations are enabled.
        std::shared_mutex _mutex; ///< Mutex for thread-safe operations.
        Discord_bot_ptr _discord_bot; ///< Pointer to the Discord bot.
        Database_ptr _db; ///< Pointer to the Databsase.
        Admin_terminal_ptr _admin_terminal; ///< Pointer to the admin terminal.
        std::vector<std::string> _command_register_queue; ///< Queue of commands to register.
        std::map<std::string, Discord_command_ptr> _commands; ///< Map of registered commands.
        dpp::event_handle  _on_ready_handler = 0; ///< Handler of on ready event to remove in stop
        dpp::event_handle  _on_slashcommand_handler = 0; ///< Handler of on slashcommand event to remove in stop
        Prepared_statement _insert_command_use_stmt; ///< ID of insert command use database query.

        /**
         * @brief Perform bulk actions.
         * Registers all commands in the queue to Discord.
         * Throws an exception if bulk operations are enabled.
         */
        void bulk_actions();

        /**
         * @brief Remove a command by name.
         * @param name The name of the command to remove.
         * Removes the command from Discord and the internal command map.
         * Throws an exception if the command does not exist.
         */
        void do_command_remove(const std::string& name);

    public:
        /**
         * @brief Constructor for Discord_command_handler_impl.
         * Initializes the module with the required dependencies.
         */
        Discord_command_handler_impl();

        /**
         * @brief Destructor for Discord_command_handler_impl.
         * Default destructor.
         */
        virtual ~Discord_command_handler_impl() = default;

        /**
         * @brief Starts the Discord command handler.
         * Sets bulk operations to false.
         */
        void run() override;

        /**
         * @brief Stops the Discord command handler.
         * Removes all related commands from the admin terminal.
         */
        void stop() override;

        /**
         * @brief Initializes the module with dependencies.
         * @param modules A map of module names to module shared pointers.
         * Initializes the Discord bot and admin terminal, and adds related commands to the admin terminal.
         */
        void innit(const Modules &modules) override;

        /**
         * @brief Removes a command by name.
         * @param name The name of the command to remove.
         * Removes the command from Discord and the internal command map.
         */
        void remove_command(const std::string& name) override;

        /**
         * @brief Sets the bulk operation status.
         * @param value Boolean value to set bulk status.
         * If set to true, performs bulk actions.
         */
        void set_bulk(bool value) override;

        /**
         * @brief Gets the bulk operation status.
         * @return True if bulk operations are enabled, otherwise false.
         */
        bool is_bulk() override;

        /**
         * @brief Registers a Discord command.
         * @param command A shared pointer to the Discord command to register.
         * Adds the command to the internal map and queue if bulk operations are enabled, otherwise registers immediately.
         */
        void register_command(const Discord_command_ptr& command) override;

        /**
         * @brief Removes all commands.
         * Removes all commands from Discord in bulk.
         */
        void remove_commands() override;

        /**
         * @brief Registers all commands in bulk.
         * Registers all commands in the internal map to Discord in bulk.
         */
        void register_commands() override;

        /**
        * @brief Getter for commands.
        * @returns std::map<std::string, Discord_command_ptr> of commands.
        */
        std::map<std::string, Discord_command_ptr> get_commands() override;
    };

    /**
    * @brief External C function declaration to create a Discord_command_handler_impl instance.
    * @return Shared pointer to a new Module instance.
    */
    extern "C" Module_ptr create();
} // gb
