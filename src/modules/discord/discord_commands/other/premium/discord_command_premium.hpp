//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/discord_commands/discord_general_command.hpp>

namespace gb {
    /*
     * Abstract class for implementation of premium command to get premium status of user.
     */
    class Discord_command_premium : public Discord_general_command {
    public:
        /**
         * @brief Constructor for Discord_command_ping.
         *
         * Initializes the base Module class with the given name and dependencies.
         *
         * @param name The name of the module.
         * @param dependencies A vector of module names that this module depends on.
         */
        Discord_command_premium(const std::string &name, const std::vector<std::string> &dependencies) :
            Discord_general_command(name, dependencies){};
    };

} // namespace gb
