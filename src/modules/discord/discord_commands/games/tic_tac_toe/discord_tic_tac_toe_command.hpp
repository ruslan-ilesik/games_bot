//
// Created by ilesik on 7/31/24.
//

#pragma once

#include "../discord_game_command.hpp"

namespace gb {

    /**
     * @class Discord_tic_tac_toe_command
     * @brief An abstract class that handles the Tic-Tac-Toe game command for the Discord bot.
     *
     * This abstract class inherits from Discord_game_command and is responsible for managing
     * the Tic-Tac-Toe game within a Discord server.
     */
    class Discord_tic_tac_toe_command : public Discord_game_command {
    public:
        /**
         * @brief Constructs a Discord_tic_tac_toe_command object.
         *
         * @param name The name of the module.
         * @param dependencies A list of module names that this module depends on.
         */
        Discord_tic_tac_toe_command(const std::string& name, const std::vector<std::string>& dependencies) : Discord_game_command(name, dependencies) {};

    };

} // namespace gb
