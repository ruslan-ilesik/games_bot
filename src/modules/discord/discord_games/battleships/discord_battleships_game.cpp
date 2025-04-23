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
            int line_width = 2;
            double sector_size = ((field_size - line_width * 11.0) / 10.0);

            // horizontal lines
            for (int i = 0; i < 11; i++) {
                img->draw_line({0, static_cast<int>(i * sector_size + i * line_width)},
                               {static_cast<int>(10.0 * sector_size + 10.0 * line_width),
                                static_cast<int>(i * sector_size + i * line_width)},
                               _line_color, line_width);
            }

            // vertical
            for (int i = 0; i < 11; i++) {
                img->draw_line({static_cast<int>(i * sector_size + i * line_width), 0},
                               {static_cast<int>(i * sector_size + i * line_width),
                                static_cast<int>(10.0 * sector_size + 10.0 * line_width)},
                               _line_color, line_width);
            }
            return img;
        };
        return {{"battleships_grid", grid_generator}};
    }

    Image_ptr Discord_battleships_game::generate_view_field(int state) {
        Image_ptr img = _data.image_processing->create_image({_field_size, _field_size}, _default_background);
        auto tmp = _data.image_processing->cache_get(
            "battleships_grid", {static_cast<int>(_field_size - ((_field_size - _line_width * 11.0) / 10.0)),
                                 static_cast<int>(_field_size - ((_field_size - _line_width * 11.0) / 10.0))});
        img->overlay_image(tmp, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)});
        std::array<int, 2> position = {static_cast<int>(_sector_size),
                                       static_cast<int>(_sector_size - _line_width * 2)};

        for (int i = 0; i < 10; i++) {
            std::string text = std::string(1, static_cast<char>(65 + i));
            Vector2i t_size = img->get_text_size(text, _text_scale, _text_thickness);
            int offset = abs(static_cast<int>(_sector_size) - t_size.x) / 2;
            img->draw_text(
                text, {static_cast<int>(position[0] + i * _sector_size + i * _line_width * 1.25 + offset), position[1]},
                _text_scale, state == 1 ? _text_active : _text_standard, _text_thickness);
        }
        position = {0, static_cast<int>(_sector_size * 1.75)};
        for (int i = 0; i < 10; i++) {
            img->draw_text(std::to_string(i + 1),
                           {static_cast<int>(position[0] + (i == 9 ? 0 : _sector_size / 4)),
                            static_cast<int>(position[1] + i * _sector_size + i * _line_width * 1.25)},
                           _text_scale, state == 2 ? _text_active : _text_standard, _text_thickness);
        }
        return img;
    }
    void Discord_battleships_game::draw_private_field(Image_ptr &image, const std::array<int, 2> &position,
                                                      const battleships_engine::Field &field,
                                                      const battleships_engine::Ships_container &ships,
                                                      battleships_engine::Ship *to_not_draw) {
        for (auto &ship: ships) {
            if (!ship->is_dead() && to_not_draw != ship.get()) {
                draw_ship(image, position, ship.get());
            }
        }
        this->draw_public_field(image, position, field, ships);
    }

    void Discord_battleships_game::draw_ship(Image_ptr &image, const std::array<int, 2> &position,
                                             battleships_engine::Ship *ship) {
        // we do not draw not placed ships.
        if (!ship->is_placed()) {
            return;
        }

        const auto draw_cell = [&](const Vector2i &local_pos) {
            if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 9 || local_pos.y > 9) {
                return;
            }
            Vector2i p1 = {static_cast<int>(position[0] + local_pos.x * _sector_size + local_pos.x * _line_width),
                           static_cast<int>(position[1] + local_pos.y * _sector_size + local_pos.y * _line_width)};
            image->draw_rectangle(p1, {static_cast<int>(p1.x + _sector_size), static_cast<int>(p1.y + _sector_size)},
                                  _occupied_cell_color, -1);
        };

        Vector2i direction = ship->is_rotated() ? Vector2i{1, 0} : Vector2i{0, 1};
        Vector2i p1 = {static_cast<int>(position[0] + ship->get_position()[0] * _sector_size +
                                        ship->get_position()[0] * _line_width),
                       static_cast<int>(position[1] + ship->get_position()[1] * _sector_size +
                                        ship->get_position()[1] * _line_width)};
        image->draw_rectangle(
            p1,
            {p1.x +
                 static_cast<int>(_sector_size +
                                  direction.x * (battleships_engine::ship_sizes[ship->get_type()] - 1) * _sector_size +
                                  direction.x * (battleships_engine::ship_sizes[ship->get_type()] - 1) * _line_width),
             p1.y +
                 static_cast<int>(_sector_size +
                                  direction.y * (battleships_engine::ship_sizes[ship->get_type()] - 1) * _sector_size +
                                  direction.y * (battleships_engine::ship_sizes[ship->get_type()] - 1) * _line_width)},
            _ship_colors.at(ship->get_type()), -1

        );

        auto cells = ship->get_cells();
        // define offsets for middle cells
        std::array<Vector2i, 2> offsets;
        if (ship->is_rotated()) {
            offsets[0] = {0, -1};
            offsets[1] = {0, 1};
        } else {
            offsets[0] = {-1, 0};
            offsets[1] = {1, 0};
        }
        // middle cells have very simple algorithm to draw
        Vector2i temp_pos;
        for (int i = 1; i < static_cast<int>(cells.size() - 1); i++) {
            temp_pos = {cells[i]._x, cells[i]._y};
            for (auto &offset: offsets) {
                draw_cell(temp_pos + offset);
            }
        }

        // first cells require much more work
        temp_pos = {cells[0]._x, cells[0]._y};
        draw_cell(temp_pos + Vector2i(-1, -1));
        draw_cell(temp_pos + Vector2i(-1, 0));
        draw_cell(temp_pos + Vector2i(0, -1));
        if (ship->is_rotated()) {
            draw_cell(temp_pos + Vector2i(0, 1));
            draw_cell(temp_pos + Vector2i(-1, 1));
        } else {
            draw_cell(temp_pos + Vector2i(1, -1));
            draw_cell(temp_pos + Vector2i(1, 0));
        }

        // last cells require much more work
        temp_pos = {cells[cells.size() - 1]._x, cells[cells.size() - 1]._y};
        draw_cell(temp_pos + Vector2i(0, 1));
        draw_cell(temp_pos + Vector2i(1, 0));
        draw_cell(temp_pos + Vector2i(1, 1));
        if (ship->is_rotated()) {
            draw_cell(temp_pos + Vector2i(0, -1));
            draw_cell(temp_pos + Vector2i(1, -1));
        } else {
            draw_cell(temp_pos + Vector2i(-1, 0));
            draw_cell(temp_pos + Vector2i(-1, 1));
        }
    }

    void Discord_battleships_game::draw_public_field(Image_ptr &image, const std::array<int, 2> &position,
                                                     const battleships_engine::Field &field,
                                                     const battleships_engine::Ships_container &ships) {
        for (auto &ship: ships) {
            if (ship->is_dead()) {
                draw_ship(image, position, ship.get());
            }
        }
        for (int i = 0; i < static_cast<int>(field.size()); i++) {
            for (int j = 0; j < static_cast<int>(field[i].size()); j++) {
                switch (field[i][j]) {
                    case battleships_engine::Cell_states::OCCUPIED:
                        break;
                    case battleships_engine::Cell_states::EMPTY:
                        break;
                    case battleships_engine::Cell_states::MISSED:
                        image->draw_circle(
                            {static_cast<int>(position[0] + i * _sector_size + i * _line_width + _sector_size / 2),
                             static_cast<int>(position[1] + j * _sector_size + j * _line_width + _sector_size / 2)},
                            _sector_size / 4, _miss_cell_color, -1);
                        break;
                    case battleships_engine::Cell_states::DAMAGED:
                        auto s_p = Vector2i(
                            static_cast<int>(position[0] + i * _sector_size + i * _line_width + _sector_size / 8),
                            static_cast<int>(position[1] + j * _sector_size + j * _line_width + _sector_size / 8));

                        image->draw_line(
                            s_p, s_p + Vector2i(_sector_size - _sector_size / 4, _sector_size - _sector_size / 4),
                            _damaged_cell_color, _sector_size / 4);
                        image->draw_line(s_p + Vector2i(0, _sector_size - _sector_size / 4),
                                         s_p + Vector2i(_sector_size - _sector_size / 4, 0), _damaged_cell_color,
                                         _sector_size / 4);
                        break;
                }
            }
        }
    }

    dpp::task<void> Discord_battleships_game::run(dpp::button_click_t event) {
        _data.bot->reply(event, dpp::message().add_embed(
                                    dpp::embed().set_color(dpp::colors::blue).set_title("Game is starting...")));
        _message.add_embed(dpp::embed());
        _message.guild_id = event.command.guild_id;
        _message.channel_id = event.command.channel_id;
        _message.id = event.command.message_id;
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

            std::string players_text;
            if (!get_players().empty()) {
                players_text += dpp::utility::user_mention(get_players()[0]);
                for (size_t i = 1; i < get_players().size(); i++) {
                    players_text += ", " + dpp::utility::user_mention(get_players()[i]);
                }
            }
            dpp::message main_message;
            main_message.add_embed(dpp::embed()
                                       .set_color(dpp::colors::blue)
                                       .set_title("Battleships game is in progress.")
                                       .set_description(std::format(
                                           "Players: {} should check their private messages with bot.", players_text)));
            main_message.channel_id = event.command.channel_id;
            main_message.id = event.command.message_id;
            _data.bot->message_edit(main_message);
            for (auto &i: ships_placement) {
                co_await i;
            }
            if (_is_timeout) {
                end_of_game_timeout(_is_timeout == 2);
            } else {
                _engine.start_game();
                _move_start = std::chrono::duration_cast<std::chrono::seconds>(
                                  std::chrono::system_clock::now().time_since_epoch())
                                  .count();
                dpp::task<Button_click_return> button_click_awaiter;
                Button_click_return r;
                event.custom_id = "FIRST_TURN";
                _states = {0, 0};
                while (1) {
                    auto back_row = dpp::component().set_type(dpp::cot_action_row);
                    back_row.add_component(dpp::component()
                                               .set_type(dpp::cot_button)
                                               .set_id("back")
                                               .set_label("Back")
                                               .set_emoji("🔙")
                                               .set_style(dpp::cos_danger));

                    _message.components.clear();
                    _message.embeds[0]
                        .set_title("Battleships game")
                        .set_description(std::format(
                            "Turn: {}\nTimeout: <t:{}:R>\nState: {}", dpp::utility::user_mention(get_current_player()),
                            _move_start + 60ll,
                            (_states[_user_to_player_id[get_current_player()]] == 0
                                 ? "Select column"
                                 : (_states[_user_to_player_id[get_current_player()]] == 1 ? "Select row"
                                                                                           : "Submit action"))));
                    dpp::component row = dpp::component().set_type(dpp::cot_action_row);

                    if (_states[_user_to_player_id[get_current_player()]] == 0) {
                        for (int i: _engine.get_free_cols_for_attack()) {
                            if (row.components.size() == 5) {
                                _message.add_component(row);
                                row = dpp::component().set_type(dpp::cot_action_row);
                            }
                            row.add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_id(std::to_string(i))
                                                  .set_label(std::string(1, static_cast<char>(65 + i))));
                        }
                        if (!row.components.empty()) {
                            _message.add_component(row);
                        }
                    }

                    else if (_states[_user_to_player_id[get_current_player()]] == 1) {
                        for (int i:
                             _engine.get_free_rows_for_attack(_temp_pos[_user_to_player_id[get_current_player()]][0])) {
                            if (row.components.size() == 5) {
                                _message.add_component(row);
                                row = dpp::component().set_type(dpp::cot_action_row);
                            }
                            row.add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_id(std::to_string(i))
                                                  .set_label(std::to_string(i + 1)));
                        }
                        if (!row.components.empty()) {
                            _message.add_component(row);
                        }
                        _message.add_component(back_row);
                    }

                    else {
                        back_row.add_component(dpp::component()
                                                   .set_type(dpp::cot_button)
                                                   .set_label("Attack")
                                                   .set_id("attack")
                                                   .set_style(dpp::cos_secondary)
                                                   .set_emoji("🎯"));
                        _message.add_component(back_row);
                    }

                    _img_cnt++;
                    Image_ptr img = _data.image_processing->create_image(_image_size, _default_background);
                    auto &player = _engine.get_player_by_ind(_user_to_player_id[get_current_player()]);
                    auto &player2 = _engine.get_player_by_ind(_user_to_player_id[get_current_player()] + 1 > 1 ? 0 : 1);
                    auto view_field = generate_view_field();
                    img->overlay_image(view_field, {0, 0});
                    view_field = generate_view_field(_states[_user_to_player_id[get_current_player()]] + 1);
                    img->overlay_image(view_field, {_field_size + _distance_between_fields, 0});

                    draw_public_field(img, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)},
                                      player.get_field(), player.get_ships());
                    if (_states[_user_to_player_id[get_current_player()]] == 1) {
                        // add column draw on  field
                        Vector2i position_start = {
                            static_cast<int>(_field_size + _distance_between_fields + _sector_size +
                                             _sector_size * _temp_pos[_user_to_player_id[get_current_player()]][0] +
                                             _line_width * _temp_pos[_user_to_player_id[get_current_player()]][0]),
                            static_cast<int>(_sector_size)};
                        img->draw_rectangle(position_start,
                                            position_start +
                                                Vector2i{static_cast<int>(_sector_size),
                                                         static_cast<int>(_sector_size * 10 + _line_width * 10)},
                                            _selected_col_color, -1);
                    } else if (_states[_user_to_player_id[get_current_player()]] == 2) {
                        Vector2i position_start = {
                            static_cast<int>(_field_size + _distance_between_fields + _sector_size +
                                             _sector_size * _temp_pos[_user_to_player_id[get_current_player()]][0] +
                                             _line_width * _temp_pos[_user_to_player_id[get_current_player()]][0]),
                            static_cast<int>(_sector_size +
                                             _sector_size * _temp_pos[_user_to_player_id[get_current_player()]][1] +
                                             _line_width * _temp_pos[_user_to_player_id[get_current_player()]][1])};
                        img->draw_rectangle(
                            position_start,
                            position_start + Vector2i{static_cast<int>(_sector_size), static_cast<int>(_sector_size)},
                            _selected_col_color, -1);
                    }
                    draw_public_field(img,
                                      {static_cast<int>(_field_size + _distance_between_fields + _sector_size),
                                       static_cast<int>(_sector_size)},
                                      player2.get_field(), player2.get_ships());


                    _message.embeds[0].set_image(add_image(_message, img));

                    button_click_awaiter = _data.button_click_handler->wait_for(
                        _message, {get_current_player()},
                       60);
                    if (event.custom_id == "FIRST_TURN") {
                        _data.bot->message_edit(_message);
                    } else {
                        _data.bot->event_edit_original_response(event, _message);
                    }
                    r = co_await button_click_awaiter;

                    if (r.second) {
                        // timeout
                        end_of_game_timeout();
                        break;
                    }
                    event = r.first;
                    event.reply(dpp::ir_update_message,"loading");
                    if (event.custom_id == "back") {
                        _states[_user_to_player_id[get_current_player()]]--;
                        continue;
                    }

                    if (_states[_user_to_player_id[get_current_player()]] == 0) {
                        _temp_pos[_user_to_player_id[get_current_player()]][0] = std::stoi(event.custom_id);
                    } else if (_states[_user_to_player_id[get_current_player()]] == 1) {
                        _temp_pos[_user_to_player_id[get_current_player()]][1] = std::stoi(event.custom_id);
                    } else {
                        if (!_engine.attack({_temp_pos[_user_to_player_id[get_current_player()]]})) {
                            next_player();
                        } else {
                            auto ind = _user_to_player_id[get_current_player()] == 1 ? 0 : 1;
                            auto &ships = _engine.get_player_by_ind(ind).get_ships();
                            for (const auto &i: ships) {
                                if (i->get_type() == battleships_engine::Ship_types::Carrier && i->is_dead()) {
                                    _data.achievements_processing->activate_achievement(
                                        "44.92002°, 31.49265°", get_current_player(), event.command.channel_id);
                                }
                            }
                        }
                        int winner = _engine.get_winner();
                        if (winner != -1) {
                            // win
                            auto winner_id = get_current_player();
                            next_player();
                            _message.components.clear();
                            _message.embeds[0]
                                .set_title("Game over")
                                .set_description(std::format(
                                    "Player {} won a game.\nPlayer {} will be luckier next time.",
                                    dpp::utility::user_mention(winner_id),
                                    dpp::utility::user_mention(winner == 1 ? get_players()[0] : get_players()[1])))
                                .set_color(dpp::colors::blue);

                            _img_cnt++;
                            Image_ptr img = _data.image_processing->create_image(_image_size, _default_background);
                            auto &player = _engine.get_player_by_ind(_user_to_player_id[get_current_player()]);
                            auto &player2 =
                                _engine.get_player_by_ind(_user_to_player_id[get_current_player()] + 1 > 1 ? 0 : 1);
                            auto view_field = generate_view_field();
                            img->overlay_image(view_field, {0, 0});
                            view_field = generate_view_field(_states[_user_to_player_id[get_current_player()]] + 1);
                            img->overlay_image(view_field, {_field_size + _distance_between_fields, 0});
                            draw_private_field(img, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)},
                                               player.get_field(), player.get_ships());
                            draw_private_field(img,
                                               {static_cast<int>(_field_size + _distance_between_fields + _sector_size),
                                                static_cast<int>(_sector_size)},
                                               player2.get_field(), player2.get_ships());
                            _message.embeds[0].set_image(add_image(_message, img));
                            _data.bot->event_edit_original_response(event, _message);

                            remove_player(USER_REMOVE_REASON::WIN, winner_id);
                            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                            break;
                        }
                        _move_start = std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::system_clock::now().time_since_epoch())
                                          .count();
                        _states[_user_to_player_id[get_current_player()]] = -1;
                    }
                    _states[_user_to_player_id[get_current_player()]]++;
                }
            }
            game_stop();
        }
        co_return;
    }

    void Discord_battleships_game::end_of_game_timeout(bool is_both_timeout) {
        dpp::snowflake loser_id = get_current_player();
        next_player();
        _message.components.clear();
        if (is_both_timeout) {
            _message.embeds[0]
                .set_title("Game over")
                .set_description(std::format("If you still want to play, you can create new game.\nPlayers {} and {} "
                                             "got timeout, both of them lost the game .",
                                             dpp::utility::user_mention(get_current_player()),
                                             dpp::utility::user_mention(loser_id)))
                .set_color(dpp::colors::red);
            _data.bot->message_edit(_message);
            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
        } else {
            _message.embeds[0]
                .set_title("Game over")
                .set_description(std::format("If you still want to play, you can create new game.\nPlayer {} won the "
                                             "game.\nPlayer {} should think faster next time.",
                                             dpp::utility::user_mention(get_current_player()),
                                             dpp::utility::user_mention(loser_id)))
                .set_color(dpp::colors::blue);
            _data.bot->message_edit(_message);
            remove_player(USER_REMOVE_REASON::WIN, get_current_player());
            remove_player(USER_REMOVE_REASON::LOSE, loser_id);
        }
    }

    dpp::task<void> Discord_battleships_game::player_place_ships(const dpp::snowflake player) {
        std::unique_lock lk(_mutex);
        auto &p = _engine.get_player_by_ind(_user_to_player_id[player]);
        auto &m = _messages[player];
        Button_click_return r;
        dpp::task<Button_click_return> button_click_awaiter;
        auto prepare_message = [&, player]() -> dpp::task<void> {
            m.embeds[0]
                .set_title("Preparing state of battleships game")
                .set_description(std::format("State: {}\nTimeout: <t:{}:R>",
                                             "Select ship to place/edit or submit your field.",
                                             _game_start_time + _place_timeout));

            m.components.clear();
            int cnt = 0;
            dpp::component row;
            row.set_type(dpp::cot_action_row);
            auto ships = p.get_unplaced_ships();
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
            _img_cnt++;
            draw_private_field(img, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)}, p.get_field(),
                               p.get_ships());
            m.embeds[0].set_image(add_image(m, img));
            button_click_awaiter = _data.button_click_handler->wait_for(
                m, {player},
                _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                      std::chrono::system_clock::now().time_since_epoch())
                                      .count() -
                                  _game_start_time));
            co_return;
        };
        co_await prepare_message();
        dpp::button_click_t event;
        _data.bot->message_edit(m);
        lk.unlock();
        r = co_await button_click_awaiter;
        lk.lock();
        while (1) {
            if (r.second) {
                _is_timeout++;
                m.components.clear();
                m.embeds[0]
                    .set_title("Game has been ended!")
                    .set_color(dpp::colors::green)
                    .set_description(std::format(
                        "Reason: Game timeout\nGo here to see result: https://discord.com/channels/{}/{}/{} \n",
                        _message.guild_id, _message.channel_id, _message.id));
                _data.bot->message_edit(m);
                if (get_players()[0] == player) {
                    set_current_player_index(0);
                } else {
                    set_current_player_index(1);
                }
                break;
            }
            event = r.first;
            event.reply(dpp::ir_update_message,"loading");
            std::string game_state = "";
            auto back_row = dpp::component().set_type(dpp::cot_action_row);
            back_row.add_component(dpp::component()
                                       .set_type(dpp::cot_button)
                                       .set_id("back")
                                       .set_label("Back")
                                       .set_emoji("🔙")
                                       .set_style(dpp::cos_danger));

            m.components.clear();
            if (event.custom_id == "ready") {
                game_state = "Are you sure that you ready to play? After this action you will not be able to change "
                             "layout of your ships!";
                back_row.add_component(dpp::component()
                                           .set_type(dpp::cot_button)
                                           .set_style(dpp::cos_secondary)
                                           .set_emoji("✅")
                                           .set_label("Ready")
                                           .set_id("ready_game")
                                           .set_style(dpp::cos_success));
                m.add_component(back_row);
                _states[_user_to_player_id[player]] = -1;
                button_click_awaiter = _data.button_click_handler->wait_for(
                    m, {player},
                    _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::system_clock::now().time_since_epoch())
                                          .count() -
                                      _game_start_time));
                _data.bot->event_edit_original_response(event, m);
                lk.unlock();
                r = co_await button_click_awaiter;
                lk.lock();
                continue;
            } else if (event.custom_id == "ready_game") {
                p.make_ready();
                m.embeds[0]
                    .set_title("Waiting for opponent!")
                    .set_description(std::format("Game will be continued here: https://discord.com/channels/{}/{}/{} "
                                                 "\n Time left for opponent to press ready: <t:{}:R>",
                                                 _message.guild_id, _message.channel_id, _message.id,
                                                 _game_start_time + _place_timeout));
                _data.bot->event_edit_original_response(event, m);
                break;
            } else if (event.custom_id == "shuffle") {
                if (p.get_placed_ships().empty()) {
                    p.random_place_ships();
                    _states[_user_to_player_id[player]] = 0;
                    co_await prepare_message();
                    _data.bot->event_edit_original_response(event, m);
                    lk.unlock();
                    r = co_await button_click_awaiter;
                    lk.lock();
                    continue;
                } else {
                    game_state = "You have already placed ships. Are you sure you want to shuffle them?";
                    back_row.add_component(dpp::component()
                                               .set_type(dpp::cot_button)
                                               .set_style(dpp::cos_secondary)
                                               .set_emoji("🔀")
                                               .set_label("Shuffle")
                                               .set_id("agree_shuffle")
                                               .set_style(dpp::cos_success));
                    m.add_component(back_row);
                    _states[_user_to_player_id[player]] = -1;
                    button_click_awaiter = _data.button_click_handler->wait_for(
                        m, {player},
                        _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                              std::chrono::system_clock::now().time_since_epoch())
                                              .count() -
                                          _game_start_time));
                    _data.bot->event_edit_original_response(event, m);
                    lk.unlock();
                    r = co_await button_click_awaiter;
                    lk.lock();
                    continue;
                }
            } else if (event.custom_id == "agree_shuffle") {
                p.random_place_ships();
                _states[_user_to_player_id[player]] = 0;
                co_await prepare_message();
                _data.bot->event_edit_original_response(event, m);
                lk.unlock();
                r = co_await button_click_awaiter;
                lk.lock();
                continue;
            } else if (event.custom_id == "unplace") {
                _temp_ships[_user_to_player_id[player]]->unplace(p.get_field());
                _states[_user_to_player_id[player]] = 0;
                co_await prepare_message();
                _data.bot->event_edit_original_response(event, m);
                lk.unlock();
                r = co_await button_click_awaiter;
                lk.lock();
                continue;
            }
            if (event.custom_id == "back") {
                if (_states[_user_to_player_id[player]] == 3) {
                    _temp_ships[_user_to_player_id[player]]->unplace(p.get_field());
                    for (auto i: p.get_placed_ships()) {
                        i->place_same_position(p.get_field());
                    }
                }
                _states[_user_to_player_id[player]]--;
                if (_states[_user_to_player_id[player]] <= 0) {
                    _states[_user_to_player_id[player]] = 0;
                    co_await prepare_message();
                    _data.bot->event_edit_original_response(event, m);
                    lk.unlock();
                    r = co_await button_click_awaiter;
                    lk.lock();
                    continue;
                }
                _last_was_back[_user_to_player_id[player]] = true;
            } else {
                _states[_user_to_player_id[player]]++;
                _last_was_back[_user_to_player_id[player]] = false;
            }

            Image_ptr image = generate_view_field();
            battleships_engine::Ship *ship_to_ignore = NULL;
            if (event.custom_id == "rotate") {
                _temp_ships[_user_to_player_id[player]]->rotate(p.get_ships(), p.get_field());
                _states[_user_to_player_id[player]] = 3;
                ship_to_ignore = _temp_ships[_user_to_player_id[player]];
            }
            draw_private_field(image, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)}, p.get_field(),
                               p.get_ships(), ship_to_ignore);


            if (_states[_user_to_player_id[player]] == 1) {
                if (event.custom_id != "back") {
                    for (auto &i: p.get_ships()) {
                        if (event.custom_id == (*i)) {
                            _temp_ships[_user_to_player_id[player]] = i.get();
                            break;
                        }
                    }
                }
                game_state = "Choose column to place ship at or unplace ship if possible";
                auto free_cols = p.get_free_columns(_temp_ships[_user_to_player_id[player]]);
                auto row = dpp::component().set_type(dpp::cot_action_row);
                for (auto i: free_cols) {
                    if (row.components.size() == 5) {
                        m.add_component(row);
                        row = dpp::component().set_type(dpp::cot_action_row);
                    }
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_id(std::to_string(i))
                                          .set_label(std::string(1, static_cast<char>(65 + i))));
                }
                // add unplace btn if editing ship;
                if (_temp_ships[_user_to_player_id[player]]->is_placed()) {
                    back_row.add_component(dpp::component()
                                               .set_type(dpp::cot_button)
                                               .set_style(dpp::cos_secondary)
                                               .set_id("unplace")
                                               .set_label("unplace")
                                               .set_emoji("🧨"));
                }
                m.add_component(row).add_component(back_row);

                button_click_awaiter = _data.button_click_handler->wait_for(
                    m, {player},
                    _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::system_clock::now().time_since_epoch())
                                          .count() -
                                      _game_start_time));
                _data.bot->event_edit_original_response(event, m);
                lk.unlock();
                r = co_await button_click_awaiter;
                lk.lock();
                continue;

            } else if (_states[_user_to_player_id[player]] == 2) {
                if (event.custom_id != "back") {
                    _temp_pos[_user_to_player_id[player]][0] = std::stoi(event.custom_id);
                }
                game_state = "Choose row to place ship at.";
                auto free_pos = p.get_free_positions_in_col(_temp_ships[_user_to_player_id[player]],
                                                            _temp_pos[_user_to_player_id[player]][0]);
                auto row = dpp::component().set_type(dpp::cot_action_row);
                for (auto i: free_pos) {
                    if (row.components.size() == 5) {
                        m.add_component(row);
                        row = dpp::component().set_type(dpp::cot_action_row);
                    }
                    row.add_component(dpp::component()
                                          .set_type(dpp::cot_button)
                                          .set_id(std::to_string(i))
                                          .set_label(std::to_string(i + 1)));
                }
                m.add_component(row).add_component(back_row);

                button_click_awaiter = _data.button_click_handler->wait_for(
                    m, {player},
                    _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::system_clock::now().time_since_epoch())
                                          .count() -
                                      _game_start_time));

                Vector2i position_start = {static_cast<int>(_sector_size +
                                                            _sector_size * _temp_pos[_user_to_player_id[player]][0] +
                                                            _line_width * _temp_pos[_user_to_player_id[player]][0]),
                                           static_cast<int>(_sector_size)};

                image->draw_rectangle(position_start,
                                      position_start + Vector2i{static_cast<int>(_sector_size),
                                                                static_cast<int>(_sector_size * 10 + _line_width * 10)},
                                      _selected_col_color, -1);
            } else if (_states[_user_to_player_id[player]] == 3) {
                if (event.custom_id != "back" && event.custom_id != "rotate") {
                    _temp_pos[_user_to_player_id[player]][1] = std::stoi(event.custom_id);
                }
                game_state = "Choose action to do.";
                battleships_engine::Cell t_s{_temp_pos[_user_to_player_id[player]][0],
                                             _temp_pos[_user_to_player_id[player]][1]};
                if (_temp_ships[_user_to_player_id[player]]->can_be_placed(p.get_ships(), t_s)) {
                    // draw
                    _temp_ships[_user_to_player_id[player]]->place(p.get_ships(), p.get_field(), t_s);
                    draw_ship(image, {static_cast<int>(_sector_size), static_cast<int>(_sector_size)},
                              _temp_ships[_user_to_player_id[player]]);
                }
                back_row
                    .add_component(
                        dpp::component()
                            .set_type(dpp::cot_button)
                            .set_label("Rotate")
                            .set_id("rotate")
                            .set_emoji("\xF0\x9F\x94\x84")
                            .set_disabled(!_temp_ships[_user_to_player_id[player]]->is_rotatable(p.get_ships())))
                    .add_component(dpp::component()
                                       .set_type(dpp::cot_button)
                                       .set_label("Place")
                                       .set_id("place")
                                       .set_emoji("✅")
                                       .set_style(dpp::cos_success)
                                       .set_disabled(!_temp_ships[_user_to_player_id[player]]->can_be_placed(
                                           p.get_ships(), {_temp_pos[_user_to_player_id[player]]})));
                m.add_component(back_row);
                button_click_awaiter = _data.button_click_handler->wait_for(
                    m, {player},
                    _place_timeout - (std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::system_clock::now().time_since_epoch())
                                          .count() -
                                      _game_start_time));
            } else if (_states[_user_to_player_id[player]] == 4) {
                _temp_ships[_user_to_player_id[player]]->place(p.get_ships(), p.get_field(),
                                                               {_temp_pos[_user_to_player_id[player]]});
                _states[_user_to_player_id[player]] = 0;
                co_await prepare_message();
                _data.bot->event_edit_original_response(event, m);
                lk.unlock();
                r = co_await button_click_awaiter;
                lk.lock();
                continue;
            }
            m.embeds[0]
                .set_title("Preparing state of battleships game")
                .set_description(
                    std::format("State: {}\nTimeout: <t:{}:R>", game_state, _game_start_time + _place_timeout));
            m.embeds[0].set_image(add_image(m, image));
            _data.bot->event_edit_original_response(event, m);
            _img_cnt++;
            lk.unlock();
            r = co_await button_click_awaiter;
            lk.lock();
            continue;
        }

        co_return;
    }
} // namespace gb
