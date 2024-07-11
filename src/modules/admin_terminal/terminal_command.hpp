//
// Created by ilesik on 7/6/24.
//

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace gb {

    /**
     * @brief Alias for the callback function type used in terminal commands.
     * The callback function takes a vector of strings (arguments) and returns void.
     */
    typedef std::function<void(std::vector<std::string>)> Terminal_command_callback;

    /**
     * @brief The Terminal_command class represents a command that can be executed in the terminal.
     * Each command has a name, description, help text, and a callback function that is executed when the command is called.
     */
    class Terminal_command {
    public:
        /**
         * @brief Constructor for Terminal_command.
         */
        Terminal_command() = default;

        /**
         * @brief Get the name of the command.
         * @return The name of the command.
         */
        virtual std::string get_name() const = 0;

        /**
         * @brief Get the description of the command.
         * @return The description of the command.
         */
        virtual std::string get_description() const = 0;

        /**
         * @brief Get the help text of the command.
         * @return The help text of the command.
         */
        virtual std::string get_help() const = 0;

        /**
         * @brief Operator overload to call the command's callback function.
         * @param arguments A vector of strings representing the arguments passed to the command.
         */
        virtual void operator()(const std::vector<std::string> &arguments) = 0;
    };

    /**
     * @brief Alias for a shared pointer to a Terminal_command.
     */
    typedef std::shared_ptr<Terminal_command> Terminal_command_ptr;

} // namespace gb
