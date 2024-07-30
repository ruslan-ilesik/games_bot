//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "src/modules/discord/discord_games/discord_game.hpp"
#include "src/module/module.hpp"

namespace gb {

    class Discord_game_command : public  Module {
    protected:
        Database_ptr _db;
        Discord_bot_ptr _bot;
        Discord_games_manager_ptr _games_manager;
    public:
        Discord_game_command(const std::string& name, const std::vector<std::string>& dependencies);

        virtual void innit(const Modules& modules);
    };

} // gb

