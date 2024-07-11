//
// Created by ilesik on 7/10/24.
//

#pragma once

#include "discord_command.hpp"

namespace gb {

    /**
     * @class Discord_command_impl
     * @brief Implementation of the Discord_command class.
     * This class represents a specific implementation of a Discord command.
     */
    class Discord_command_impl: public Discord_command {
    private:
        dpp::slashcommand _command; ///< The slash command.
        slash_command_handler_t _handler; ///< The handler for the command.
        Command_data _command_data; ///< Additional data for the command.

    public:
        /**
         * @brief Constructs a Discord_command_impl object.
         * @param command The slash command.
         * @param handler The handler for the command.
         * @param command_data Additional data for the command.
         */
        Discord_command_impl(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                             const Command_data &command_data);

        /**
         * @brief Gets the slash command.
         * @return The slash command.
         */
        dpp::slashcommand get_command() const override;

        /**
         * @brief Gets the handler for the command.
         * @return The handler for the command.
         */
        slash_command_handler_t get_handler() const override;

        /**
         * @brief Gets the additional data for the command.
         * @return The command data.
         */
        Command_data get_command_data() const override;

        /**
         * @brief Executes the command.
         * @param event The slash command event.
         */
        void operator()(const dpp::slashcommand_t &event) override;

        /**
         * @brief Gets the name of the command.
         * @return The name of the command.
         */
        std::string get_name() const override;
    };

} // gb
