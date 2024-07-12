//
// Created by ilesik on 7/12/24.
//

#pragma once

#include "./discord_command_ping.hpp"
#include "../../../discord_command_handler/discord_command_handler.hpp"
#include "../../../discord_bot/discord_bot.hpp"


namespace gb {

    class Discord_command_ping_impl : public Discord_command_ping {
    private:
        Discord_command_handler_ptr _command_handler;
        Discord_bot_ptr _discord_bot;
    public:

        Discord_command_ping_impl();

        void stop() override;

        void run() override;

        void innit(const Modules &modules) override;
    };

    extern "C" Module_ptr create();

} // gb

