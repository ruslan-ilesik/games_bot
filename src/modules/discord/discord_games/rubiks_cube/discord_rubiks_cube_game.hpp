//
// Created by ilesik on 9/18/24.
//

#pragma once
#include <random>
#include <src/games/rubiks_cube/rubiks_cube.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>


namespace rubiks_cube {
    struct isometric_data {
        gb::Vector2d start_p;
        gb::Vector2d vector_x;
        gb::Vector2d vector_y;

        isometric_data(gb::Vector2d start_p, gb::Vector2d vector_x, gb::Vector2d vector_y);

        gb::Vector2i get_dot(gb::Vector2d coord);
    };



    inline std::map<char, gb::Color> colors = {{'B', gb::Color(0, 0, 255)},   {'W', gb::Color(255, 255, 255)},
                                        {'R', gb::Color(255, 0, 0)},   {'G', gb::Color(0, 255, 0)},
                                        {'Y', gb::Color(255, 255, 0)}, {'O', gb::Color(255, 102, 0)}};
}

namespace gb {

    class Discord_rubiks_cube_game : public Discord_game {
        rubiks_cube::Rubiks_cube_engine _engine;
        bool _is_view = false;
        int _amount_moves = 0;
        std::default_random_engine _rand_obj;

    public:
        Discord_rubiks_cube_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        dpp::task<void> run(dpp::slashcommand_t sevent);

        void prepare_message(dpp::message& message);

        Image_ptr create_image();

        bool is_win();
    };

} // namespace gb
