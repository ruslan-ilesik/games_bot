//
// Created by ilesik on 7/24/24.
//

#pragma once

#include <dpp/dpp.h>

#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"


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
    };

    class Discord_game {
    private:
        std::vector<dpp::snowflake> _players;
        size_t _current_player_ind;
        Game_data_initialization _data;
    public:
        Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players) {
            this->_players = players;
            this->_data = _data;
        }

        std::vector<dpp::snowflake> get_players() {
            return _players;
        }

        dpp::snowflake next_player() {
            _current_player_ind++;
            if (_current_player_ind >= _players.size()) {
                _current_player_ind = 0;
            }
            return get_current_player();
        }

        dpp::snowflake get_current_player() {
            return _players.at(_current_player_ind);
        }

        void remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user) {
            auto e = std::find(_players.begin(), _players.end(), user);
            if (e == _players.end()) {
                return;
            }
            size_t index = std::distance(_players.begin(), e);
            if (index <= _current_player_ind) {
                _current_player_ind--;
                if (_current_player_ind == 0) {
                    if (_players.size() < 2) {
                        _current_player_ind = 0;
                    } else {
                        _current_player_ind = _players.size() - 2;
                    }
                }
            }
            _players.erase(_players.begin() + index);
        }

        virtual void run() = 0;

    };

} // gb


