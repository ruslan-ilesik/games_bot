//
// Created by ilesik on 9/5/24.
//

#include "discord_dominoes_game.hpp"

namespace gb {
    Discord_dominoes_game::Discord_dominoes_game(Game_data_initialization &_data,
                                                 const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}


    std::vector<std::pair<std::string, image_generator_t>> Discord_dominoes_game::get_image_generators() {
        std::vector<std::pair<std::string, image_generator_t>> r;
        for (const dominoes::piece &piece: dominoes::list_of_domino) {
            r.emplace_back(std::format("dominoes_piece_{}_{}", piece[0], piece[1]),
                           [=](Image_processing_ptr image_processing, const Vector2i &resolution) {
                               if (resolution.x * 2 != resolution.y) {
                                   throw std::runtime_error("Resolution of dominoes should be strictly 1/2 (x/y)");
                               }
                               int line_width = std::max(resolution.x / 20, 1);
                               int dots_res = resolution.x - line_width * 2;
                               Image_ptr img = image_processing->create_image(resolution, {255, 255, 255});
                               img->draw_line({0, resolution.x - line_width / 2},
                                              {resolution.x, resolution.x - line_width / 2}, {0, 0, 0}, line_width);
                               draw_dots(img, {line_width, line_width / 2}, dots_res, piece[0]);
                               draw_dots(img, {line_width, resolution.y / 2 + line_width}, dots_res, piece[1]);
                               return img;
                           });
        }
        return r;
    }

    void Discord_dominoes_game::draw_dots(Image_ptr &img, const Vector2i &position, int resolution, int dot) {
        int circle_radius = resolution / 3 / 2;
        if (dot == 1 || dot == 3 || dot == 5) {
            // center x center y
            img->draw_circle(position + Vector2i(resolution / 2, resolution / 2), circle_radius, {0, 0, 0}, -1);
        }

        if (dot == 2 || dot == 3 || dot == 4 || dot == 5 || dot == 6) {
            // start x, end y;
            img->draw_circle(position + Vector2i(circle_radius, resolution - circle_radius), circle_radius, {0, 0, 0},
                             -1);
            // end x, start y;
            img->draw_circle(position + Vector2i(resolution - circle_radius, circle_radius), circle_radius, {0, 0, 0},
                             -1);
        }

        if (dot == 4 || dot == 5 || dot == 6) { // start x, start y;
            img->draw_circle(position + Vector2i(circle_radius, circle_radius), circle_radius, {0, 0, 0}, -1);
            // end x, end y;
            img->draw_circle(position + Vector2i(resolution - circle_radius, resolution - circle_radius), circle_radius,
                             {0, 0, 0}, -1);
        }

        if (dot == 6) {
            // start x, center y
            img->draw_circle(position + Vector2i(circle_radius, resolution / 2), circle_radius, {0, 0, 0}, -1);
            // end x, center y
            img->draw_circle(position + Vector2i(resolution - circle_radius, resolution / 2), circle_radius, {0, 0, 0},
                             -1);
        }
    }

    dpp::task<void> Discord_dominoes_game::run(const dpp::button_click_t &event) {
        _original_players_list = get_players();
        _data.bot->reply(event, dpp::message().add_embed(
                                    dpp::embed().set_color(dpp::colors::blue).set_title("Game is starting...")));
        Direct_messages_return messages = co_await get_private_messages(get_players());
        if (messages.first) {

            std::vector<dpp::snowflake> missing_players;
            auto users_valid = std::views::keys(messages.second);
            std::vector<dpp::snowflake> set_tmp1 = this->get_players();
            std::vector<dpp::snowflake> set_tmp2 = {users_valid.begin(),users_valid.end()};
            std::sort(set_tmp1.begin(),set_tmp1.end());
            std::sort(set_tmp2.begin(),set_tmp2.end());
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
            this->_messages = messages.second;
            game_start(event.command.channel_id, event.command.guild_id);
            this->_pieces_in_deck = (this->get_players().size() > 2 ? 5 : 7);
            generate_decks();
            make_first_turn();

            std::string players_text;
            if (!get_players().empty()) {
                players_text += dpp::utility::user_mention(get_players()[0]);
                for (size_t i = 1; i < get_players().size(); i++) {
                    players_text += ", " + dpp::utility::user_mention(get_players()[i]);
                }
            }

            _main_message.add_embed(
                dpp::embed()
                    .set_color(dpp::colors::blue)
                    .set_title("Dominoes game is in progress.")
                    .set_description(
                        std::format("Players: {} should check their private messages with bot.", players_text)));
            _main_message.channel_id = event.command.channel_id;
            _main_message.id = event.command.message_id;
            _data.bot->message_edit(_main_message);

            dpp::button_click_t click_event;
            get_possible_moves();

            dpp::embed embed;
            make_message();
            generate_image(this->_messages[get_current_player()], this->_messages[get_current_player()].embeds[0]);
            auto button_click_awaiter =
                _data.button_click_handler->wait_for(this->_messages[get_current_player()], {get_current_player()}, 60);
            _data.bot->message_edit(this->_messages[get_current_player()]);
            Button_click_return r = co_await button_click_awaiter;
            while (true) {
                if (r.second) {
                    // timeout
                    if (get_players().size() == 2) {
                        _messages[get_current_player()].embeds.clear();
                        _messages[get_current_player()].attachments.clear();
                        _messages[get_current_player()].components.clear();
                        _messages[get_current_player()].add_embed(
                            dpp::embed()
                                .set_color(dpp::colors::red)
                                .set_title("Dominoes game over!")
                                .set_description("You lost. Game timeout has been reached."));
                        _data.bot->message_edit(_messages[get_current_player()]);
                        remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                        win();
                        _messages[get_current_player()].embeds.clear();
                        _messages[get_current_player()].attachments.clear();
                        _messages[get_current_player()].components.clear();
                        _messages[get_current_player()].add_embed(
                            dpp::embed()
                                .set_color(dpp::colors::green)
                                .set_title("Dominoes game over!")
                                .set_description("Congratulations! You won the game."));
                        _data.bot->message_edit(_messages[get_current_player()]);
                        remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                        break;
                    } else {
                        _messages[get_current_player()].embeds.clear();
                        _messages[get_current_player()].attachments.clear();
                        _messages[get_current_player()].components.clear();
                        _messages[get_current_player()].add_embed(
                            dpp::embed()
                                .set_color(dpp::colors::red)
                                .set_title("Dominoes game over!")
                                .set_description("You lost. Game timeout has been reached."));
                        _data.bot->message_edit(_messages[get_current_player()]);
                        remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                        get_possible_moves();
                        make_message();
                        generate_image(this->_messages[get_current_player()],
                                       this->_messages[get_current_player()].embeds[0]);
                        button_click_awaiter = _data.button_click_handler->wait_for(
                            this->_messages[get_current_player()], {get_current_player()}, 60);
                        _data.bot->message_edit(this->_messages[get_current_player()]);
                        r = co_await button_click_awaiter;
                        continue;
                    }
                }
                click_event = r.first;
                click_event.reply(dpp::ir_update_message,"loading");
                _hidden_deck_images.erase(get_current_player());
                if (click_event.custom_id == "take") {
                    std::uniform_int_distribution<size_t> random_int{0, _local_list_of_domino.size() - 1};
                    auto id = random_int(_rd);
                    _decks[get_current_player()].push_back(_local_list_of_domino[id]);
                    _local_list_of_domino.erase(_local_list_of_domino.begin() + id);
                    if (_local_list_of_domino.empty()) {
                        for (const auto &i: get_players()) {
                            _data.achievements_processing->activate_achievement("Ran out of stack", i,
                                                                                event.command.channel_id);
                        }
                    }
                    get_possible_moves();

                    if (!_possible_moves.empty() || !_local_list_of_domino.empty()) {
                        make_message();
                        generate_image(this->_messages[get_current_player()],
                                       this->_messages[get_current_player()].embeds[0]);
                        button_click_awaiter = _data.button_click_handler->wait_for(this->_messages[get_current_player()],
                                                                                    {get_current_player()}, 60);
                        _data.bot->event_edit_original_response(click_event, this->_messages[get_current_player()]);
                        r = co_await button_click_awaiter;
                        continue;
                    }
                    else {
                        auto id = get_current_player();
                        next_player();
                        bool is_continue = false;
                        while (get_current_player() != id) {
                            get_possible_moves();
                            if (!_possible_moves.empty()) {
                                make_message();
                                generate_image(this->_messages[get_current_player()],
                                               this->_messages[get_current_player()].embeds[0]);
                                button_click_awaiter = _data.button_click_handler->wait_for(
                                    this->_messages[get_current_player()], {get_current_player()}, 60);
                                _data.bot->message_edit(this->_messages[get_current_player()]);
                                r = co_await button_click_awaiter;
                                is_continue = true;
                                break;
                            }
                            next_player();
                        }
                        if (is_continue) {
                            continue;
                        }
                        end_game_custom_condition();
                        break;
                    }
                } else {
                    if (click_event.custom_id == "left") {
                        _board.insert(_board.begin(), _to_place);
                        _decks[get_current_player()].erase(std::remove(_decks[get_current_player()].begin(),
                                                                       _decks[get_current_player()].end(), _to_place),
                                                           _decks[get_current_player()].end());
                    } else if (click_event.custom_id == "right") {
                        _board.push_back(_to_place);
                        _decks[get_current_player()].erase(std::remove(_decks[get_current_player()].begin(),
                                                                       _decks[get_current_player()].end(), _to_place),
                                                           _decks[get_current_player()].end());
                    } else {
                        dominoes::piece piece = {std::stoi(click_event.custom_id.substr(0, 1)),
                                                 std::stoi(click_event.custom_id.substr(2, 1))};
                        _decks[get_current_player()].erase(std::remove(_decks[get_current_player()].begin(),
                                                                       _decks[get_current_player()].end(), piece),
                                                           _decks[get_current_player()].end());
                        if (!(piece[0] == _board.back()[1] || piece[1] == _board[0][0])) {
                            std::reverse(std::begin(piece), std::end(piece));
                        }
                        if ((piece[0] == _board.back()[1] && piece[1] == _board[0][0])) {
                            _messages[get_current_player()].components.clear();
                            _messages[get_current_player()].add_component(
                                dpp::component()
                                    .set_type(dpp::cot_action_row)
                                    .add_component(dpp::component()
                                                       .set_type(dpp::cot_button)
                                                       .set_label("place at left side")
                                                       .set_id("left")
                                                       .set_emoji("⬅️"))

                                    .add_component(dpp::component()
                                                       .set_type(dpp::cot_button)
                                                       .set_label("place at right side")
                                                       .set_id("right")
                                                       .set_emoji("➡️")));
                            _to_place = piece;
                            button_click_awaiter = _data.button_click_handler->wait_for(_messages[get_current_player()],
                                                                                        {get_current_player()}, 60);
                            _data.bot->event_edit_original_response(click_event, _messages[get_current_player()]);
                            r = co_await button_click_awaiter;
                            continue;
                        }
                        if (piece[1] == _board[0][0]) {
                            _board.insert(_board.begin(), piece);
                        } else {
                            _board.push_back(piece);
                        }
                    }
                    if (_decks[get_current_player()].size() == 0) {
                        // win
                        win();
                        _messages[get_current_player()].embeds.clear();
                        _messages[get_current_player()].attachments.clear();
                        _messages[get_current_player()].components.clear();
                        _messages[get_current_player()].add_embed(
                            dpp::embed()
                                .set_color(dpp::colors::green)
                                .set_title("Dominoes game over!")
                                .set_description("Congratulations! You won the game."));
                        _data.bot->event_edit_original_response(click_event,_messages[get_current_player()]);
                        remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                        std::vector<dpp::snowflake> losers;
                        for (auto& i : get_players()) {
                            _messages[i].embeds.clear();
                            _messages[i].attachments.clear();
                            _messages[i].components.clear();
                            _messages[i].add_embed(
                                dpp::embed()
                                    .set_color(dpp::colors::red)
                                    .set_title("Dominoes game over!")
                                    .set_description("You lost! Other player placed all their dominoes."));
                            _data.bot->message_edit(_messages[i]);
                            losers.push_back(i);
                        }
                        for (auto& i : losers) {
                            remove_player(USER_REMOVE_REASON::LOSE,i);
                        }

                        break;
                    }
                    _data.bot->event_edit_original_response(
                        click_event,
                        dpp::message().add_embed(dpp::embed()
                                                     .set_color(dpp::colors::blue)
                                                     .set_title("Dominoes game!")
                                                     .set_description("Wait for other players to make their moves.")));
                    next_player();
                }
                get_possible_moves();

                if (!_possible_moves.empty() || !_local_list_of_domino.empty()) {
                    make_message();
                    generate_image(this->_messages[get_current_player()],
                                   this->_messages[get_current_player()].embeds[0]);
                    button_click_awaiter = _data.button_click_handler->wait_for(this->_messages[get_current_player()],
                                                                                {get_current_player()}, 60);
                    _data.bot->message_edit(this->_messages[get_current_player()]);
                    r = co_await button_click_awaiter;
                    continue;
                } else {
                    auto id = get_current_player();
                    next_player();
                    bool is_continue = false;
                    while (get_current_player() != id) {
                        get_possible_moves();
                        if (!_possible_moves.empty()) {
                            make_message();
                            generate_image(this->_messages[get_current_player()],
                                           this->_messages[get_current_player()].embeds[0]);
                            button_click_awaiter = _data.button_click_handler->wait_for(
                                this->_messages[get_current_player()], {get_current_player()}, 60);
                            _data.bot->message_edit(this->_messages[get_current_player()]);
                            r = co_await button_click_awaiter;
                            is_continue = true;
                            break;
                        }
                        next_player();
                    }
                    if (is_continue) {
                        continue;
                    }
                    end_game_custom_condition();
                    break;
                }
            }
            game_stop();
        }
        co_return;
    }
    void Discord_dominoes_game::generate_decks() {
        for (const dpp::snowflake &i: this->get_players()) {
            while (this->_decks[i].size() < _pieces_in_deck) {
                std::uniform_int_distribution<int> random_int{0,
                                                              static_cast<int>(this->_local_list_of_domino.size() - 1)};
                int id = random_int(this->_rd);
                this->_decks[i].push_back(this->_local_list_of_domino[id]);
                this->_local_list_of_domino.erase(this->_local_list_of_domino.begin() + id);
            }
        }
    }

    void Discord_dominoes_game::get_possible_moves() {
        this->_possible_moves.clear();
        for (auto &i: this->_decks[get_current_player()]) {
            if (i[0] == this->_board[0][0] || i[1] == this->_board[0][0] || i[0] == this->_board.back()[1] ||
                i[1] == this->_board.back()[1]) {
                this->_possible_moves.push_back(i);
            }
        }
    }

    void Discord_dominoes_game::make_first_turn() {
        // look for smallest "doubles" (0,0 not count)
        dominoes::piece smallest = {INT_MAX, INT_MAX};
        for (int i = 0; i < static_cast<int>(this->get_players().size()); i++) {
            for (auto &p: this->_decks[this->get_players()[i]]) {
                if (p[0] == p[1] && p[0] < smallest[0] && p[0] != 0) {
                    smallest = p;
                    this->set_current_player_index(i);
                }
            }
        }
        // look for (0,0)
        if (smallest[0] == INT_MAX) {
            for (int i = 0; i < static_cast<int>(this->get_players().size()); i++) {
                for (auto &p: this->_decks[this->get_players()[i]]) {
                    if (p[0] == p[1] && p[0] == 0) {
                        smallest = p;
                        this->set_current_player_index(i);
                    }
                }
            }
        }

        // no doubles
        int biggest = 0;
        if (smallest[0] == INT_MAX) {
            dominoes::piece piece = {0, 0};
            for (int i = 0; i < static_cast<int>(this->get_players().size()); i++) {
                for (auto &p: this->_decks[this->get_players()[i]]) {
                    if (p[0] * p[1] > piece[0] * piece[1]) {
                        piece = p;
                        this->set_current_player_index(i);
                        smallest = piece;
                    }
                }
            }
        }

        // set_player
        this->_board.push_back(smallest);
        this->_decks[get_current_player()].erase(
            std::remove(this->_decks[get_current_player()].begin(), this->_decks[get_current_player()].end(), smallest),
            this->_decks[get_current_player()].end());
        this->next_player();
    }

    void Discord_dominoes_game::make_message() {
        this->_messages[get_current_player()].components.clear();
        this->_messages[get_current_player()].embeds.clear();

        std::string description = std::format(
            "Dominoes left in deck: **{}**\nTimeout: <t:{}:R>\nMake your move.", this->_local_list_of_domino.size(),
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count() +
                60);
        dpp::embed embed =
            dpp::embed().set_title("Dominoes game!").set_description(description).set_color(dpp::colors::blue);
        this->_messages[get_current_player()].add_embed(embed);
        this->_messages[get_current_player()].add_component(dpp::component().set_type(dpp::cot_action_row));
        if (!this->_possible_moves.empty()) {
            std::string id;
            for (auto &i: this->_possible_moves) {
                if (this->_messages[get_current_player()].components.back().components.size() == 5) {
                    this->_messages[get_current_player()].add_component(dpp::component().set_type(dpp::cot_action_row));
                }
                auto emoji = dominoes::emojis[i];
                this->_messages[get_current_player()].components.back().add_component(
                    dpp::component()
                        .set_type(dpp::cot_button)
                        .set_label(std::to_string(i[0]) + " | " + std::to_string(i[1]))
                        .set_id(std::to_string(i[0]) + "|" + std::to_string(i[1]))
                        .set_emoji(std::get<0>(emoji), std::get<1>(emoji)));
            }
        } else {
            this->_messages[get_current_player()].components[0].add_component(
                dpp::component().set_type(dpp::cot_button).set_label("Take dominoes").set_id("take"));
        }
    }

    void Discord_dominoes_game::end_game_custom_condition() {
        // count scores
        std::vector<int> scores;
        for (int i = 0; i < static_cast<int>(get_players().size()); i++) {
            scores.push_back(0);
            for (auto &p: _decks[get_players()[i]]) {
                scores[i] += p[0] + p[1];
            }
        }

        // select winner
        std::vector<int> copy_sc(scores);
        std::sort(scores.begin(), scores.end());
        std::vector<dpp::snowflake> winners;
        std::string los_str;
        std::string win_str;

        for (int i = 0; i < static_cast<int>(get_players().size()); i++) {
            if (copy_sc[i] == scores[0]) {
                if (!win_str.empty()) {
                    win_str += ", ";
                }
                win_str += dpp::utility::user_mention(get_players()[i]);
                winners.push_back(get_players()[i]);
            } else {
                if (!los_str.empty()) {
                    los_str += ", ";
                }
                los_str += dpp::utility::user_mention(get_players()[i]);
            }
        }

        std::string players_text;
        std::vector<dpp::snowflake> temp;
        std::vector<dpp::snowflake> set_tmp1 = _original_players_list;
        std::vector<dpp::snowflake> set_tmp2 = winners;
        std::sort(set_tmp1.begin(),set_tmp1.end());
        std::sort(set_tmp2.begin(),set_tmp2.end());
        std::ranges::set_difference(set_tmp1, set_tmp2,
                                    std::back_inserter(temp));
        if (!temp.empty()) {
            players_text += dpp::utility::user_mention(temp[0]);
            for (size_t i = 1; i < temp.size(); i++) {
                players_text += ", " + dpp::utility::user_mention(temp[i]);
            }
        }

        if (winners.size() > 1) {
            for (auto &i: winners) {
                remove_player(USER_REMOVE_REASON::DRAW, i);
                _messages[i].embeds.clear();
                _messages[i].attachments.clear();
                _messages[i].components.clear();
                _messages[i].add_embed(
                    dpp::embed()
                        .set_color(dpp::colors::yellow)
                        .set_title("Dominoes game over!")
                        .set_description(
                            "Draw. No one can make moves and you got same sum of dominoes as other player(s)."));
                _data.bot->message_edit(_messages[get_current_player()]);
            }

            _main_message.add_embed(dpp::embed().set_color(dpp::colors::yellow).set_title("Dominoes game over").set_description(
                std::format("Players {} played in draw.\nPlayers {} will be luckier next time",win_str,players_text)));

            _data.bot->message_edit(_main_message);
        } else {
            auto i = winners[0];
            remove_player(USER_REMOVE_REASON::WIN, i);
            _messages[i].embeds.clear();
            _messages[i].attachments.clear();
            _messages[i].components.clear();
            _messages[i].add_embed(
                dpp::embed()
                    .set_color(dpp::colors::green)
                    .set_title("Dominoes game over!")
                    .set_description("You won. No one could make a move but you had smallest sum of dominoes."));
            _data.bot->message_edit(_messages[get_current_player()]);

            _main_message.add_embed(dpp::embed().set_color(dpp::colors::yellow).set_title("Dominoes game over").set_description(
                    std::format("Player {} won the game.\nPlayer(s) {} will be luckier next time",win_str,players_text)));
            _data.bot->message_edit(_main_message);
        }
        std::vector<dpp::snowflake> temp2;
        for (auto& i : get_players()) {
            _messages[i].embeds.clear();
            _messages[i].attachments.clear();
            _messages[i].components.clear();
            _messages[i].add_embed(
                dpp::embed()
                    .set_color(dpp::colors::red)
                    .set_title("Dominoes game over!")
                    .set_description("You lost. No one could make a move but you had higher sum of dominoes than other players."));
            _data.bot->message_edit(_messages[get_current_player()]);
            temp2.push_back(i);
        }

        for( auto& i: temp2) {
            remove_player(USER_REMOVE_REASON::LOSE,i);
        }
        _main_message.embeds.clear();

    }

    void Discord_dominoes_game::generate_image(dpp::message &m, dpp::embed &embed) {
        _img_cnt++;
        m.attachments.clear();

        int size = 512;
        int size_for_deck = size / 8;
        int size_for_field = size - size_for_deck * 2;
        Image_ptr img = _data.image_processing->create_image({size, size}, {2, 97, 27});

        // draw stuff on board
        int figures_per_colum = 5;
        int direction = 2;
        int distance_between_dominoes_field = size_for_field / 50;
        int figure_size =
            (size_for_field - distance_between_dominoes_field * figures_per_colum * direction) / figures_per_colum / 2;
        int start_x = size_for_deck + distance_between_dominoes_field;
        int start_y = size_for_deck + 1;
        int grid_x = 0;
        float grid_y = 0;
        float pos_now_x = grid_x;
        float pos_now_y = grid_y;
        bool turn = false;


        std::vector<dpp::snowflake> diff;
        auto view_keys = std::views::keys(_hidden_deck_images);
        std::vector<dpp::snowflake> set_tmp1 = get_players();
        std::vector<dpp::snowflake> set_tmp2{view_keys.begin(),view_keys.end()};
        std::sort(set_tmp1.begin(),set_tmp1.end());
        std::sort(set_tmp2.begin(),set_tmp2.end());
        std::ranges::set_difference(set_tmp1,set_tmp2 , std::back_inserter(diff));
        for (auto &i: diff) {
            generate_hidden_deck_image(i, {size_for_field - size_for_deck * 2, size_for_deck});
        }

        for (auto &raw_piece: this->_board) {
            float pos_now_x = grid_x;
            float pos_now_y = grid_y;
            dominoes::piece piece = raw_piece;
            std::ranges::sort(piece);

            Image_ptr in = _data.image_processing->cache_get(std::format("dominoes_piece_{}_{}", piece[0], piece[1]),
                                                             {figure_size, figure_size * 2});
            if ((grid_x % 2 == 0 && ((!turn && piece[0] != raw_piece[0]) || (turn && piece[0] == raw_piece[0])))) {
                in->rotate(180);
            }
            if (grid_x % 2 != 0) {
                in = _data.image_processing->create_image({figure_size * 2, figure_size * 2}, {0, 0, 0, 0});
                Image_ptr abc = _data.image_processing->cache_get(
                    std::format("dominoes_piece_{}_{}", piece[0], piece[1]), {figure_size, figure_size * 2});
                if (piece[0] != raw_piece[0]) {
                    abc->rotate(180);
                }
                in->overlay_image(abc);
                in->rotate(90);
                grid_x++;
                grid_y += (turn ? 0 : 1);
                turn = !turn;
            }
            pos_now_y -= (static_cast<int>(pos_now_x) % 4 == 0 && pos_now_x != 0 && pos_now_y > 1 ? 0.25 : 0);
            img->overlay_image(in, {static_cast<int>(start_x + pos_now_x * figure_size +
                                                     ((pos_now_x + 1) / 2 + ((int) pos_now_x % 2 != 0) * 0.5) *
                                                         distance_between_dominoes_field),
                                    static_cast<int>(start_y + pos_now_y * figure_size +
                                                     (pos_now_y) *distance_between_dominoes_field)});
            grid_y += direction * (int) (turn ? -1 : 1);

            if (grid_y <= 0 && turn) {
                grid_y = 0.25;
                grid_x++;
            } else if ((int) (grid_y) > figures_per_colum * direction - direction && !turn) {
                grid_y = figures_per_colum * direction - direction;
                grid_x++;
            }
        }

        int position_and_rotation[][3] = {{0, size_for_deck * 2, 90},
                                          {size_for_deck * 2, 0, 0},
                                          {size_for_field + size_for_deck, size_for_deck * 2, -90}};

        Image_ptr deck_img =
            generate_deck_image(get_current_player(), {size_for_field - size_for_deck * 2, size_for_deck});

        img->overlay_image(deck_img, {size_for_deck * 2, size - size_for_deck});
        if (get_players().size() == 2) {
            std::vector<dpp::snowflake> diff;
            std::vector<dpp::snowflake>set_tmp1 = get_players();
            std::vector<dpp::snowflake> set_tmp2 = {get_current_player()};
            std::sort(set_tmp1.begin(),set_tmp1.end());
            std::sort(set_tmp2.begin(),set_tmp2.end());
            std::ranges::set_difference(set_tmp1, set_tmp2,
                                        std::back_inserter(diff));
            img->overlay_image(this->_hidden_deck_images[diff[0]], {size_for_deck * 2, 0});
        }
        // multiple players
        else {
            // x,y, (bool) rotate
            size_t local_player = get_current_player_index();
            for (int j = 0; j < static_cast<int>(get_players().size() - 1); j++) {
                local_player++;
                if (local_player >= static_cast<int>(get_players().size())) {
                    local_player = 0;
                }
                Image_ptr other_deck = _hidden_deck_images[get_players()[dpp::snowflake{local_player}]];
                other_deck->rotate(position_and_rotation[j][2]);
                img->overlay_image(other_deck, {position_and_rotation[j][0], position_and_rotation[j][1]});
                other_deck->rotate(-position_and_rotation[j][2]);
            }
        }
        embed.set_image(add_image(m, img));
    }

    void Discord_dominoes_game::generate_hidden_deck_image(const dpp::snowflake &player, const Vector2i &resolution) {
        Image_ptr deck_img = _data.image_processing->create_image(resolution, {0, 0, 0, 0});
        int size_of_dominoes =
            std::min(resolution.y / 2, static_cast<int>(resolution.x / (this->_decks[player].size() + 1)));
        int distance_between_dominoes = (resolution.x - (size_of_dominoes * this->_decks[player].size())) /
                                        (this->_decks[player].size() ? this->_decks[player].size() : 1);
        Image_ptr clear_image =
            _data.image_processing->create_image({size_of_dominoes, size_of_dominoes * 2}, {255, 255, 255});
        for (int i = 0; i < static_cast<int>(this->_decks[player].size()); i++) {
            deck_img->overlay_image(clear_image, {size_of_dominoes * i + distance_between_dominoes * i, 0});
        }
        this->_hidden_deck_images.insert_or_assign(player, deck_img);
    }

    void Discord_dominoes_game::win() {

        std::string players_text;
        std::vector<dpp::snowflake> losers;
        std::vector<dpp::snowflake> set_tmp1 = _original_players_list;
        std::vector<dpp::snowflake> set_tmp2 = {get_current_player()};
        std::sort(set_tmp1.begin(),set_tmp1.end());
        std::sort(set_tmp2.begin(),set_tmp2.end());
        std::ranges::set_difference(set_tmp1, set_tmp2,
                                    std::back_inserter(losers));
        if (!losers.empty()) {
            players_text += dpp::utility::user_mention(losers[0]);
            for (size_t i = 1; i < losers.size(); i++) {
                players_text += ", " + dpp::utility::user_mention(losers[i]);
            }
        }

        _main_message.embeds.clear();
        _main_message.add_embed(
            dpp::embed()
                .set_color(dpp::colors::green)
                .set_title("Dominoes game over!")
                .set_description(std::format("Player: {} won the game\n Player(s): {} will be luckier next time",
                                             dpp::utility::user_mention(get_current_player()), players_text)));
        _data.bot->message_edit(_main_message);
    }


    Image_ptr Discord_dominoes_game::generate_deck_image(const dpp::snowflake &player, const Vector2i &resolution) {
        Image_ptr deck_img = _data.image_processing->create_image(resolution, {0, 0, 0, 0});
        int size_of_dominoes =
            std::min(resolution.y / 2, static_cast<int>(resolution.x / (this->_decks[player].size() + 1)));
        int distance_between_dominoes = (resolution.x - (size_of_dominoes * this->_decks[player].size())) /
                                        (this->_decks[player].size() ? this->_decks[player].size() : 1);
        for (int i = 0; i < static_cast<int>(this->_decks[player].size()); i++) {
            Image_ptr in = _data.image_processing->cache_get(
                std::format("dominoes_piece_{}_{}", _decks[player][i][0], _decks[player][i][1]),
                {size_of_dominoes, size_of_dominoes * 2});
            deck_img->overlay_image(in, {size_of_dominoes * i + distance_between_dominoes * i, 0});
        }
        return deck_img;
    }
} // namespace gb
