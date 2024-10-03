//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

class Discord_hangman_game: public Discord_game{
public:
  Discord_hangman_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

  static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

  dpp::task<void> run(dpp::slashcommand_t sevent);
  dpp::task<void> run(dpp::button_click_t event);
};

} // gb
