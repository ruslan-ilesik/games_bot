//
// Created by ilesik on 7/6/24.
//

#pragma once

#include "admin_terminal.hpp"
#include "terminal_command_impl.hpp"
#include <unistd.h>      // for pipe
#include <cstring>       // for memset
#include <poll.h>
#include <iostream>
#include <mutex>
#include <format>

namespace gb {

    /**
     * @class Admin_terminal_impl
     * @brief Implementation of the Admin_terminal module.
     */
    class Admin_terminal_impl : public Admin_terminal {
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
         * @brief Map of terminal command ptrs, indexed by command name.
         */
        std::map<std::string, Terminal_command_ptr> _commands;

        /**
         * @brief Pipe file descriptors for communication between threads.
         * Used to stop user input waiting thread.
         */
        int _pipe_fd[2];

        /**
         * @brief Splits a string into tokens based on a delimiter.
         * @param s The input string to split.
         * @param delimiter The delimiter string to split by.
         * @return Vector of tokens split from the input string.
         */
        std::vector<std::string> split(const std::string &s, const std::string &delimiter);

    public:
        /**
         * @brief Constructor for Admin_terminal_impl.
         */
        Admin_terminal_impl();

        /**
         * @brief Destructor for Admin_terminal_impl.
         */
        virtual ~Admin_terminal_impl();

        /**
         * @brief Stops the admin terminal thread and cleans up resources.
         */
        virtual void stop() override;

        /**
         * @brief Initializes the admin terminal with modules.
         * @param modules The map of modules to initialize.
         */
        virtual void init(const Modules &modules) override;

        /**
         * @brief Adds a command to the admin terminal.
         * @param cmd The pointer to the command to add.
         */
        virtual void add_command(const Terminal_command_ptr &cmd) override;

        /**
         * @brief Starts running the admin terminal interface.
         */
        virtual void run() override;

        /**
         * @brief Removes a command from the admin terminal.
         * @param name The name of the command to remove.
         */
        virtual void remove_command(const std::string &name) override;

        /**
         * @brief Adds a command to the admin terminal.
         * @param name The name of the command.
         * @param description The description of the command.
         * @param help The help information for the command.
         * @param callback The callback function to execute when the command is invoked.
         */
        virtual void add_command(std::string name, std::string description, std::string help,
                                 Terminal_command_callback callback) override;
    };

    /**
     * @brief Function to create a new instance of the Admin_terminal_impl module.
     * @return A shared pointer to the newly created Admin_terminal_impl module.
     */
    Module_ptr create();

} // namespace gb
