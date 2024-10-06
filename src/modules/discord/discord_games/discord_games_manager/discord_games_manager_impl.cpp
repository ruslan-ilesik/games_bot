//
// Created by ilesik on 7/24/24.
//

#include "discord_games_manager_impl.hpp"

namespace gb {
    Task<Database_return_t> Discord_games_manager_impl::add_game(Discord_game *game, const dpp::snowflake &channel_id,
                                                                 const dpp::snowflake &guild_id) {
        std::unique_lock lock(_mutex);
        _games.push_back(game);
        return _db->execute_prepared_statement(_create_game_stmt, game->get_name(), channel_id, guild_id);
    }

    void Discord_games_manager_impl::remove_game(Discord_game *game, GAME_END_REASON end_reason,
                                                 const std::string &additional_data) {
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
        _db->background_execute_prepared_statement(_finish_game_stmt, end_r_str, game->get_image_cnt(), additional_data,
                                                   game->get_uid());
        _cv.notify_all();
    }

    void Discord_games_manager_impl::record_user_result(Discord_game *game, const dpp::snowflake &player,
                                                        const std::string &result) {
        _db->background_execute_prepared_statement(_user_game_result_stmt, result, game->get_uid(), player,
                                                   game->get_uid());
    }

    Task<time_t> Discord_games_manager_impl::get_seconds_since_last_game(const std::string &game_name,
                                                                   const dpp::snowflake &user_id) {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_last_user_game_played,game_name,user_id);
        if (r.empty()) {
            co_return std::numeric_limits<time_t>::max();
        }
        time_t value;
        std::string s = r.at(0).at("time_diff");
        std::from_chars(s.data(), s.data() + s.size(), value);
        co_return value;
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
        _db->remove_prepared_statement(_get_last_user_game_played);
    }

    void Discord_games_manager_impl::run() {}

    void Discord_games_manager_impl::init(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _create_game_stmt = _db->create_prepared_statement("CALL create_game(?,?,?);");

        _user_game_result_stmt = _db->create_prepared_statement(
            R"XXX(INSERT INTO `user_game_results` (`result`, `game`, `user`, `time_played`)
      VALUES (
        ?,
        ?,
        ?,
        TIMEDIFF(UTC_TIMESTAMP(), (SELECT `start_time` FROM `games_history` WHERE id = ?))
      );
      )XXX");


        _get_last_user_game_played = _db->create_prepared_statement(R"XXX(
        select TIMESTAMPDIFF(SECOND, start_time, UTC_TIMESTAMP()) as time_diff
        from
	        games_history join user_game_results on
		        games_history.id = user_game_results.game and
                games_history.game_name=? and
                user_game_results.user = ?
        ORDER BY
	        start_time DESC
        LIMIT 1
        ;
        )XXX");

        _finish_game_stmt =
            _db->create_prepared_statement("UPDATE `games_history` SET `end_time`=UTC_TIMESTAMP(), `game_state`=?, "
                                           "`images_generated`=?, `additional_data`=? WHERE `id`=?");
    }

    Discord_games_manager_impl::Discord_games_manager_impl() :
        Discord_games_manager("discord_games_manager", {"database"}) {}

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_games_manager_impl>()); }
} // namespace gb
