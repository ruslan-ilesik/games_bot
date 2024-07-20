//
// Created by ilesik on 7/20/24.
//

#pragma once

#include "src/module/module.hpp"
namespace gb {

    class Discord_command_help : public Module{
    public:
        Discord_command_help(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};
    };

} // gb