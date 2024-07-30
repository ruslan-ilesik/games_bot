//
// Created by ilesik on 7/31/24.
//

#include "discord_game_command.hpp"

namespace gb {
    Discord_game_command::Discord_game_command(const std::string &name, const std::vector<std::string> &dependencies) : Module(name,dependencies){
        // insert basic game dependencies if they are not there
        for (auto& i: Discord_game::get_basic_game_dependencies()){
            if (std::find(_dependencies.begin(), _dependencies.end(), i) == _dependencies.end()) {
                _dependencies.push_back(i);
            }
        }
    }

    void Discord_game_command::innit(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        _games_manager = std::static_pointer_cast<Discord_games_manager>(modules.at("discord_games_manager"));
    }
} // gb