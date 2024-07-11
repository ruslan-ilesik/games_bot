//
// Created by ilesik on 7/10/24.
//

#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <vector>

namespace gb {

    /**
     * @typedef slash_command_handler_t
     * @brief Type definition for a function that handles slash commands.
     */
    typedef std::function<void(const dpp::slashcommand_t &)> slash_command_handler_t;

    /**
     * @struct Command_data
     * @brief Structure to hold additional data for a command.
     */
    struct Command_data {
        std::string help_text; ///< Help text for the command.
        std::vector<std::string> category; ///< Categories for the command, nested in each other.
    };

    /**
     * @class Discord_command
     * @brief Abstract base class representing a Discord command.
     */
    class Discord_command {
    public:
        /**
         * @brief Virtual destructor for Discord_command.
         */
        virtual ~Discord_command() = default;

        /**
         * @brief Get the slash command.
         * @return The slash command.
         */
        virtual dpp::slashcommand get_command() const = 0;

        /**
         * @brief Get the handler for the command.
         * @return The handler for the command.
         */
        virtual slash_command_handler_t get_handler() const = 0;

        /**
         * @brief Get the command data.
         * @return The command data.
         */
        virtual Command_data get_command_data() const = 0;

        /**
         * @brief Execute the command.
         * @param event The slash command event.
         */
        virtual void operator()(const dpp::slashcommand_t & event) = 0;

        /**
         * @brief Get the name of the command.
         * @return The name of the command.
         */
        virtual std::string get_name() const = 0;
    };

    /**
     * @typedef Discord_command_ptr
     * @brief Type definition for a shared pointer to a Discord_command.
     */
    typedef std::shared_ptr<Discord_command> Discord_command_ptr;

} // gb
