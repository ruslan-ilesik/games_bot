//
// Created by ilesik on 9/29/24.
//

#pragma once
#include <random>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    class Discord_game_2048 : public Discord_game {
        unsigned int board[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        std::vector<std::array<int, 2>> clear_places;
        std::map<char, bool> possible_moves; //  w s a d
        std::default_random_engine rand_obj;

    public:
        Discord_game_2048(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        dpp::task<void> run(dpp::slashcommand_t sevent);

        Image_ptr create_image();

        void prepare_message(dpp::message& message);

        void up(unsigned int a[4][4]);

        void down(unsigned int a[4][4]);

        void left(unsigned int a[4][4]);

        void right(unsigned int a[4][4]);

        void find_clear();

        void get_possible_moves();

        void new_peace();
    };

} // namespace gb
