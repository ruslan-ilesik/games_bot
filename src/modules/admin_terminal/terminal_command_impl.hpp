//
// Created by ilesik on 7/12/24.
//

#pragma once

#include <string>
#include <functional>
#include <vector>

#include "./terminal_command.hpp"
namespace gb {

    /**
     * @class Terminal_command_impl
     * @brief Implementation of the Terminal_command interface.
     */
    class Terminal_command_impl: public Terminal_command {
    private:
        std::string _name; ///< Name of the terminal command.
        std::string _description; ///< Description of the terminal command.
        std::string _help; ///< Help text for the terminal command.
        Terminal_command_callback _callback; ///< Callback function to execute the command.

    public:
        /**
         * @brief Constructor for Terminal_command_impl.
         * @param name The name of the terminal command.
         * @param description The description of the terminal command.
         * @param help The help text for the terminal command.
         * @param callback The callback function to execute when the command is invoked.
         */
        Terminal_command_impl(const std::string& name, const std::string& description,
                              const std::string& help, const Terminal_command_callback& callback);

        /**
         * @brief Getter for the name of the terminal command.
         * @return The name of the terminal command.
         */
        std::string get_name() const override;

        /**
         * @brief Getter for the description of the terminal command.
         * @return The description of the terminal command.
         */
        std::string get_description() const override;

        /**
         * @brief Getter for the help text of the terminal command.
         * @return The help text of the terminal command.
         */
        std::string get_help() const override;

        /**
         * @brief Operator () overload to execute the terminal command.
         * @param arguments The arguments passed to the terminal command.
         */
        void operator()(const std::vector<std::string>& arguments) override;
    };

} // namespace gb

