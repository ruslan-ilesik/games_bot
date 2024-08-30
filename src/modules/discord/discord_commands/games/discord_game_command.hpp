//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "src/module/module.hpp"
#include "src/modules/discord/discord_games/discord_game.hpp"
#include "src/modules/discord/discord_interactions_handler/discord_button_click_handler/discord_button_click_handler.hpp"

#include <src/modules/discord/discord_commands/discord_general_command.hpp>


namespace gb {

    class Discord_game_command : public  Discord_general_command {
    protected:
        Database_ptr _db;
        Discord_bot_ptr _bot;
        Discord_games_manager_ptr _games_manager;
        Image_processing_ptr _image_processing;
        Discord_button_click_handler_ptr _button_click_handler;
        Discord_achievements_processing_ptr _achievements_processing;

        std::string lobby_title;
        std::string lobby_description;
        std::string lobby_image_url;
    public:

        struct Lobby_return {
            bool is_timeout;
            dpp::button_click_t event;
            std::vector<dpp::snowflake> players;
        };

        Discord_game_command(const std::string& name, const std::vector<std::string>& dependencies);

        Game_data_initialization get_game_data_initialization(const std::string &game_name) const;

        // true in first argument if fail (timeout or error).
        dpp::task<Lobby_return> lobby(const dpp::slashcommand_t& event, std::vector<dpp::snowflake> players, const dpp::snowflake& host, unsigned int players_amount);

        virtual void init(const Modules& modules);

        virtual void run();
    };

} // gb

