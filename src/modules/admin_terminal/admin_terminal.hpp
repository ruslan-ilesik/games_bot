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
    private:
        /**
         * @brief Shared pointer to the Modules_manager instance.
         */
        Modules_manager_ptr _modules_manager;

        /**
         * @brief Thread for running the terminal interface.
         */
        std::thread _terminal_thread;

        /**
         * @brief Flag to indicate when to stop the terminal thread.
         */
        std::atomic_bool _stop_flag = false;

        /**
         * @brief Mutex for synchronizing access to shared resources.
         */
        std::mutex _mutex;

        /**
         * @brief Map of terminal commands, indexed by command name.
         */
        std::map<std::string, Terminal_command> _commands;

        /**
         * @brief Pipe file descriptors for communication between threads. Used to stop user input waiting thread.
         */
        int _pipe_fd[2];

    public:
        /**
         * @brief Constructor for Admin_terminal.
         * Initializes a new instance of Admin_terminal.
         */
        Admin_terminal();

        /**
         * @brief Virtual destructor for Admin_terminal.
         */
        virtual ~Admin_terminal();

        /**
         * @brief Method to run the Admin_terminal instance.
         * @note This method defines the runtime behavior of the Admin_terminal instance.
         */
        virtual void run();

        /**
         * @brief Method to add a command to the terminal.
         * @param cmd The command to add.
         * @throws std::runtime_error if the command is already registered.
         */
        virtual void add_command(const Terminal_command& cmd);

        /**
         * @brief Constructor for Terminal_command.
         * @param name The name of the command.
         * @param description A brief description of the command.
         * @param help Help text explaining the command usage.
         * @param callback The callback function to execute when the command is called.
         */
        virtual void add_command(std::string name, std::string description, std::string help,
                                 Terminal_command_callback callback);

        /**
         * @brief Method to remove a command from the terminal.
         * @param name The name of the command to remove.
         */
        virtual void remove_command(const std::string& name);

        /**
         * @brief Method to initialize the Admin_terminal instance.
         * @param modules Map of all modules managed by the application, including the module manager.
         * @note This method sets the modules_manager by retrieving it from the provided modules.
         */
        virtual void innit(const Modules& modules);

        /**
         * @brief Method to stop the Admin_terminal instance.
         * @note This method performs any necessary cleanup or shutdown procedures for the Admin_terminal instance.
         */
        virtual void stop();
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
