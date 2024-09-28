//
// Created by ilesik on 9/26/24.
//

#pragma once
#include <src/games/sudoku/sudoku.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @class Discord_sudoku_game
     * @brief Represents a Sudoku game playable through Discord.
     *
     * This class provides functionality for hosting and managing a Sudoku game
     * on a Discord server. Players can interact with the game using buttons to
     * select cells and place numbers. The game includes support for tracking mistakes
     * and determining the end conditions, such as winning or losing based on mistakes.
     */
    class Discord_sudoku_game : public Discord_game {
        sudoku::Sudoku _engine = sudoku::Sudoku(); /**< The Sudoku game engine instance managing the puzzle. */
        int _state = 0; /**< Represents the current state of the game:
                         * 0 - select column,
                         * 1 - select row,
                         * 2 - select number.
                         */
        std::array<int, 2> _pos; /**< The current position on the Sudoku grid, represented as {row, column}. */
        int _available_mistakes = 3; /**< The number of mistakes a player can make before losing the game. */
        int _timeout = 300; /**< The maximum time in seconds a player can take to make a move before timing out. */
        bool _is_mistake = false; /**< A flag indicating if the last action was a mistake. */

    public:
        /**
         * @brief Constructor for initializing the Sudoku game.
         * @param _data The game data initialization object.
         * @param players A vector of player IDs participating in the game.
         */
        Discord_sudoku_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves a list of image generators associated with the game.
         * @return A vector of pairs containing string descriptions and corresponding image generator functions.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Prepares the Discord message with the current game state and UI components.
         * @param message The message to be updated with game details and interactive elements.
         */
        void prepare_message(dpp::message& message);

        /**
         * @brief Creates an image representing the current state of the Sudoku puzzle.
         * @return A pointer to the generated image.
         */
        Image_ptr create_image();

        /**
         * @brief The main game loop for the Sudoku game.
         *
         * This function handles the game flow, including user inputs, game state updates,
         * and rendering of the game board and components. It awaits user interactions
         * via Discord buttons and updates the game state accordingly.
         *
         * @param sevent The slash command event that initiated the game.
         * @return A task representing the asynchronous execution of the game loop.
         */
        dpp::task<void> run(dpp::slashcommand_t sevent);
    };

} // namespace gb
