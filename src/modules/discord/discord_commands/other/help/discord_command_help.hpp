//
// Created by ilesik on 7/20/24.
//

#pragma once

#include "src/module/module.hpp"

namespace gb {

    /**
     * @class Discord_command_help
     * @brief Abstract base class for handling Discord command help functionality.
     *
     * This class extends the Module class and provides an interface for implementing
     * help commands that can be used in a Discord bot to assist users in getting information
     * about available commands.
     */
    class Discord_command_help : public Module {
    public:
        /**
         * @brief Constructor for Discord_command_help.
         *
         * Initializes the base Module class with the given name and dependencies.
         *
         * @param name The name of the module.
         * @param dependencies A vector of module names that this module depends on.
         */
        Discord_command_help(const std::string& name, const std::vector<std::string>& dependencies)
            : Module(name, dependencies) {};

    };

} // gb
