//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "src/module/module.hpp"
#include "src/modules/discord/discord_games/discord_game.hpp"
#include "src/modules/discord/discord_interactions_handler/discord_button_click_handler/discord_button_click_handler.hpp"
#include <src/modules/discord/discord_commands/discord_general_command.hpp>

namespace gb {

/**
 * @class Discord_game_command
 * @brief A class that manages game-specific commands within a Discord bot.
 *
 * This class extends the functionality of the Discord_general_command class,
 * adding features that are necessary for handling games on a Discord server.
 */
class Discord_game_command : public Discord_general_command {
protected:
    /**
     * @brief Pointer to the database module.
     */
    Database_ptr _db;

    /**
     * @brief Pointer to the games manager module.
     */
    Discord_games_manager_ptr _games_manager;

    /**
     * @brief Pointer to the image processing module.
     */
    Image_processing_ptr _image_processing;

    /**
     * @brief Pointer to the Discord button click handler module.
     */
    Discord_button_click_handler_ptr _button_click_handler;

    /**
     * @brief Pointer to the Discord achievements processing module.
     */
    Discord_achievements_processing_ptr _achievements_processing;

    /**
     * @brief Title of the game lobby.
     */
    std::string lobby_title;

    /**
     * @brief Description of the game lobby.
     */
    std::string lobby_description;

    /**
     * @brief URL of the image to display in the game lobby.
     */
    std::string lobby_image_url;

public:

    /**
     * @struct Lobby_return
     * @brief Structure to store the result of the lobby function.
     */
    struct Lobby_return {
        bool is_timeout;                /**< Indicates if the lobby has timed out. */
        dpp::button_click_t event;      /**< Stores the button click event data. */
        std::vector<dpp::snowflake> players; /**< List of players in the lobby. */
    };

    /**
     * @brief Constructs a Discord_game_command object.
     *
     * @param name The name of the module.
     * @param dependencies A list of module names that this module depends on.
     */
    Discord_game_command(const std::string& name, const std::vector<std::string>& dependencies);

    /**
     * @brief Initializes game-specific data.
     *
     * @param game_name The name of the game to initialize data for.
     * @return Game_data_initialization Struct containing initialized game data.
     */
    Game_data_initialization get_game_data_initialization(const std::string &game_name) const;

    /**
     * @brief Handles the creation and management of a game lobby.
     *
     * @param event The slash command event that triggered the lobby.
     * @param players A list of players to invite to the lobby.
     * @param host The host of the lobby.
     * @param players_amount The total number of players required.
     * @return dpp::task<Lobby_return> A task returning the lobby outcome.
     */
    dpp::task<Lobby_return> lobby(const dpp::slashcommand_t& event, std::vector<dpp::snowflake> players, const dpp::snowflake& host, unsigned int players_amount);

    /**
     * @brief Initializes the module with its dependencies.
     *
     * @param modules A map of module names to module pointers.
     */
    virtual void init(const Modules& modules);

    /**
     * @brief Runs the module, typically handling any ongoing processes or events.
     */
    virtual void run();
};

} // gb
