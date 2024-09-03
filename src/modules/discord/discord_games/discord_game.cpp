//
// Created by ilesik on 7/24/24.
//

#include "discord_game.hpp"


namespace gb {
std::string to_string(USER_REMOVE_REASON r) {
  switch (r) {
  case USER_REMOVE_REASON::UNKNOWN:
    return "UNKNOWN";
  case USER_REMOVE_REASON::TIMEOUT:
    return "TIMEOUT";
  case USER_REMOVE_REASON::LOSE:
    return "LOSE";
  case USER_REMOVE_REASON::WIN:
    return "WIN";
  case USER_REMOVE_REASON::DRAW:
    return "DRAW";
  case USER_REMOVE_REASON::SAVED:
    return "SAVED";
  default:
    throw std::runtime_error(
        "No known way to convert given USER_REMOVE_REASON to string");
  }
}
Discord_game::~Discord_game() {
        game_stop();
    }

    std::vector<std::string> Discord_game::get_basic_game_dependencies() {
        return {"database","discord_bot","discord_games_manager","image_processing","discord_button_click_handler","discord_achievements_processing"};
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
        _data.games_manager->record_user_result(this,user,to_string(reason));
        // Adjust _current_player_ind after removal
        if (_players.empty()) {
            _current_player_ind = 0;
        } else if (_current_player_ind >= _players.size()) {
            _current_player_ind = _players.size() - 1;
        }
    }

    void Discord_game::game_start(const dpp::snowflake& channel_id,const dpp::snowflake& guild_id) {
        _game_create_req = _data.games_manager->add_game(this,channel_id,guild_id);
    }

    void Discord_game::game_stop() {
        _data.games_manager->remove_game(this,GAME_END_REASON::FINISHED);
    }

    std::string Discord_game::add_image(dpp::message &m, const Image_ptr &image) {
        auto str = image->convert_to_string();
        m.set_filename("test"+str.first).set_file_content(str.second);
        return "attachment://test"+str.first;
    }

    std::string Discord_game::get_name() const { return _data.name; }

    uint64_t Discord_game::get_uid() {
        if (_unique_game_id == std::numeric_limits<uint64_t>::max()) {
            Database_return_t r = sync_wait(_game_create_req);
            std::istringstream iss(r.at(0)["id"]);
            iss >> _unique_game_id;
        }
        return _unique_game_id;
    }

    uint64_t Discord_game::get_image_cnt() const { return _img_cnt; }
} // gb
