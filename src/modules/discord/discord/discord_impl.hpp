//
// Created by ilesik on 7/10/24.
//

#pragma once

#include "discord.hpp"
#include "discord_command.hpp"

namespace gb {

    /**
     * @class Discord
     * @brief Implementation of the Discord module.
     * This class provides methods to create Discord wrappers we have.
     */
    class Discord_impl : public Discord {
    public:
        /**
         * @brief Constructs a Discord object.
         */
        Discord_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_impl() = default;

        /**
         * @brief Starts the Discord module.
         */
        void run() override;

        /**
         * @brief Stops the Discord module.
         */
        void stop() override;

        /**
         * @brief Initializes the Discord module with the provided modules.
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Creates a Discord command.
         * @param command The slash command to create.
         * @param handler The handler for the command.
         * @param command_data Additional data for the command.
         * @return A shared pointer to the created Discord command.
         */
        Discord_command_ptr create_discord_command(const dpp::slashcommand &command,
                                                   const slash_command_handler_t &handler,
                                                   const Command_data &command_data) override;
    };

    /**
     * @brief Function to create a new instance of the Discord module.
     * @return A shared pointer to the newly created Discord module.
     */
    Module_ptr create();

} // namespace gb
