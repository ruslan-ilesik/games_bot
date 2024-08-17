//
// Created by ilesik on 7/31/24.
//

#include "discord_game_command.hpp"

namespace gb {
    Discord_game_command::Discord_game_command(const std::string &name,
                                               const std::vector<std::string> &dependencies) : Module(
        name, dependencies) {
        // insert basic game dependencies if they are not there
        for (auto &i: Discord_game::get_basic_game_dependencies()) {
            if (std::find(_dependencies.begin(), _dependencies.end(), i) == _dependencies.end()) {
                _dependencies.push_back(i);
            }
        }
    }

    void Discord_game_command::innit(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        _games_manager = std::static_pointer_cast<Discord_games_manager>(modules.at("discord_games_manager"));
        _image_processing = std::static_pointer_cast<Image_processing>(modules.at("image_processing"));
        _button_click_handler = std::static_pointer_cast<Discord_button_click_handler>(modules.at("discord_button_click_handler"));
    }

    void Discord_game_command::stop() {
        std::mutex m;
        std::unique_lock lk(m);
        _cv.wait(lk, [this]() { return _games_cnt == 0; });
    }

    void Discord_game_command::run() {

    }

    void Discord_game_command::command_finished() {
        _games_cnt--;
        _cv.notify_all();
    }



    dpp::task<Discord_game_command::Lobby_return> Discord_game_command::lobby(const dpp::slashcommand_t &event,
                                                                              std::vector<dpp::snowflake> players, const dpp::snowflake &host, unsigned int players_amount) {
        std::vector<dpp::snowflake> joined_players{host};
        std::vector<dpp::snowflake> required_players = players;
        auto e = std::ranges::find(players, host);
        if (e != players.end()) {
            players.erase(e);
        }
        required_players.push_back(host);
        dpp::button_click_t click;
        bool is_click = false;
        while(true){
            unsigned int waiting_for_amount = players_amount - joined_players.size() - players.size();
            if (players_amount ==  joined_players.size()) {
                co_return {false,click,joined_players};
            }
            dpp::message m;
            dpp::embed embed;

            std::string players_joined = std::accumulate(
                std::next(joined_players.begin()),
                joined_players.end(),
                dpp::utility::user_mention(joined_players[0]),
                [](const std::string& a, const dpp::snowflake &b) {
                    return a + ", " + dpp::utility::user_mention(b);
                }
            );

            std::string players_waiting = "";

            if (!players.empty()) {
                players_waiting = std::accumulate(
                    std::next(players.begin()),
                    players.end(),
                    dpp::utility::user_mention(players[0]),
                    [](const std::string &a, const dpp::snowflake &b) {
                        return a + ", " + dpp::utility::user_mention(b);
                    }
                );
            }

            embed.add_field("Lobby for " + lobby_title + " game.", lobby_description)
                .set_description(std::format("Timeout: <t:{}:R>",
                                            std::chrono::duration_cast<std::chrono::seconds>(
                                                std::chrono::system_clock::now().time_since_epoch()).count()+60
                                            )
                                        )
                .add_field("Joined players:", players_joined)
                .add_field("Waiting for:", std::format("{}\nAny {} player/s", players_waiting,waiting_for_amount
                                                      ))
                .set_thumbnail(lobby_image_url)
                .set_color(dpp::colors::gold);

            m.add_embed(embed);
            dpp::component cmp;
            cmp.add_component(dpp::component()
                                .set_type(dpp::cot_button)
                                .set_id("join")
                                .set_emoji("✅")
                                .set_label("Join")
                                .set_style(dpp::cos_success)
                                );
            cmp.add_component(dpp::component()
                               .set_type(dpp::cot_button)
                               .set_id("cancel")
                               .set_emoji("❌")
                               .set_label("Cancel")
                               .set_style(dpp::cos_danger)
                               );
            m.add_component(cmp);

            std::vector<dpp::snowflake> waiting = {};
            if (!players.empty() && players.size()+joined_players.size() >= players_amount) {
                waiting = players;
            }
            bool fst = true;
            auto awaitable = _button_click_handler->wait_for(m,waiting,60,fst,false);
            if (is_click) {
                _bot->reply(click,m);
            }
            else {
                _bot->reply(event,m);
            }

            is_click = true;
            time_t timeout =  std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() +60;
            while(1) {
                if (!fst) {
                    awaitable = _button_click_handler->wait_for(m,waiting,timeout - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(),fst,false);
                }
                Button_click_return r = co_await awaitable;
                fst = false;
                if (r.second) {
                    _button_click_handler->clear_ids(m);
                    co_return {true,{},{}};
                }

                 click = r.first;
                if (click.custom_id == "join") {
                    auto e = std::ranges::find(joined_players, click.command.usr.id);
                    if (e != joined_players.end()) {
                        dpp::message error_msg;
                        error_msg.set_flags(dpp::m_ephemeral)
                                .add_embed(dpp::embed()
                                        .set_color(dpp::colors::red)
                                        .set_title("Error joining the game.")
                                        .set_description("You have already joined this game.")
                                );
                        _bot->reply_new(click,error_msg);
                        continue;
                    }
                    joined_players.push_back(click.command.usr.id);
                    e = std::ranges::find(players, click.command.usr.id);
                    if (e != players.end()) {
                        players.erase(e);
                    }
                    _button_click_handler->clear_ids(m);
                    break;
                }

                //cancel
                else {
                    auto e = std::ranges::find(required_players, click.command.usr.id);
                    if (e == required_players.end()) {
                        e = std::ranges::find(joined_players, click.command.usr.id);
                        joined_players.erase(e);
                        _button_click_handler->clear_ids(m);
                        break;
                    }
                    //required player left, stop the game
                    dpp::embed error_embed {};
                    error_embed.set_color(dpp::colors::red)
                                .set_title(lobby_title+" lobby closed!")
                                .set_description("Player " + dpp::utility::user_mention(click.command.usr.id)+ "left the game while they were required for game to start.")
                                .set_thumbnail(lobby_image_url);
                    _bot->reply(click,dpp::message().add_embed(error_embed));
                    _button_click_handler->clear_ids(m);
                    co_return {true,{},{}};
                }
            }
        }
        co_return {true, {},{}};
    }

    void Discord_game_command::command_run() {
        _games_cnt++;
    }
} // gb
