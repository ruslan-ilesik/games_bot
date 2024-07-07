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
    private:
        /**
        * @brief The name of the command.
        */
        std::string _name;

        /**
         * @brief A brief description of the command.
         */
        std::string _description;

        /**
         * @brief Help text explaining the command usage.
         */
        std::string _help;

        /**
        * @brief The callback function executed when the command is called.
        */
        Terminal_command_callback _callback;

    public:
        /**
         * @brief Constructor for Terminal_command.
         * @param name The name of the command.
         * @param description A brief description of the command.
         * @param help Help text explaining the command usage.
         * @param callback The callback function to execute when the command is called.
         */
        Terminal_command(std::string name, std::string description, std::string help, Terminal_command_callback callback);

        /**
         * @brief Get the name of the command.
         * @return The name of the command.
         */
        std::string get_name() const;

        /**
         * @brief Get the description of the command.
         * @return The description of the command.
         */
        std::string get_description() const;

        /**
         * @brief Get the help text of the command.
         * @return The help text of the command.
         */
        std::string get_help() const;

        /**
         * @brief Operator overload to call the command's callback function.
         * @param arguments A vector of strings representing the arguments passed to the command.
         */
        void operator()(const std::vector<std::string>& arguments);
    };

    /**
     * @brief Alias for a shared pointer to a Terminal_command.
     */
    typedef std::shared_ptr<Terminal_command> Terminal_command_ptr;

} // namespace gb
