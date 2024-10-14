//
// Created by ilesik on 10/14/24.
//

#pragma once
#include <src/games/puzzle_15/puzzle_15.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

class Discord_puzzle_15_game: public Discord_game{

  puzzle_15::Puzzle_15 _engine;
public:
  Discord_puzzle_15_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

  static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();
};

} // gb
