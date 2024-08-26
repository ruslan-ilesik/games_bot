//
// Created by ilesik on 7/24/24.
//

#include "discord_game.hpp"


namespace gb {
    Discord_game::~Discord_game() {
        game_stop();
    }

    std::vector<std::string> Discord_game::get_basic_game_dependencies() {
        return {"database","discord_bot","discord_games_manager","image_processing","discord_button_click_handler"};
    }

    Discord_game::Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players): _game_create_req({}) {
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

    size_t Discord_game::get_current_player_index() const {
        return _current_player_ind;
    }

    void Discord_game::remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user) {
        auto e = std::find(_players.begin(), _players.end(), user);
        if (e == _players.end()) {
            return;
        }
        size_t index = std::distance(_players.begin(), e);
        if (index <= _current_player_ind && _current_player_ind > 0) {
            _current_player_ind--;
        }

        _players.erase(_players.begin() + index);

        // Adjust _current_player_ind after removal
        if (_players.empty()) {
            _current_player_ind = 0;
        } else if (_current_player_ind >= _players.size()) {
            _current_player_ind = _players.size() - 1;
        }
    }

    void Discord_game::game_start() {
        _game_create_req = _data.games_manager->add_game(this);

    }

    void Discord_game::game_stop() {
        _data.games_manager->remove_game(this);
    }

    std::string Discord_game::add_image(dpp::message &m, const Image_ptr &image) {
        auto str = image->convert_to_string();
        m.set_filename("test"+str.first).set_file_content(str.second);
        return "attachment://test"+str.first;
    }

    std::string Discord_game::get_name() const { return _data.name; }

    uint64_t Discord_game::get_uid() {
      if (_unique_game_id == std::numeric_limits<uint64_t>::max()) {
        std::istringstream iss(sync_wait(_game_create_req).at(0)[""]);
        iss >> _unique_game_id;
      }
      return _unique_game_id;
    }
    } // gb
