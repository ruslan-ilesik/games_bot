//
// Created by ilesik on 9/15/24.
//

#pragma once

#include <src/modules/discord/discord_games/discord_game.hpp>
#include "src/games/chess/chess/chess.h"

namespace chess {

    /**
     * @brief A map linking chess pieces to corresponding emoji names and Discord snowflake IDs.
     *
     * This map holds tuples containing the emoji's name and its Discord snowflake ID for various chess pieces.
     * '.' represents an empty square, while uppercase letters represent white pieces and lowercase letters represent
     * black pieces.
     */
    inline std::map<char, std::tuple<std::string, dpp::snowflake>> emojis = {
        {'.', {"clear", 1015646216509468683}}, {'B', {"B_", 883415614755045397}}, {'K', {"K_", 883415619649806337}},
        {'N', {"N_", 883415619662413854}},     {'P', {"P_", 883415619863740466}}, {'Q', {"Q_", 883415619679182898}},
        {'R', {"R_", 883415619343622176}},     {'b', {"b1", 883415614922838016}}, {'k', {"k1", 883415619301687337}},
        {'n', {"n1", 883415619289112667}},     {'p', {"p1", 883415619624632390}}, {'q', {"q1", 883415619633025104}},
        {'r', {"r1", 883415619733688320}},
    };

} // namespace chess

namespace gb {

    /**
     * @brief Represents a chess game integrated with Discord.
     *
     * This class manages a chess game played in Discord using the dpp library.
     * It handles player actions, board updates, and image generation.
     */
    class Discord_chess_game : public Discord_game {
    private:
        chess::Board _board; ///< The internal chess board state.
        bool _choose_figure = true; ///< Tracks whether the player is currently selecting a figure.
        std::string _selected_figure = ""; ///< Stores the currently selected chess piece.
        std::vector<std::string> _possible_places_to_go; ///< A list of possible moves for the selected piece.
        bool _next = false; ///< Indicates whether the select turn menu should show second page.
        int _moves_amount = 0; ///< The number of moves made in the game.
        bool is_view = false; ///< Indicates if the current game state is being viewed (as opposed to played).

        /**
         * @brief Runs the main loop of the chess game, handling button clicks and moves.
         *
         * @param event The button click event that triggered the function.
         * @param timeout The amount of time (in seconds) before a move times out. Defaults to 60 seconds.
         * @return A task representing the asynchronous execution of the game.
         */
        dpp::task<void> run(dpp::button_click_t event, int timeout = 60);


    public:
        /**
         * @brief Converts chess board coordinates (e.g., "e2") into numerical grid positions.
         *
         * @param to_convert The chess notation coordinates (e.g., "e2").
         * @return A Vector2i containing the numeric row and column positions.
         */
        static Vector2i chess_board_cords_to_numbers(std::string to_convert);

        /**
         * @brief Generates an image representing the current state of the chess board.
         *
         * @return A shared pointer to the generated image.
         */
        Image_ptr generate_image();

        /**
         * @brief Constructs a Discord chess game with the given initialization data and players.
         *
         * @param _data Game initialization data such as game settings.
         * @param players A vector of Discord snowflake IDs representing the players.
         */
        Discord_chess_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves the image generators used to render the chess board.
         *
         * @return A vector of pairs, where each pair consists of a string identifier and the corresponding image
         * generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();
    };

} // namespace gb
