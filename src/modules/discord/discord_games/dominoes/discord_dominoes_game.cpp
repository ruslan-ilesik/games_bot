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
        _data.bot->reply(event,dpp::message().add_embed(dpp::embed().set_color(dpp::colors::blue).set_title("Game is starting...")));
        Direct_messages_return messages = co_await get_private_messages(get_players());
        if (messages.first) {


            std::vector<dpp::snowflake> missing_players;
            auto users_valid = std::views::keys(messages.second);
            std::ranges::set_difference(this->get_players(),users_valid,std::back_inserter(missing_players));

            std::string players_text;
            if (!missing_players.empty()) {
                players_text += dpp::utility::user_mention(missing_players[0]);
                for (size_t i =1; i < missing_players.size(); i++) {
                    players_text += ", " +  dpp::utility::user_mention(missing_players[i]);
                }
            }

            dpp::message m;
            dpp::embed embed;
            embed.set_color(dpp::colors::red)
            .set_title("ERROR sending private message!")
            .set_description(std::format("User(s) {} should allow bot to send private messages to them!",players_text));
            m.add_embed(embed);
            m.channel_id = event.command.channel_id;
            m.id = event.command.message_id;
            _data.bot->message_edit(m);
            for (auto& [id,message] : messages.second) {
                message.embeds.clear();
                message.add_embed(embed);
                message.content.clear();
                _data.bot->message_edit(message);
            }

        }
        else {
            game_start(event.command.channel_id, event.command.guild_id);
            this->pieces_in_deck = (this->get_players().size() > 2 ? 5 : 7);
            generate_decks();
            make_first_turn();


            game_stop();
        }
        co_return;
    }
    void Discord_dominoes_game::generate_decks() {
        for (const dpp::snowflake &i: this->get_players()) {
            while (this->decks[i].size() < pieces_in_deck) {
                std::uniform_int_distribution<int> random_int{0,
                                                              static_cast<int>(this->local_list_of_domino.size() - 1)};
                int id = random_int(this->rd);
                this->decks[i].push_back(this->local_list_of_domino[id]);
                this->local_list_of_domino.erase(this->local_list_of_domino.begin() + id);
            }
        }
    }
    void Discord_dominoes_game::make_first_turn() {
        // look for smallest "doubles" (0,0 not count)
        dominoes::piece smallest = {INT_MAX, INT_MAX};
        for (int i = 0; i < static_cast<int>(this->get_players().size()); i++) {
            for (auto &p: this->decks[this->get_players()[i]]) {
                if (p[0] == p[1] && p[0] < smallest[0] && p[0] != 0) {
                    smallest = p;
                    this->set_current_player_index(i);
                }
            }
        }
        // look for (0,0)
        if (smallest[0] == INT_MAX) {
            for (int i = 0; i < static_cast<int>(this->get_players().size()); i++) {
                for (auto &p: this->decks[this->get_players()[i]]) {
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
                for (auto &p: this->decks[this->get_players()[i]]) {
                    if (p[0] * p[1] > piece[0] * piece[1]) {
                        piece = p;
                        this->set_current_player_index(i);
                        smallest = piece;
                    }
                }
            }
        }

        // set_player
        this->board.push_back(smallest);
        this->decks[get_current_player()].erase(
            std::remove(this->decks[get_current_player()].begin(), this->decks[get_current_player()].end(), smallest),
            this->decks[get_current_player()].end());
        this->next_player();
    }
} // namespace gb
