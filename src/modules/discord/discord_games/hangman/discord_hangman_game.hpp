//
// Created by ilesik on 10/3/24.
//

#pragma once
#include <src/games/hangman/hangman.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief A class representing a Discord-based Hangman game.
     *
     * This class manages the Hangman game flow within Discord, including
     * player state, button interactions, message updates, and image rendering.
     * It integrates the core hangman engine with Discord-specific behavior.
     */
    class Discord_hangman_game : public Discord_game {
        std::map<dpp::snowflake, dpp::message> _messages; ///< Cached per-player messages.
        std::mutex _mutex; ///< Mutex protecting shared game state.
        std::map<dpp::snowflake, hangman::Player_ptr> _hangman_rel; ///< Mapping between Discord users and hangman players.
        std::map<dpp::snowflake, int> _users_buttons_page; ///< Tracks pagination state for user buttons.
        hangman::Hangman _game = {}; ///< Core hangman game engine.
        std::string _word; ///< The secret word used in the game.
        dpp::message _message; ///< Main game message.

    public:
        /**
         * @brief Constructs a new Discord Hangman Game.
         *
         * @param _data Reference to the game data initialization structure.
         * @param players List of players participating in the game.
         */
        Discord_hangman_game(Game_data_initialization &_data,
                             const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves the list of image generators used by this game.
         *
         * @return A vector of pairs containing generator names and generator functions.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Prepares a Discord message for a specific player.
         *
         * This function updates message content, components, and attachments
         * according to the current game state for the given player.
         *
         * @param message The message object to prepare.
         * @param player The Discord user ID of the player.
         */
        void prepare_message(dpp::message &message, const dpp::snowflake &player);

        /**
         * @brief Creates an image representing the current hangman state for a player.
         *
         * @param player The Discord user ID of the player.
         * @return Pointer to the generated image.
         */
        Image_ptr create_image(const dpp::snowflake& player);

        /**
         * @brief Initializes and starts the internal hangman game state.
         *
         * Sets up the word, players, and any required initial data.
         */
        void start();

        /**
         * @brief Produces a JSON string describing the game results.
         *
         * Typically used as additional data passed to Discord_game::game_stop().
         *
         * @return JSON string with final game results.
         */
        std::string results_json();

        /**
         * @brief Handles the per-player interaction loop.
         *
         * Awaits button interactions for a specific player and updates the
         * game state accordingly.
         *
         * @param player The Discord user ID of the player.
         * @param button_click_awaitable Awaitable button click event.
         * @return Coroutine task representing the asynchronous operation.
         */
        dpp::task<void> per_player_run(
            dpp::snowflake player,
            dpp::task<Button_click_return>& button_click_awaitable);

        /**
         * @brief Main slash-command entry point for the Hangman game.
         *
         * Starts the game session triggered by a slash command and manages
         * the primary game loop.
         *
         * @param sevent The slash command event.
         * @return Coroutine task representing the asynchronous execution.
         */
        dpp::task<void> run(dpp::slashcommand_t sevent);

        /**
         * @brief Handles button click interactions for an existing game.
         *
         * Processes button input and returns serialized result data that
         * may be forwarded to Discord_game::game_stop().
         *
         * @param event The button click event.
         * @return Coroutine resolving to JSON result string.
         */
        dpp::task<std::string> run_btn(dpp::button_click_t event);
    };

} // namespace gb