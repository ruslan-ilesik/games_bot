//
// Created by ilesik on 10/3/24.
//

#include "discord_hangman_game.hpp"

namespace gb {

    std::string letter_to_emoji(char letter) {
        unsigned int beginning = 0x1F1E6;
        int diff = letter - 'a';
        unsigned int end = beginning + diff;
        std::string emoji;
        if (end >= 0x10000) {
            emoji += static_cast<char>(0xF0 | ((end >> 18) & 0x07));
            emoji += static_cast<char>(0x80 | ((end >> 12) & 0x3F));
            emoji += static_cast<char>(0x80 | ((end >> 6) & 0x3F));
            emoji += static_cast<char>(0x80 | (end & 0x3F));
        } else if (end >= 0x800) {
            emoji += static_cast<char>(0xE0 | ((end >> 12) & 0x0F));
            emoji += static_cast<char>(0x80 | ((end >> 6) & 0x3F));
            emoji += static_cast<char>(0x80 | (end & 0x3F));
        } else if (end >= 0x80) {
            emoji += static_cast<char>(0xC0 | ((end >> 6) & 0x1F));
            emoji += static_cast<char>(0x80 | (end & 0x3F));
        } else {
            emoji += static_cast<char>(end & 0x7F);
        }

        return emoji;
    };


    Discord_hangman_game::Discord_hangman_game(Game_data_initialization &_data,
                                               const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players,&Discord_hangman_game::run_btn) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_hangman_game::get_image_generators() {
        std::vector<std::pair<std::string, image_generator_t>> generators;
        for (int i = 0; i < 12; i++) {
            generators.emplace_back("hangman_" + std::to_string(i), [=](const Image_processing_ptr &image_processing,
                                                                        const Vector2i &resolution) {
                Image_ptr img = image_processing->create_image("./data/img/hangman/" + std::to_string(i) + ".png");
                img->resize(resolution);
                return img;
            });
        }

        return generators;
    }

    void Discord_hangman_game::prepare_message(dpp::message &message, const dpp::snowflake &player) {
        hangman::Player_ptr &p = _hangman_rel.at(player);
        message.components.clear();
        if (!p->is_finished()) {
            message.embeds[0]
                .set_title("Hangman game: ")
                .set_description(std::format("Player: {}\n"
                                             "Timeout: <t:{}:R>\n"
                                             "Mistakes: {}/{}\n"
                                             "Moves amount: {}",
                                             dpp::utility::user_mention(player),
                                             std::chrono::duration_cast<std::chrono::seconds>(
                                                 std::chrono::system_clock::now().time_since_epoch())
                                                     .count() +
                                                 60,
                                             p->error_cnt, 11, p->moves_cnt));
            dpp::component component;
            component.set_type(dpp::cot_action_row);
            auto chars = p->get_unused_characters();
            if (chars.size() < 26) {
                for (int i = 0; i < chars.size(); i++) {
                    if (component.components.size() == 5) {
                        message.add_component(component);
                        component = {};
                    }
                    component.add_component(dpp::component()
                                                .set_type(dpp::cot_button)
                                                .set_id(std::string{chars[i]} + std::to_string(player))
                                                .set_emoji(letter_to_emoji(chars[i])));
                }
                message.add_component(component);
            } else {
                int s_ind = 0;
                if (_users_buttons_page.at(player) == 1) {
                    s_ind = 24;
                    component.add_component(dpp::component()
                                                .set_type(dpp::cot_button)
                                                .set_label("Previous Page")
                                                .set_style(dpp::cos_primary)
                                                .set_emoji(std::string(reinterpret_cast<const char *>(u8"⏮️")))
                                                .set_id("back"));
                }

                for (int i = s_ind; i < chars.size() && i < s_ind + 24; i++) {
                    if (component.components.size() == 5) {
                        message.add_component(component);
                        component = {};
                    }
                    component.add_component(dpp::component()
                                                .set_type(dpp::cot_button)
                                                .set_id(std::string{chars[i]} + std::to_string(player))
                                                .set_emoji(letter_to_emoji(chars[i])));
                }
                if (_users_buttons_page.at(player) == 0) {
                    if (component.components.size() == 5) {
                        message.add_component(component);
                        component = {};
                    }
                    component.add_component(dpp::component()
                                                .set_type(dpp::cot_button)
                                                .set_label("Next Page")
                                                .set_style(dpp::cos_primary)
                                                .set_emoji("⏭️")
                                                .set_id("next"));
                }
                message.add_component(component);
            }

        } else {
            // achievement for solving game with one mistake left
            if (p->error_cnt == 10 && p->is_win()) {
                _data.achievements_processing->activate_achievement("Captain Jack Sparrow", player, message.channel_id);
            }

            // achievement for losing game with no correct guessed letters
            if (p->error_cnt == p->moves_cnt && p->is_finished() && !p->is_eliminated()) {
                _data.achievements_processing->activate_achievement("Stalin repression", player, message.channel_id);
            }
            int active_p_cnt = std::ranges::count_if(std::ranges::views::values(_hangman_rel),[=](hangman::Player_ptr& p){
                return !p->is_finished();});
            if (active_p_cnt >= 1) {
                if (p->is_eliminated()) {
                    message.embeds[0].set_title("Game timeout!");
                } else {
                    message.embeds[0].set_title("Hangman game finished!");
                }

                message.embeds[0].set_description(
                    std::format("You can see results in  https://discord.com/channels/{}/{}/{} after all players will "
                                "finish the game",
                                _message.guild_id, _message.channel_id, _message.id));
            }
            if (_hangman_rel.size() == 1) {
                if (p->is_eliminated()) {
                    message.embeds[0]
                        .set_title("Game timeout!")
                        .set_color(dpp::colors::red)
                        .set_description("Game timeout has been reached, if you still want to play, start a new game.");
                    remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                } else if (!p->is_win()) {
                    message.embeds[0]
                        .set_title("Game over!")
                        .set_color(dpp::colors::red)
                        .set_description("You could not guess the word and lost a game!\nWord was: **" + _word + "**");
                    remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                } else {
                    message.embeds[0]
                        .set_title("Game over!")
                        .set_color(dpp::colors::green)
                        .set_description("You successfully guessed a word and won a game!");
                    remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                }
            } else if (_game.is_game_finished()) {
                auto result = _game.get_winners();
                auto &embed = _message.embeds[0];
                if (_game.is_all_eliminated() && result.empty()) {
                    embed.set_title("Game over!")
                        .set_color(dpp::colors::red)
                        .set_description("Everyone reached timeout!\n"
                                         "Word was: **" +
                                         _word + "**");
                    int temp =  get_players().size();
                    for (int i = 0; i < temp; i++) {
                        remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                    }
                } else if (result.empty()) {
                    embed.set_title("Game over!")
                        .set_color(dpp::colors::red)
                        .set_description("No one was able to win the game!\n"
                                         "Word was: **" +
                                         _word + "**");
                    for (int i = 0; i < get_players().size(); i++) {
                        remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                    }
                } else if (result.size() >= 2) {
                    embed.set_title("Game over!")
                        .set_color(dpp::colors::yellow)
                        .set_description("Draw! 2 or more players got same results!\n"
                                         "Word was: **" +
                                         _word + "**");
                    int temp =  get_players().size();
                    for (int i = 0; i < temp; i++) {
                        if (std::ranges::find(result, _hangman_rel.at(get_current_player())) != result.end()) {
                            remove_player(USER_REMOVE_REASON::DRAW, get_current_player());
                        } else {
                            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                        }
                    }
                } else {
                    dpp::snowflake winner_id;
                    for (const auto &pair: _hangman_rel) {
                        if (pair.second == result[0]) {
                            winner_id = pair.first;
                            break;
                        }
                    }
                    embed.set_title("Game Over!")
                        .set_color(dpp::colors::green)
                        .set_description("Winner: " + dpp::utility::user_mention(winner_id) +
                                         "\n"
                                         "Word was: **" +
                                         _word + "**");
                    int temp =  get_players().size();
                    for (int i = 0; i < temp; i++) {
                        if (std::ranges::find(result, _hangman_rel.at(get_current_player())) != result.end()) {
                            remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                        } else {
                            remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                        }
                    }
                }
                std::string results = "";
                for (auto &[i, p]: _hangman_rel) {
                    auto guessed_word = p->get_guessed_word();
                    auto used_characters = p->get_used_characters();
                    auto guessed_word_copy = guessed_word;
                    std::ranges::sort(guessed_word_copy);
                    std::ranges::sort(used_characters);
                    std::vector<char> difference;
                    std::ranges::set_difference(used_characters, guessed_word_copy, std::back_inserter(difference));
                    std::string errors;
                    for (auto i: difference) {
                        errors += std::string(1, i) + ", ";
                    }
                    errors = errors.substr(0, errors.size() - 2);
                    results += dpp::utility::user_mention(i) +
                               " results\n"
                               "Guessed result: `" +
                               std::string(guessed_word.begin(), guessed_word.end()) + "`\nErrors: " + errors +
                               "\n----------------\n";
                }
                embed.add_field("Results by user", results);
                _message.embeds[0].set_image(add_image(_message, create_image(player)));
                _data.bot->message_edit(_message);
            }
        }
        message.embeds[0].set_image(add_image(message, create_image(player)));
    }

    Image_ptr Discord_hangman_game::create_image(const dpp::snowflake &player) {
        _img_cnt++;
        hangman::Player_ptr &p = _hangman_rel.at(player);
        int size = 256;
        int offset = 10;
        double font_scale = 0.7;
        int thickness = 2;
        Color text_color(0, 0, 0); // Black color
        Color right_text_color(0, 255, 0); // Green color
        Color cross_color(255, 0, 0); // Red color
        int spacing = 10; // Spacing between characters
        Image_ptr img = _data.image_processing->create_image({size, size * 2}, {255, 255, 255});
        std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

        // Calculate the width of each character
        std::vector<int> charWidths;
        std::vector<int> charHeights;
        for (char c: alphabet) {
            auto t_s = img->get_text_size(std::string(1, c), font_scale, thickness);
            charWidths.push_back(t_s.x);
            charHeights.push_back(t_s.y);
        }
        // Draw the known word
        auto known_word = p->get_guessed_word();
        int x = offset; // Reset x position
        int y = size + size / 8; // Move down one line

        for (size_t i = 0; i < known_word.size(); i++) {
            img->draw_text(std::string(1, known_word[i] == '_' ? '_' : known_word[i] + ('A' - 'a')), {x, y}, font_scale,
                           text_color, thickness);
            x += charWidths[known_word[i] == '_' ? charWidths.size() - 1 : known_word[i] - 'a'] + spacing;
            // Update position for next character
            // Wrap to new line if necessary
            if (i < known_word.size() - 1 &&
                x + charWidths[known_word[i + 1] == '_' ? charWidths.size() - 1 : known_word[i + 1] - 'a'] + spacing >
                    size - offset) {
                x = offset;
                y += size / 8; // Move down one line
            }
        }
        // Draw the line of text
        x = offset; // Start position with offset
        y = size + size / 8 + size / 2; // Center vertically
        const auto &unused_chars = p->get_unused_characters();
        size_t ind = 0;
        for (size_t i = 0; i < alphabet.size() - 1; ++i) {
            bool right_letter = std::ranges::find(known_word, alphabet[i] - ('A' - 'a')) != known_word.end();
            if (unused_chars[ind] + ('A' - 'a') != alphabet[i]) {
                if (!right_letter) {
                    img->draw_text(std::string(1, alphabet[i]), {x, y}, font_scale, text_color, thickness);
                    img->draw_line({x, y - charHeights[i]}, {x + charWidths[i], y}, cross_color, thickness);
                    img->draw_line({x, y}, {x + charWidths[i], y - charHeights[i]}, cross_color, thickness);
                } else {
                    img->draw_text(std::string(1, alphabet[i]), {x, y}, font_scale, right_text_color, thickness);
                }
            } else {
                img->draw_text(std::string(1, alphabet[i]), {x, y}, font_scale, text_color, thickness);
                ind = std::min(ind + 1, unused_chars.size() - 1);
            }

            x += charWidths[i] + spacing; // Update position for next character

            // Wrap to new line if necessary
            if (x + charWidths[i + 1] + spacing > size - offset) {
                x = offset;
                y += size / 8; // Move down one line
            }
        }

        auto icon = _data.image_processing->cache_get("hangman_" + std::to_string(p->error_cnt), {size, size});
        img->overlay_image(icon, {0, 0});
        return img;
    }


    void Discord_hangman_game::start() {
        _word = hangman::get_random_word();
        for (auto i: get_players()) {
            auto p = hangman::Player::create(_word);
            this->_hangman_rel.emplace(i, p);
            _game.add_player(p);
            _users_buttons_page.emplace(i, 0);
        }
    }
    std::string Discord_hangman_game::results_json() {
        nlohmann::json json;
        for (auto &[i, p]: _hangman_rel) {
            std::vector<char> wtg = p->get_word_to_guess();
            std::vector<char> gw = p->get_guessed_word();
            std::vector<char> uc = p->get_used_characters();

            json[std::to_string(i)] = {{"guessed_word", std::string(gw.begin(), gw.end())},
                                       {"word", std::string(wtg.begin(), wtg.end())},
                                       {"used_characters", std::string(uc.begin(), uc.end())},
                                       {"errors_cnt", p->error_cnt},
                                       {"moves_amount", p->moves_cnt}};
        }
        return json.dump();
    }

    dpp::task<void> Discord_hangman_game::per_player_run(dpp::snowflake player,
                                                         dpp::task<Button_click_return> &button_click_awaitable) {
        Button_click_return r;
        dpp::button_click_t event;
        while (1) {
            r = co_await button_click_awaitable;
            std::unique_lock lk(_mutex);
            if (r.second) {
                // timeout
                _hangman_rel.at(player)->eliminate();
                prepare_message(_messages.at(player), player);
                if (_messages.at(player).id == 0) {
                    _data.bot->message_create(_messages.at(player));
                } else {
                    _data.bot->message_edit(_messages.at(player));
                }
                break;
            }
            event = r.first;
            _messages.at(player).id = event.command.message_id;
            auto &p = _hangman_rel.at(player);
            if (event.custom_id == "next") {
                _users_buttons_page[player]++;
            } else if (event.custom_id == "back") {
                _users_buttons_page[player]--;
            } else {
                char letter = event.custom_id[0];
                _game.make_move(p, letter);
            }
            prepare_message(_messages.at(player), player);
            if (_hangman_rel.at(player)->is_finished()) {
                _data.bot->event_edit_original_response(event, _messages.at(player));
                break;
            } else {
                button_click_awaitable = _data.button_click_handler->wait_for_with_reply(_messages.at(player), {player}, 60);
                _data.bot->event_edit_original_response(event, _messages.at(player));
            }
        }
        co_return;
    }

    dpp::task<void> Discord_hangman_game::run(dpp::slashcommand_t sevent) {
        game_start(sevent.command.channel_id, sevent.command.guild_id);
        try {
            start();
            _message.add_embed(dpp::embed());
            _message.guild_id = sevent.command.guild_id;
            _message.channel_id = sevent.command.channel_id;
            _messages.insert({sevent.command.usr.id, _message});
            prepare_message(_messages.at(get_current_player()), get_current_player());
            dpp::task<Button_click_return> button_click_awaitable =
                _data.button_click_handler->wait_for_with_reply(_messages.at(get_current_player()), {get_current_player()}, 60);
            _data.bot->reply(sevent, _messages.at(get_current_player()));
            co_await per_player_run(get_current_player(), button_click_awaitable);
        } catch (...) {
            game_stop();
            throw;
        }
        game_stop(results_json());
        co_return;
    }

    dpp::task<std::string> Discord_hangman_game::run_btn(dpp::button_click_t event) {
        start();
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
            std::list<dpp::task<Button_click_return>> temp;
            std::list<dpp::task<void>> list;
            _messages = messages.second;
            for (size_t i = 0; i < get_players().size(); i++) {
                prepare_message(_messages.at(get_current_player()), get_current_player());
                temp.push_back(_data.button_click_handler->wait_for_with_reply(_messages.at(get_current_player()),
                                                                    {get_current_player()}, 60));
                list.push_back(per_player_run(get_current_player(), temp.back()));
                _data.bot->message_edit(_messages.at(get_current_player()));
                next_player();
            }
            _message.embeds[0]
                .set_title("Hangman game!")
                .set_description("Players should check their private messages with bot");
            _data.bot->message_edit(_message);
            for (auto &i: list) {
                co_await i;
            }

        }

        co_return results_json();
    }
} // namespace gb

