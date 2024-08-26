//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "./discord_tic_tac_toe_command.hpp"
#include "src/modules/discord/discord/discord.hpp"
#include "src/modules/discord/discord_command_handler/discord_command_handler.hpp"

namespace gb {

    class Discord_tic_tac_toe_command_impl: public Discord_tic_tac_toe_command {
        Discord_command_handler_ptr _command_handler;
        Discord_ptr _discord;

    public:
        Discord_tic_tac_toe_command_impl();

        void init(const Modules &modules) override;

        void stop() override;
    };

    extern "C" Module_ptr create();

} // gb


