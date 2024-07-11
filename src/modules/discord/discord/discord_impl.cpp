//
// Created by ilesik on 7/10/24.
//

#include "discord_impl.hpp"
#include "./discord_command_impl.cpp"

namespace gb {

    Discord_impl::Discord_impl() : Discord("discord", {}) {}

    void Discord_impl::run() {}

    void Discord_impl::stop() {}

    void Discord_impl::innit(const Modules &modules) {}

    Discord_command_ptr
    Discord_impl::create_discord_command(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                                    const Command_data &command_data) {
        return std::dynamic_pointer_cast<Discord_command>(std::make_shared<Discord_command_impl>(command, handler, command_data));
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_impl>());
    }

} // gb
