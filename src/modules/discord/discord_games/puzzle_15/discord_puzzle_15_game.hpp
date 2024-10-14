//
// Created by ilesik on 10/14/24.
//

#pragma once
#include <src/games/puzzle_15/puzzle_15.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief A class representing a Discord-based Puzzle 15 game.
     *
     * This class manages the flow of a Puzzle 15 game played within a Discord channel.
     * It handles user interactions, game state, and visual representations through Discord.
     */
    class Discord_puzzle_15_game : public Discord_game {
        puzzle_15::Puzzle_15 _engine; /**< The Puzzle 15 game engine instance. */

    public:
        /**
         * @brief Constructs a new Discord Puzzle 15 Game.
         *
         * @param _data Reference to the game data initialization object.
         * @param players List of players participating in the game.
         */
        Discord_puzzle_15_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Prepares the Discord message to be sent or updated with the current game state.
         *
         * Updates the provided Discord message object with the current state of the Puzzle 15 board.
         *
         * @param message The Discord message object to be updated.
         */
        void prepare_message(dpp::message& message);

        /**
         * @brief Creates and returns an image representation of the current Puzzle 15 game board.
         *
         * @return A pointer to the generated image representing the game state.
         */
        Image_ptr create_image();

        /**
         * @brief The main game loop, executed as a coroutine.
         *
         * Manages the game flow, processes player input, and updates the game state. This function
         * runs asynchronously and is triggered by a Discord slash command event.
         *
         * @param sevent The slash command event that triggered the game.
         * @return A dpp::task<void> representing the asynchronous execution of the game loop.
         */
        dpp::task<void> run(dpp::slashcommand_t sevent);

        /**
         * @brief Retrieves a list of image generators for the game.
         *
         * @return A vector of pairs where each pair consists of a string identifier and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();
    };

} // namespace gb
