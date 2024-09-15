//
// Created by ilesik on 9/15/24.
//

#pragma once

#include "src/games/chess/chess/chess.h"

#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

class Discord_chess_game : public Discord_game {
public:
  Discord_chess_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

  dpp::task<void> run(dpp::button_click_t event);
  static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();
};

} // gb
