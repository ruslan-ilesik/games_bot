//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "../discord_game.hpp"

namespace gb {
class Discord_tic_tac_toe_game : public Discord_game {
  enum class SIGNS { X = 0, O = 1, EMPTY };

  std::array<std::array<SIGNS, 3>, 3> board = {
      {{SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
       {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
       {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY}}};

  void create_image(dpp::message &m, dpp::embed &embed);

  void create_components(dpp::message &m);

  dpp::message win(bool timeout = false);

  dpp::message draw();

public:
  Discord_tic_tac_toe_game(Game_data_initialization &_data,
                           const std::vector<dpp::snowflake> &players);

  static std::vector<std::pair<std::string, image_generator_t>>
  get_image_generators();

  dpp::task<void> run(const dpp::button_click_t &_event);
};
} // namespace gb
