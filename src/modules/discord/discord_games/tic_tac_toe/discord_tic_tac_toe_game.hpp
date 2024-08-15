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
            image_generator_t base = [](const Image_processing_ptr& image_processing, const Vector2i& resolution) {
                auto image = image_processing->create_image(resolution,{0,0,0,0});
                return image;
            };
            return {{"tic_tac_toe_base",base}};
        }

        static Image_ptr board_image() {
            //_data.image_processing->create_image();
            return {nullptr};
        }

        void create_image(dpp::message&m,dpp::embed& embed) {
            auto base = _data.image_processing->cache_get("tic_tac_toe_base",{256,256});
            embed.set_image(add_image(m,base));
        }

        void run(const dpp::button_click_t& _event) {
            game_start();
            dpp::button_click_t event = _event;
            dpp::message m;
            dpp::embed embed;
            embed.set_color(dpp::colors::blue)
                .set_title("Tic Tac Toe game.");
            create_image(m,embed);
            m.add_embed(embed);
            _data.bot->reply(event,m);
            get_current_player();
            game_stop();

        }

    };

} // gb

