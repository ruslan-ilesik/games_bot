//
// Created by ilesik on 7/10/24.
//

#pragma once

#include "../../../module/module.hpp"
#include "../discord/discord_command.hpp"

namespace gb {
    /**
     * @class Discord_command_handler
     * @brief A class representing a handler for Discord commands.
     *
     * This class inherits from the Module class and provides methods for managing
     * Discord commands, including registering, removing, and bulk operations.
     */
    class Discord_command_handler : public Module {
    public:

        /**
         * @brief Constructs a Discord_command_handler object.
         *
         * @param name The name of the module.
         * @param dependencies A vector of strings representing the dependencies of the module.
         */
        Discord_command_handler(const std::string &name, const std::vector<std::string> &dependencies)
            : Module(name, dependencies) {};

        /**
         * @brief Sets the bulk operation status.
         *
         * @param value A boolean indicating whether bulk operations are enabled.
         */
        virtual void set_bulk(bool value) = 0;

        /**
         * @brief Checks if bulk operations are enabled.
         *
         * @return A boolean indicating the bulk operation status.
         */
        virtual bool is_bulk() = 0;

        /**
         * @brief Initializes the Discord_command_handler with the provided modules.
         *
         * @param modules A map of module names to module shared pointers.
         */
        virtual void innit(const Modules &modules) = 0;

        /**
         * @brief Stops the Discord_command_handler module.
         */
        virtual void stop() = 0;

        /**
         * @brief Runs the Discord_command_handler module.
         */
        virtual void run() = 0;

        /**
         * @brief Registers a Discord command.
         *
         * @param command A shared pointer to the Discord_command to be registered.
         */
        virtual void register_command(const Discord_command_ptr &command) = 0;

        /**
         * @brief Removes a Discord command by name.
         *
         * @param name The name of the command to be removed.
         */
        virtual void remove_command(const std::string &name) = 0;

        /**
         * @brief Delete all commands, but does not remove them from handler.
         */
        virtual void remove_commands() = 0;

        /**
         * @brief Registers all loaded commands to Discord in bulk, ignoring individual operations.
         */
        virtual void register_commands() = 0;
    };

    /**
     * @typedef Discord_command_handler_ptr
     * @brief A shared pointer type for Discord_command_handler.
     */
    typedef std::shared_ptr<Discord_command_handler> Discord_command_handler_ptr;

    /**
     * @brief Function to create a new instance of the Discord_command_handler module.
     *
     * @return A shared pointer to the newly created Discord_command_handler module.
     */
    extern "C" Module_ptr create();

} // gb
