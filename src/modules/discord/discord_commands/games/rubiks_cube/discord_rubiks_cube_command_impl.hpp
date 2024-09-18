//
// Created by ilesik on 9/18/24.
//

#pragma once

#include "./discord_rubiks_cube_command.hpp"
namespace gb {

    class Discord_rubiks_cube_command_impl : public Discord_rubiks_cube_command {
    public:
        Discord_rubiks_cube_command_impl();

        void run() override;

        void init(const Modules &modules) override;

        void stop() override;
    };

    extern "C" Module_ptr create();

} // namespace gb
