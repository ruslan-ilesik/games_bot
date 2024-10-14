//
// Created by rusla on 2/10/2023.
//

#pragma once
#include <random>
#include <array>
#include <vector>

namespace minesweeper_engine {

    /**
     * @brief Enum representing the possible actions in the Minesweeper game.
     */
    enum ACTIONS {
        DIG,                ///< Dig at the specified position.
        PLACE_FLAG,         ///< Place a flag at the specified position.
        REMOVE_FLAG,        ///< Remove a flag from the specified position.
        OPEN_ALL_AROUND,    ///< Open all squares around the specified position.
        REMOVE_FLAG_AND_DIG ///< Remove the flag and dig at the specified position.
    };

    /**
     * @brief Enum representing the possible game states.
     */
    enum STATES {
        WIN,      ///< Player has won the game.
        LOSE,     ///< Player has lost the game.
        CONTINUE  ///< Game is still ongoing.
    };

    /**
     * @brief A class representing a Minesweeper game engine.
     *
     * This class manages the Minesweeper game logic, including board generation,
     * player actions, and game state evaluation.
     */
    class Minesweeper {
    private:
        bool _is_stated = false; ///< Flag indicating if the game has started.
        std::array<std::array<int, 2>, 8> _offsets{{{1, 0}, {1, 1}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {-1, 1}}}; ///< Offset directions for neighboring cells.
        std::random_device _rd; ///< Random device used for generating random positions.

        // 0-8 values represent counts of neighboring mines, 9 - mine, 10 - flag.
        std::vector<std::vector<char>> _board; ///< Internal representation of the Minesweeper board.
        time_t _start_time; ///< Timestamp for the start of the game.

        /**
         * @brief Opens all zero-valued squares around the specified position.
         *
         * Recursively reveals all connected zero-valued squares.
         *
         * @param pos The position to start revealing from.
         * @param fst Flag indicating if this is the initial call.
         */
        void open_all_zeros_around(const std::array<int, 2>& pos, bool fst = true);

    public:
        int level; ///< Difficulty level of the game (1 = Easy, 2 = Medium, 3 = Hard).
        int flags_cnt; ///< Count of remaining flags available for the player.

        // 11 - visible 0 (zero revealed), 12 - cross (incorrect flag).
        std::vector<std::vector<char>> visible_board; ///< Player-visible representation of the game board.

        /**
         * @brief Constructs a new Minesweeper object.
         *
         * Initializes the game board based on the specified difficulty level.
         *
         * @param harness_level The difficulty level of the game (1 = Easy, 2 = Medium, 3 = Hard).
         * @throws std::invalid_argument If the difficulty level is not within the valid range (1-3).
         */
        Minesweeper(int harness_level);

        /**
         * @brief Generates a safe field with mines placed away from the initial position.
         *
         * Ensures the initial move is always safe by avoiding mine placement near the specified position.
         *
         * @param pos The initial position from which mine placement safety is determined.
         */
        void generate_safe_field(std::array<int, 2> pos);

        /**
         * @brief Retrieves the possible move indices (columns or rows).
         *
         * Based on the current state of the board, identifies which columns or rows
         * have potential valid moves available.
         *
         * @param col Optional column index to restrict the search to a specific column. Default is -1 (search all columns).
         * @return A vector of indices representing possible moves.
         */
        std::vector<int> get_possible_moves(int col = -1);

        /**
         * @brief Performs the specified action at the given position.
         *
         * Handles various player actions such as digging, flagging, or removing flags,
         * and updates the game state accordingly.
         *
         * @param action The action to perform (dig, place flag, etc.).
         * @param pos The position at which to perform the action.
         * @return The current game state after performing the action (WIN, LOSE, or CONTINUE).
         */
        STATES make_action(ACTIONS action, std::array<int, 2> pos);

        /**
         * @brief Reveals the entire board at the end of the game.
         *
         * Displays all mines and indicates incorrectly placed flags, ending the game.
         */
        void end_of_game();
    };

} // namespace minesweeper_engine
