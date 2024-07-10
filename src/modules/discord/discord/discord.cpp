//
// Created by ilesik on 7/10/24.
//

#include "discord.hpp"
#include "./discord_command.cpp"


namespace gb {
    class Discord_command_impl;
    Discord::Discord() : Module("discord",{}){

    }

    void Discord::run() {}

    void Discord::stop() {}

    void Discord::innit(const Modules &modules) {}

    Discord_command_ptr
    Discord::create_discord_command(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                                    const Command_data &command_data) {
        return std::dynamic_pointer_cast<Discord_command>(std::make_shared<Discord_command_impl>(command,handler,command_data));
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord>());
    }
} // gb