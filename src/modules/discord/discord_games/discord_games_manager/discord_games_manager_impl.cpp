//
// Created by ilesik on 7/24/24.
//

#include "discord_games_manager_impl.hpp"

namespace gb {
Task<Database_return_t>
Discord_games_manager_impl::add_game(Discord_game *game) {
  std::unique_lock lock(_mutex);
  _games.push_back(game);
  return _db->execute_prepared_statement(_create_game_stmt, game->get_name());
}

void Discord_games_manager_impl::remove_game(Discord_game *game, GAME_END_REASON end_reason) {
  std::unique_lock lock(_mutex);
  auto e = std::ranges::remove(_games, game);
  _games.erase(e.begin(), e.end());
  std::string end_r_str;
  switch (end_reason) {
  case GAME_END_REASON::ERROR:
    end_r_str = "ERROR";
    break;
  case GAME_END_REASON::SAVED:
    end_r_str = "SAVED";
    break;
  case GAME_END_REASON::FINISHED:
    end_r_str = "FINISHED";
    break;
  default:
      throw std::runtime_error("No known to_string conversion for GAME_END_REASON");
  }
  _db->background_execute_prepared_statement(_finish_game_stmt,end_r_str,game->get_image_cnt(),game->get_uid());
  _cv.notify_all();
}

void Discord_games_manager_impl::record_user_result(
    Discord_game *game, const dpp::snowflake &player,
    const std::string &result) {
    _db->background_execute_prepared_statement(_user_game_result_stmt,result,game->get_uid(),player,game->get_uid());
}

void Discord_games_manager_impl::stop() {
  std::mutex m;
  std::unique_lock lk(m);
  _cv.wait(lk, [this]() {
    std::unique_lock lk(_mutex);
    return _games.empty();
  });
  _db->remove_prepared_statement(_create_game_stmt);
  _db->remove_prepared_statement(_user_game_result_stmt);
  _db->remove_prepared_statement(_finish_game_stmt);
}

void Discord_games_manager_impl::run() {}

void Discord_games_manager_impl::init(const Modules &modules) {
  _db = std::static_pointer_cast<Database>(modules.at("database"));
  _create_game_stmt = _db->create_prepared_statement("CALL create_game(?);");

  _user_game_result_stmt = _db->create_prepared_statement(
      R"XXX(INSERT INTO `user_game_results` (`result`, `game`, `user`, `time_played`)
      VALUES (
        ?,
        ?,
        ?,
        TIMEDIFF(UTC_TIMESTAMP(), (SELECT `start_time` FROM `games_history` WHERE id = ?))
      );
      )XXX"
      );

  _finish_game_stmt = _db->create_prepared_statement("UPDATE `games_history` SET `end_time`=UTC_TIMESTAMP(), `game_state`=?, `images_generated`=? WHERE `id`=?");
}

Discord_games_manager_impl::Discord_games_manager_impl()
    : Discord_games_manager("discord_games_manager", {"database"}) {}

Module_ptr create() {
  return std::dynamic_pointer_cast<Module>(
      std::make_shared<Discord_games_manager_impl>());
}
} // namespace gb
