//
// Created by ilesik on 7/6/24.
//

#pragma once

#include "../../module/module.hpp"
#include "../../module/modules_manager.hpp"
#include "./terminal_command.hpp"

#include <thread>
#include <atomic>
#include <iostream>
#include <shared_mutex>
#include <map>

namespace gb {

    /**
     * @brief The Admin_terminal class provides an administrative terminal interface for managing modules.
     * It inherits from the Module class and integrates with the Modules_manager for additional functionality.
     */
    class Admin_terminal : public Module {
    public:
        /**
         * @brief Constructor for Admin_terminal.
         * @param name Module name.
         * @param dependencies Module dependecies.
         * Initializes a new instance of Admin_terminal.
         */
        Admin_terminal(const std::string &name, const std::vector<std::string>& dependencies): Module(name,dependencies){};

        /**
         * @brief Virtual destructor for Admin_terminal.
         */
        virtual ~Admin_terminal() = default;

        /**
         * @brief Method to run the Admin_terminal instance.
         * @note This method defines the runtime behavior of the Admin_terminal instance.
         */
        virtual void run() = 0;

        /**
         * @brief Method to add a command to the terminal.
         * @param cmd The command to add.
         * @throws std::runtime_error if the command is already registered.
         */
        virtual void add_command(const Terminal_command_ptr &cmd) = 0;

        /**
         * @brief Constructor for Terminal_command.
         * @param name The name of the command.
         * @param description A brief description of the command.
         * @param help Help text explaining the command usage.
         * @param callback The callback function to execute when the command is called.
         */
        virtual void add_command(std::string name, std::string description, std::string help,
                                 Terminal_command_callback callback) = 0;

        /**
         * @brief Method to remove a command from the terminal.
         * @param name The name of the command to remove.
         */
        virtual void remove_command(const std::string &name) = 0;

        /**
         * @brief Method to initialize the Admin_terminal instance.
         * @param modules Map of all modules managed by the application, including the module manager.
         * @note This method sets the modules_manager by retrieving it from the provided modules.
         */
        virtual void innit(const Modules &modules) = 0;

        /**
         * @brief Method to stop the Admin_terminal instance.
         * @note This method performs any necessary cleanup or shutdown procedures for the Admin_terminal instance.
         */
        virtual void stop() = 0;
    };

    /**
     * @brief Alias for a shared pointer to an Admin_terminal.
     */
    typedef std::shared_ptr<Admin_terminal> Admin_terminal_ptr;

    /**
     * @brief External C function declaration to create an Admin_terminal instance.
     * @return Shared pointer to a new Module instance.
     * @note This function is intended for dynamic module loading.
     */
    extern "C" Module_ptr create();
} // namespace gb
