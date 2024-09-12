//
// Created by ilesik on 9/9/24.
//

#pragma once

#include "./discord_battleships_command.hpp"

namespace gb {

    class Discord_battleships_command_impl : public Discord_battleships_command {
    public:
        Discord_battleships_command_impl();

        void run() override;

        void stop() override;

        void init(const Modules &modules) override;
    };

    extern "C" Module_ptr create();

} // namespace gb
