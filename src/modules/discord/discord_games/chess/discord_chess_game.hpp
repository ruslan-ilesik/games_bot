//
// Created by ilesik on 9/15/24.
//

#pragma once

#include "src/games/chess/chess/chess.h"

#include <src/modules/discord/discord_games/discord_game.hpp>

namespace chess {
    inline std::map<char, std::tuple<std::string, dpp::snowflake>> emojis = {
        {'.', {"clear", 1015646216509468683}}, {'B', {"B_", 883415614755045397}}, {'K', {"K_", 883415619649806337}},
        {'N', {"N_", 883415619662413854}},     {'P', {"P_", 883415619863740466}}, {'Q', {"Q_", 883415619679182898}},
        {'R', {"R_", 883415619343622176}},     {'b', {"b1", 883415614922838016}}, {'k', {"k1", 883415619301687337}},
        {'n', {"n1", 883415619289112667}},     {'p', {"p1", 883415619624632390}}, {'q', {"q1", 883415619633025104}},
        {'r', {"r1", 883415619733688320}},
    };

}

namespace gb {

    class Discord_chess_game : public Discord_game {
    private:
        chess::Board _board;
        bool _choose_figure = true;
        std::string _selected_figure = "";
        std::vector<std::string> _possible_places_to_go;
        bool _next = false;
        int _moves_amount = 0;
        bool is_view = false;

    public:
        static Vector2i chess_board_cords_to_numbers(std::string to_convert);

        Image_ptr generate_image();


        Discord_chess_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        dpp::task<void> run(dpp::button_click_t event, int timeout = 60);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();
    };

} // namespace gb
