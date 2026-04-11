//
// Created by ilesik on 4/11/26.
//

#pragma once
#include "src/modules/discord/discord_commands/discord_general_command.hpp"

namespace gb {
    class Discord_commands_balance_transactions: public Discord_general_command {
    public:
        Discord_commands_balance_transactions(const std::string &name, const std::vector<std::string> &dependencies) :
            Discord_general_command(name, dependencies) {}
    };
}
