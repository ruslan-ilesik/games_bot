//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "./discord_tic_tac_toe_command.hpp"
#include "src/modules/discord/discord/discord.hpp"
#include "src/modules/discord/discord_command_handler/discord_command_handler.hpp"

namespace gb {

    class Discord_tic_tac_toe_command_impl: public Discord_tic_tac_toe_command {
        Discord_command_handler_ptr _command_handler;
        Discord_ptr _discord;

    public:
        Discord_tic_tac_toe_command_impl();

        void innit(const Modules &modules) override{
            Discord_game_command::innit(modules);
            this->_command_handler = std::static_pointer_cast<Discord_command_handler>(
                modules.at("discord_command_handler"));
            this->_discord = std::static_pointer_cast<Discord>(modules.at("discord"));
            this->_button_click_handler = std::static_pointer_cast<Discord_button_click_handler>(modules.at("discord_button_click_handler"));

            _bot->add_pre_requirement([this]() {
                dpp::slashcommand command("tic_tac_toe", "Command to start tic tac toe game", _bot->get_bot()->me.id);
                command.add_option(
                    dpp::command_option(dpp::co_user, "player" ,"Player to play with.", false)
                );
                _command_handler->register_command(_discord->create_discord_command(
                    command,
                    [this](const dpp::slashcommand_t& event)-> dpp::task<void>{
                        this->command_run();
                        std::vector<dpp::snowflake> players = {};
                        auto parameter = event.get_parameter("player");
                        if (std::holds_alternative<dpp::snowflake>(parameter)) {
                            players.push_back(std::get<dpp::snowflake>(parameter));
                        }
                        auto r = co_await this->lobby(event,players,event.command.usr.id,2);
                        this->command_finished();

                        co_return ;
                    },
                    {
                        "__**Rules**__:\n1. The game is played on a grid that's 3 squares by 3 squares."
                        "\n\n2. You are X, your friend (or the computer in this case) is O. Players take turns putting"
                        " their marks in empty squares.\n\n3. The first player to get 3 of her marks in a row (up, down,"
                        " across, or diagonally) is the winner.\n\n4. When all 9 squares are full, the game is over. If "
                        "no player has 3 marks in a row, the game ends in a tie."
                        "\n\n\n__**How does it works in bot?**__\n"
                        "Bot representing game field in buttons, just click on button where you would like to place your sign.",
                        {"game","multiplayer"}
                    }
                    )
                );
            });
        }
    };

    extern "C" Module_ptr create();

} // gb


