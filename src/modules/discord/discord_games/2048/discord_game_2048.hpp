//
// Created by ilesik on 9/29/24.
//

#pragma once
#include <random>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief Class representing the 2048 game within a Discord environment.
     *
     * This class manages the logic of a 2048 game, including game setup, player interaction,
     * board manipulation, and win/loss conditions. The game is run through Discord's slash commands
     * and button interactions, with real-time gameplay rendered using images.
     */
    class Discord_game_2048 : public Discord_game {
        unsigned int _board[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}; ///< The game board for 2048.
        std::vector<std::array<int, 2>> _clear_places; ///< Stores the positions of empty spaces on the board.
        std::map<char, bool> _possible_moves; ///< Tracks possible moves: w (up), s (down), a (left), d (right).
        std::default_random_engine _rand_obj; ///< Random engine used for generating game states.

    public:
        /**
         * @brief Constructs a new `Discord_game_2048` object.
         *
         * @param _data Reference to the game initialization data.
         * @param players List of player IDs involved in the game.
         */
        Discord_game_2048(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves the image generators used to display the game state.
         *
         * @return A vector of image generators paired with their string identifiers.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Runs the game loop, processing player inputs and game events.
         *
         * @param sevent The slash command event that triggered the game start.
         * @return dpp::task<void> Coroutine representing the game execution.
         */
        dpp::task<void> run(dpp::slashcommand_t sevent);

        /**
         * @brief Creates an image of the current game state.
         *
         * @return Image_ptr Pointer to the generated image.
         */
        Image_ptr create_image();

        /**
         * @brief Prepares the Discord message that shows the current game state.
         *
         * @param message The message to be prepared with the game state and controls.
         */
        void prepare_message(dpp::message& message);

        /**
         * @brief Handles the "up" movement on the board.
         *
         * @param a The game board to be manipulated.
         */
        void up(unsigned int a[4][4]);

        /**
         * @brief Handles the "down" movement on the board.
         *
         * @param a The game board to be manipulated.
         */
        void down(unsigned int a[4][4]);

        /**
         * @brief Handles the "left" movement on the board.
         *
         * @param a The game board to be manipulated.
         */
        void left(unsigned int a[4][4]);

        /**
         * @brief Handles the "right" movement on the board.
         *
         * @param a The game board to be manipulated.
         */
        void right(unsigned int a[4][4]);

        /**
         * @brief Finds and stores the empty spaces on the game board.
         */
        void find_clear();

        /**
         * @brief Determines and stores the possible moves based on the current board state.
         */
        void get_possible_moves();

        /**
         * @brief Places a new tile (either 2 or 4) at a random empty location on the board.
         */
        void new_peace();
    };

} // namespace gb
