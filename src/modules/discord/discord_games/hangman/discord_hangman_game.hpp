//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/games/hangman/hangman.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>


namespace gb {
    class Discord_hangman_game : public Discord_game {
        std::map<dpp::snowflake, dpp::message> _messages;
        std::mutex _mutex;
        std::map<dpp::snowflake,hangman::Player_ptr> _hangman_rel;
        std::map<dpp::snowflake,int> _users_buttons_page;
        hangman::Hangman _game = {};
        std::string _word;
        dpp::message _message;

    public:
        Discord_hangman_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        void prepare_message(dpp::message &message, const dpp::snowflake &player);

        Image_ptr create_image(const dpp::snowflake& player);

        void start();

        std::string results_json();

        dpp::task<void> per_player_run(dpp::snowflake player, dpp::task<Button_click_return>& button_click_awaitable);

        dpp::task<void> run(dpp::slashcommand_t sevent);
        dpp::task<void> run(dpp::button_click_t event);
    };

} // namespace gb
