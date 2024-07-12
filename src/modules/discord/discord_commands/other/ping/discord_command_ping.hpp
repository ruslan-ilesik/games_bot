//
// Created by ilesik on 7/12/24.
//

#pragma once

#include "src/module/module.hpp"

namespace gb{

    /*
     * Abstract class for implementation of ping command to get rest ping between bot and discord
     */
    class Discord_command_ping : public Module{
    public:
        Discord_command_ping(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};
    };

} //end gb