//
// Created by ilesik on 10/3/24.
//

#include "discord_hangman_game.hpp"

namespace gb {
    Discord_hangman_game::Discord_hangman_game(Game_data_initialization &_data,
                                               const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_hangman_game::get_image_generators() { return {}; }

    dpp::task<void> Discord_hangman_game::run(dpp::slashcommand_t sevent) {
        co_return;
    }

    dpp::task<void> Discord_hangman_game::run(dpp::button_click_t event) {
        co_return;
    }
} // gb