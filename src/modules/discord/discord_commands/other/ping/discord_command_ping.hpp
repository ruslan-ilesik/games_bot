//
// Created by ilesik on 7/12/24.
//

#pragma once

#include "src/module/module.hpp"

#include <src/modules/discord/discord_commands/discord_general_command.hpp>

namespace gb{

    /*
     * Abstract class for implementation of ping command to get rest ping between bot and discord
     */
    class Discord_command_ping : public Discord_general_command{
    public:
        Discord_command_ping(const std::string& name, const std::vector<std::string>& dependencies): Discord_general_command(name,dependencies){};
    };

} //end gb