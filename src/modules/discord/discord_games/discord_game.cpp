//
// Created by ilesik on 7/24/24.
//

#include "discord_game.hpp"


namespace gb {
    std::vector<std::string> Discord_game::get_basic_game_dependencies() {
        return {"database","discord_bot","discord_games_manager","image_processing"};
    }

    Discord_game::Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players) {
        this->_players = players;
        this->_data = _data;
    }

    std::vector<dpp::snowflake> Discord_game::get_players() {
        return _players;
    }

    dpp::snowflake Discord_game::next_player() {
        _current_player_ind++;
        if (_current_player_ind >= _players.size()) {
            _current_player_ind = 0;
        }
        return get_current_player();
    }

    dpp::snowflake Discord_game::get_current_player() {
        return _players.at(_current_player_ind);
    }

    void Discord_game::remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user) {
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
} // gb