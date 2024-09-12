//
// Created by ilesik on 9/11/24.
//

#include "discord_battleships_game.hpp"

namespace gb {
    Discord_battleships_game::Discord_battleships_game(Game_data_initialization &_data,
                                                       const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {
        int cnt = 0;
        for (auto &i: players) {
            _user_to_player_id.emplace(i, cnt);
            cnt++;
        }
    }

    std::vector<std::pair<std::string, image_generator_t>> Discord_battleships_game::get_image_generators() {
        image_generator_t grid_generator = [=](const Image_processing_ptr &image_processing,
                                               const Vector2i &resolution) {
            if (resolution.x != resolution.y) {
                throw std::runtime_error("Battleships grid should be squared image");
            }
            Image_ptr img = image_processing->create_image(resolution, _default_background);
            int field_size = resolution.x;
            int line_width = std::max(field_size / 256, 1);
            double sector_size = ((field_size- line_width * 11.0) / 10.0 );

            // horizontal lines
            for (int i = 0; i < 11; i++) {
                img->draw_line({0, static_cast<int>(i * sector_size + i * line_width)},
                               {static_cast<int>(10.0 * sector_size + 10.0 * line_width), static_cast<int>(i * sector_size + i * line_width)}, _line_color,
                               line_width);
            }

            // vertical
            for (int i = 0; i < 11; i++) {
                img->draw_line({static_cast<int>(i * sector_size + i * line_width), 0},
                               {static_cast<int>(i * sector_size + i * line_width), static_cast<int>(10.0 * sector_size + 10.0 * line_width)}, _line_color,
                               line_width);
            }
            return img;
        };
        return {{"battleships_grid", grid_generator}};
    }

    Image_ptr Discord_battleships_game::generate_view_field(int state) {
        Image_ptr img = _data.image_processing->create_image({_field_size, _field_size}, _default_background);
        auto tmp =
            _data.image_processing->cache_get("battleships_grid", {static_cast<int>(_field_size - _sector_size),
                                                                   static_cast<int>(_field_size - _sector_size)});
        img->overlay_image(img, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)});
        std::array<int, 2> position = {static_cast<int>(_sector_size),
                                       static_cast<int>(_sector_size - _line_width * 2)};

        for (int i = 0; i < 10; i++) {
            std::string text = std::string(1, static_cast<char>(65 + i));
            Vector2i t_size = img->get_text_size(text, _text_scale, _text_thickness);
            int offset = abs(static_cast<int>(_sector_size) - t_size.x) / 2;
            img->draw_text(text,
                           {static_cast<int>(position[0] + i * _sector_size + i * _line_width + offset), position[1]},
                           _text_scale, state == 1 ? _text_active : _text_standard, _text_thickness);
        }
        position = {0, static_cast<int>(_sector_size * 1.75)};
        for (int i = 0; i < 10; i++) {
            img->draw_text(
                std::to_string(i + 1),
                {static_cast<int>(position[0] + (i == 9 ? 0 : _sector_size / 4)), static_cast<int>(position[1] + i * _sector_size + i * _line_width)},
                _text_scale, state == 2 ? _text_active : _text_standard, _text_thickness);
        }
        return img;
    }

    dpp::task<void> Discord_battleships_game::run(const dpp::button_click_t &event) {
        _data.bot->reply(event, dpp::message().add_embed(
                                    dpp::embed().set_color(dpp::colors::blue).set_title("Game is starting...")));
        Direct_messages_return messages = co_await get_private_messages(get_players());
        if (messages.first) {
            std::vector<dpp::snowflake> missing_players;
            auto users_valid = std::views::keys(messages.second);
            std::vector<dpp::snowflake> set_tmp1 = this->get_players();
            std::vector<dpp::snowflake> set_tmp2 = {users_valid.begin(), users_valid.end()};
            std::sort(set_tmp1.begin(), set_tmp1.end());
            std::sort(set_tmp2.begin(), set_tmp2.end());
            std::ranges::set_difference(set_tmp1, set_tmp2, std::back_inserter(missing_players));

            std::string players_text;
            if (!missing_players.empty()) {
                players_text += dpp::utility::user_mention(missing_players[0]);
                for (size_t i = 1; i < missing_players.size(); i++) {
                    players_text += ", " + dpp::utility::user_mention(missing_players[i]);
                }
            }

            dpp::message m;
            dpp::embed embed;
            embed.set_color(dpp::colors::red)
                .set_title("ERROR sending private message!")
                .set_description(
                    std::format("User(s) {} should allow bot to send private messages to them!", players_text));
            m.add_embed(embed);
            m.channel_id = event.command.channel_id;
            m.id = event.command.message_id;
            _data.bot->message_edit(m);
            for (auto &[id, message]: messages.second) {
                message.embeds.clear();
                message.add_embed(embed);
                message.content.clear();
                _data.bot->message_edit(message);
            }

        } else {
            _messages = messages.second;
            game_start(event.command.channel_id, event.command.guild_id);
            _game_start_time =
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count();
            std::array<dpp::task<void>, 2> ships_placement = {player_place_ships(get_players()[0]),
                                                              player_place_ships(get_players()[1])};

            for (auto &i: ships_placement) {
                co_await i;
            }


            game_stop();
        }
        co_return;
    }
    dpp::task<void> Discord_battleships_game::player_place_ships(const dpp::snowflake &player) {
        std::unique_lock lk(_mutex);
        auto &p = _engine.get_player_by_ind(_user_to_player_id[player]);
        auto ships = p.get_unplaced_ships();
        auto &m = _messages[player];
        m.embeds[0]
            .set_title("Preparing state of battleships game")
            .set_description(std::format("State: {}\nTimeout: <t:{}:R>",
                                         "Select ship to place/edit or submit your field.",
                                         _game_start_time + _place_timeout));

        m.components.clear();
        int cnt = 0;
        dpp::component row;
        row.set_type(dpp::cot_action_row);

        // for unplaced ships
        for (auto &i: ships) {
            row.add_component(dpp::component()
                                  .set_type(dpp::cot_button)
                                  .set_label("Place: " + battleships_engine::ship_type_to_string[i->get_type()])
                                  .set_id(battleships_engine::ship_type_to_string[i->get_type()]));
            cnt++;
            if (cnt % 5 == 0) {
                m.components.push_back(row);
                row = dpp::component();
            }
        }
        if (row.components.size()) {
            m.components.push_back(row);
        }
        // for placed ships


        cnt = 0;
        row = dpp::component();
        row.set_type(dpp::cot_action_row);

        ships = p.get_placed_ships();

        // for unplaced ships
        for (auto &i: ships) {
            row.add_component(dpp::component()
                                  .set_type(dpp::cot_button)
                                  .set_label("Edit: " + battleships_engine::ship_type_to_string[i->get_type()])
                                  .set_id(battleships_engine::ship_type_to_string[i->get_type()])
                                  .set_style(dpp::cos_secondary));
            cnt++;
            if (cnt % 5 == 0) {
                m.components.push_back(row);
                row = dpp::component();
            }
        }
        if (row.components.size()) {
            m.components.push_back(row);
        }
        row = dpp::component();

        // row for controls
        row.add_component(dpp::component()
                              .set_type(dpp::cot_button)
                              .set_label("Random shuffle")
                              .set_id("shuffle")
                              .set_style(dpp::cos_secondary)
                              .set_emoji("🔀"))
            .add_component(dpp::component()
                               .set_type(dpp::cot_button)
                               .set_id("ready")
                               .set_label("Ready")
                               .set_emoji("✅")
                               .set_style(dpp::cos_success)
                               .set_disabled(!p.can_be_ready()));
        m.add_component(row);
        Image_ptr img = generate_view_field();


        co_return;
    }
} // namespace gb
