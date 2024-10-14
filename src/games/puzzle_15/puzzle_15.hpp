//
// Created by rusla on 5/24/2024.
//

#pragma once

#include <random>
#include <array>
#include <stdexcept>
#include <string>
#include <vector>

namespace puzzle_15 {

    /**
     * @brief Enum representing the possible move directions in the Puzzle 15 game.
     *
     * The four possible directions are:
     * - UP: Moves the blank tile up.
     * - DOWN: Moves the blank tile down.
     * - LEFT: Moves the blank tile left.
     * - RIGHT: Moves the blank tile right.
     */
    enum Direction { UP, DOWN, LEFT, RIGHT };

    /**
     * @brief Class representing the Puzzle 15 game.
     *
     * The Puzzle 15 game consists of a 4x4 grid with tiles numbered from 1 to 15 and one blank space.
     * The goal is to arrange the tiles in numerical order by sliding tiles into the blank space.
     */
    class Puzzle_15 {
    private:
        std::random_device _random; /**< Random device used for shuffling the puzzle. */
        int _moves_cnt; /**< Count of moves made by the player. */
        std::array<std::array<unsigned short int, 4>, 4> _field = {
            {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 0}}
        }; /**< The current state of the puzzle grid. */
        std::array<int, 2> _zero_position = {{3, 3}}; /**< The position of the blank tile (0). */

    public:
        /**
         * @brief Constructs a new Puzzle 15 game with the initial solved state.
         */
        Puzzle_15();

        /**
         * @brief Returns the possible moves the player can make based on the current position of the blank tile.
         *
         * @return A vector of possible directions (UP, DOWN, LEFT, RIGHT) where the player can move the blank tile.
         */
        [[nodiscard]] std::vector<Direction> get_possible_moves() const;

        /**
         * @brief Moves the blank tile in the specified direction.
         *
         * Throws an exception if the move is invalid (e.g., moving out of bounds).
         *
         * @param d The direction in which to move the blank tile.
         */
        void move(const Direction& d);

        /**
         * @brief Checks if the puzzle is in the winning state.
         *
         * The puzzle is won when all tiles are in numerical order from 1 to 15, and the blank tile is in the bottom-right corner.
         *
         * @return true if the puzzle is solved, false otherwise.
         */
        [[nodiscard]] bool is_win() const;

        /**
         * @brief Randomly shuffles the puzzle.
         *
         * This function shuffles the puzzle by making 81 random valid moves, ensuring the puzzle is solvable.
         */
        void shuffle();

        /**
         * @brief Returns the current state of the puzzle grid.
         *
         * @return A 4x4 array representing the current arrangement of the tiles.
         */
        [[nodiscard]] std::array<std::array<unsigned short int, 4>, 4> get_field() const;

        /**
         * @brief Returns the number of moves made by the player.
         *
         * @return The count of moves.
         */
        [[nodiscard]] int get_moves_cnt() const;

        /**
         * @brief Converts the puzzle grid to a string representation.
         *
         * This operator provides a formatted string that displays the current state of the puzzle grid, useful for debugging or display purposes.
         *
         * @return A string representing the current puzzle grid.
         */
        explicit operator std::string() const;
    };
} // namespace puzzle_15
