//
// Created by ilesik on 9/16/24.
//

#pragma once
#include <src/modules/discord/discord_commands/games/discord_game_command.hpp>

namespace gb {

    /**
     * @brief Represents a command to initiate a Connect Four game within Discord.
     *
     * The `Discord_connect_four_command` class inherits from `Discord_game_command` and is used
     * to register and manage commands related to the Connect Four game.
     */
    class Discord_connect_four_command : public Discord_game_command {
    public:
        /**
         * @brief Constructs a new `Discord_connect_four_command` object.
         *
         * @param name The name of the command.
         * @param dependencies A vector of strings representing dependencies required for the command.
         */
        Discord_connect_four_command(const std::string &name, const std::vector<std::string> &dependencies) :
            Discord_game_command(name, dependencies) {}
    };

} // namespace gb
