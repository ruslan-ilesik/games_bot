//
// Created by ilesik on 9/11/24.
//

#pragma once
#include "src/games/battleships/battleships.hpp"


#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    class Discord_battleships_game : public Discord_game {
        constexpr static int _place_timeout = 10*60; //seconds
        constexpr static int _field_size = 512;
        constexpr static int _distance_between_fields = _field_size/10;
        constexpr static int _line_width = 2;
        constexpr static double _sector_size = (_field_size - ((_field_size - _line_width * 11.0) / 10.0)- _line_width * 11.0) / 10.0;
        constexpr static int _line_length = _sector_size*10 + _line_width*10;
        constexpr static double _text_scale = _sector_size/45;
        constexpr static double _text_thickness = _sector_size / 25;

        constexpr static const Vector2i _image_size = {_field_size*2+_distance_between_fields,_field_size};
        inline static const Color _default_background{32,42,68};
        inline static const Color _line_color{255,255,255};
        inline static const Color _text_active{251,192,45};
        inline static const Color _text_standard{255,255,255};
        inline static std::map<battleships_engine::Ship_types,Color> _ship_colors
        {
                {battleships_engine::Ship_types::Carrier,{21,21,21}},
                {battleships_engine::Ship_types::Battleship,{37,37,37}},
                {battleships_engine::Ship_types::Patrol_Boat,{53,53,53}},
                {battleships_engine::Ship_types::Submarine,{69,69,69}},
                {battleships_engine::Ship_types::Destroyer,{85,85,85}}
        };
        inline static const Color _miss_cell_color{128,128,128};
        inline static const Color _damaged_cell_color{255,0,0};
        inline static const Color _occupied_cell_color{255,255,255,0.5};
        inline static const Color _selected_col_color {251,192,45,0.5};

        std::map<dpp::snowflake,dpp::message> _messages;

        std::map<dpp::snowflake,int> _user_to_player_id;

        time_t _game_start_time;
        std::array<int,2> _states= {0,0};
        std::array<std::array<int,2>,2> _temp_pos;
        battleships_engine::Battleships _engine;
        std::array<battleships_engine::Ship*,2> _temp_ships;

        std::array<bool,2> _last_was_back = {false,false};
        time_t _move_start;
        bool _is_timeout = false;
        dpp::message _message;
        std::mutex _mutex;

    public:
        Discord_battleships_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        Image_ptr generate_view_field(int state = 0);

        void draw_private_field(Image_ptr& image, const std::array<int,2>& position, const battleships_engine::Field& field,
                                const battleships_engine::Ships_container& ships, battleships_engine::Ship* to_not_draw = nullptr);

        void draw_ship(Image_ptr& image, const std::array<int,2>& position, battleships_engine::Ship* ship);

        void draw_public_field(Image_ptr &image, const std::array<int, 2> &position,
                                    const battleships_engine::Field &field,const battleships_engine::Ships_container& ships);

        dpp::task<void> run(dpp::button_click_t event);

        void end_of_game_timeout();

        dpp::task<void> player_place_ships(const dpp::snowflake player); //need an explicit copy because data gets corrupted on awaits.
    };
}; // namespace gb
