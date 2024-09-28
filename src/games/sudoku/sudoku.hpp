//
// Created by rusla on 3/25/2023.
//
// Based on https://github.com/vaithak/Sudoku-Generator/blob/master/sudokuGen.cpp
//

#pragma once
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace sudoku {

    /**
     * @class Sudoku
     * @brief A class for generating, solving, and managing Sudoku puzzles.
     *
     * This class provides methods for creating a Sudoku puzzle, solving it, and
     * validating the correctness of the puzzle. It also supports functionality
     * for setting the difficulty level, printing the grid, and handling player interactions.
     */
    class Sudoku {
    private:
        int _grid[9][9];            /**< The current state of the Sudoku grid. */
        int _soln_grid[9][9];       /**< The solution state of the Sudoku grid. */
        int _guess_num[9];          /**< An array of numbers used for generating the puzzle. */
        int _grid_pos[81];          /**< An array representing positions in the grid. */
        int _difficulty_level;      /**< The calculated difficulty level of the puzzle. */
        bool _grid_status;          /**< Status flag for grid validation. */

    public:
        /**
         * @brief Default constructor for Sudoku class.
         *
         * Initializes a Sudoku puzzle with a blank grid and prepares for puzzle generation.
         */
        Sudoku();

        /**
         * @brief Constructs a Sudoku puzzle from a given grid string.
         *
         * @param grid_str The string representation of the Sudoku grid.
         * @param row_major If true, the grid is in row-major order, otherwise it's in column-major order.
         */
        Sudoku(std::string grid_str, bool row_major = true);

        /**
         * @brief Fills a 3x3 diagonal box in the grid.
         *
         * @param idx The index of the diagonal box to fill.
         */
        void fill_empty_diagonal_box(int idx);

        /**
         * @brief Creates a seed for the Sudoku puzzle.
         *
         * This method generates a fully solved Sudoku grid and stores it as the solution.
         */
        void create_seed();

        /**
         * @brief Prints the current state of the grid to the console.
         */
        void print_grid();

        /**
         * @brief Solves the current Sudoku puzzle.
         *
         * @return True if the puzzle is solved, otherwise false.
         */
        bool solve_grid();

        /**
         * @brief Returns the grid as a string in row-major order.
         *
         * @return A string representing the current state of the grid.
         */
        std::string get_grid();

        /**
         * @brief Returns the current state of the grid as a 2D array.
         *
         * @return A 2D array representing the current state of the grid.
         */
        std::array<std::array<int, 9>, 9> get_field();

        /**
         * @brief Counts the number of solutions for the current grid configuration.
         *
         * @param number A reference to store the number of solutions found.
         */
        void count_soln(int &number);

        /**
         * @brief Generates a Sudoku puzzle by removing cells from the solved grid.
         */
        void gen_puzzle();

        /**
         * @brief Verifies the validity of the custom grid passed during initialization.
         *
         * @return True if the grid is valid, otherwise false.
         */
        bool verify_grid_status();

        /**
         * @brief Calculates the difficulty level of the current puzzle.
         */
        void calculate_difficulty();

        /**
         * @brief Calculates the branch difficulty score of the current grid.
         *
         * @return An integer representing the branch difficulty score.
         */
        int branch_difficulty_score();

        /**
         * @brief Ends the game and fills the grid with the solution.
         */
        void end_game();

        /**
         * @brief Checks if the current grid matches the solution.
         *
         * @return True if the player has successfully completed the puzzle, otherwise false.
         */
        bool check_win();

        /**
         * @brief Places a value at a specified position in the grid.
         *
         * @param pos A pair of integers representing the row and column.
         * @param value The value to be placed at the specified position.
         * @return True if the value matches the solution, otherwise false.
         */
        bool place(const std::array<int, 2> &pos, int value);
    };

} // namespace sudoku
