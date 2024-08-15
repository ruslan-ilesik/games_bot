//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "../discord_game.hpp"

namespace gb {

    class Discord_tic_tac_toe_game: public Discord_game{
    public:
        Discord_tic_tac_toe_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string,image_generator_t>> get_image_generators(){

            return {};
        }

        static Image_ptr board_image() {
            //_data.image_processing->create_image();
            return {nullptr};
        }

        void create_image(dpp::embed& embed) {

        }

        void run(const dpp::button_click_t& _event) {
            game_start();
            dpp::button_click_t event = _event;
            dpp::message m;
            dpp::embed embed;
            embed.set_color(dpp::colors::blue)
                .set_title("Tic Tac Toe game.");

            m.add_embed(embed);
            _data.bot->reply(event,m);

            get_current_player();

            game_stop();

        }

    };

} // gb

