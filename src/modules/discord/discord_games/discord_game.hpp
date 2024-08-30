//
// Created by ilesik on 7/24/24.
//

#pragma once

#include <dpp/dpp.h>
#include <src/modules/discord/discord_interactions_handler/discord_button_click_handler/discord_button_click_handler.hpp>

#include "./discord_games_manager/discord_games_manager.hpp"
#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"
#include "src/modules/image_processing/image_processing.hpp"
#include <src/modules/discord/discord_achievements_processing/discord_achievements_processing.hpp>

namespace gb {

enum class USER_REMOVE_REASON { UNKNOWN, TIMEOUT, LOSE, WIN, DRAW, SAVED };

std::string to_string(USER_REMOVE_REASON r);

struct Game_data_initialization {
  std::string name;
  Database_ptr db;
  Discord_bot_ptr bot;
  Discord_games_manager_ptr games_manager;
  Image_processing_ptr image_processing;
  Discord_button_click_handler_ptr button_click_handler;
  Discord_achievements_processing_ptr achievements_processing;
};

class Discord_game {
private:
  std::vector<dpp::snowflake> _players;
  size_t _current_player_ind = 0;
  Task<Database_return_t> _game_create_req;
  uint64_t _unique_game_id =
      std::numeric_limits<uint64_t>::max(); // placeholder for no value, never
                                            // should be reached withing
                                            // database.
protected:
  Game_data_initialization _data;
  uint64_t _img_cnt = 0;

public:
  virtual ~Discord_game();

  static std::vector<std::string> get_basic_game_dependencies();

  static std::vector<std::pair<std::string, image_generator_t>>
  get_image_generators();

  Discord_game(Game_data_initialization &_data,
               const std::vector<dpp::snowflake> &players);

  std::vector<dpp::snowflake> get_players();

  dpp::snowflake next_player();

  dpp::snowflake get_current_player();

  size_t get_current_player_index() const;

  void remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user);

  void game_start();

  void game_stop();

  std::string get_name() const;

  uint64_t get_uid();

  uint64_t get_image_cnt() const;

  std::string add_image(dpp::message &m, const Image_ptr &image);
};

} // namespace gb
