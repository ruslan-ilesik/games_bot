//
// Created by ilesik on 7/24/24.
//

#pragma once

#include <dpp/dpp.h>

#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"
#include "./discord_games_manager/discord_games_manager.hpp"
#include "src/modules/image_processing/image_processing.hpp"


namespace gb {

    enum class USER_REMOVE_REASON {
        UNKNOWN,
        TIMEOUT,
        LOSE,
        WIN,
        DRAW,
        SAVED
    };

    struct Game_data_initialization {
        std::string name;
        Database_ptr db;
        Discord_bot_ptr bot;
        Discord_games_manager_ptr games_manager;
        Image_processing_ptr image_processing;
    };

    class Discord_game {
    private:
        std::vector<dpp::snowflake> _players;
        size_t _current_player_ind;
        Game_data_initialization _data;
    public:
        virtual ~Discord_game() = default;

        static std::vector<std::string> get_basic_game_dependencies();

        Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        std::vector<dpp::snowflake> get_players();

        dpp::snowflake next_player();

        dpp::snowflake get_current_player();

        void remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user);
    };

} // gb


