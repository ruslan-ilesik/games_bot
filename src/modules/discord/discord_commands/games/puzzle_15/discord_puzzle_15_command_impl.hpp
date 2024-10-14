//
// Created by ilesik on 10/14/24.
//

#pragma once

#include <src/modules/discord/premium_manager/premium_manager.hpp>


#include "./discord_puzzle_15_command.hpp"

namespace gb {

class Discord_puzzle_15_command_impl: public  Discord_puzzle_15_command {

    Premium_manager_ptr _premium;
public:
    Discord_puzzle_15_command_impl();
    virtual ~Discord_puzzle_15_command_impl() = default;

    void init(const Modules &modules) override;
};

extern "C" Module_ptr create();

} // gb
