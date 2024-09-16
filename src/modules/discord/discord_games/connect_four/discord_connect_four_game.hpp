//
// Created by ilesik on 9/16/24.
//

#pragma once
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief Implementation of the Connect Four game for Discord.
     *
     * The `Discord_connect_four_game` class represents the Connect Four game played within
     * a Discord environment. It extends the `Discord_game` class and handles the game logic,
     * board rendering, and user interactions through Discord buttons.
     */
    class Discord_connect_four_game : public Discord_game {
    private:
        /**
         * @brief The game board represented as a 2D array of strings.
         *
         * Each element in the board represents a cell, with empty cells containing a space (" "),
         * red player cells containing "r", and yellow player cells containing "y".
         */
        std::string _board[6][7] = {{" ", " ", " ", " ", " ", " ", " "}, {" ", " ", " ", " ", " ", " ", " "},
                                   {" ", " ", " ", " ", " ", " ", " "}, {" ", " ", " ", " ", " ", " ", " "},
                                   {" ", " ", " ", " ", " ", " ", " "}, {" ", " ", " ", " ", " ", " ", " "}};

        /**
         * @brief The player tokens representing the two players.
         *
         * The first player is represented by "r" (red), and the second player is represented by "y" (yellow).
         */
        std::string _signs[2] = {"r", "y"};

    public:
        /**
         * @brief Constructs a new `Discord_connect_four_game` object.
         *
         * This constructor initializes the Connect Four game with the provided game data and players.
         *
         * @param _data Reference to the game data required for initialization.
         * @param players A vector of player IDs participating in the game.
         */
        Discord_connect_four_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Gets the image generators for the game.
         *
         * This method provides the image generators used to create visual representations of the game.
         *
         * @return A vector of pairs where each pair contains a string and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Generates an image representing the current game state.
         *
         * This method renders the current state of the Connect Four board as an image.
         *
         * @return A pointer to the generated image.
         */
        Image_ptr generate_image();

        /**
         * @brief Runs the Connect Four game in response to a button click event.
         *
         * This method manages the game flow, including player moves, checking for a win or draw,
         * and updating the game board after each interaction.
         *
         * @param event The button click event triggered by a player.
         * @return A task that handles the asynchronous game execution.
         */
        dpp::task<void> run(dpp::button_click_t event);
    };

} // namespace gb
