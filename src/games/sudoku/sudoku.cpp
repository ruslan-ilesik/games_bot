//
// Created by rusla on 3/25/2023.
//

#include "./sudoku.hpp"

#include <chrono>
#include <random>

using namespace sudoku;
// START: Get grid as string in row major order
std::string Sudoku::get_grid() {
    std::string s = "";
    for (int row_num = 0; row_num < 9; ++row_num) {
        for (int col_num = 0; col_num < 9; ++col_num) {
            s = s + std::to_string(_grid[row_num][col_num]);
        }
    }

    return s;
}
// END: Get grid as string in row major order


// START: Generate random number
int gen_rand_num(int max_limit) { return rand() % max_limit; }
// END: Generate random number


// START: Helper functions for solving grid
bool find_unassigned_location(int grid[9][9], int &row, int &col) {
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (grid[row][col] == 0)
                return true;
        }
    }

    return false;
}

bool used_in_row(int grid[9][9], int row, int num) {
    for (int col = 0; col < 9; col++) {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool used_in_col(int grid[9][9], int col, int num) {
    for (int row = 0; row < 9; row++) {
        if (grid[row][col] == num)
            return true;
    }

    return false;
}

bool used_in_box(int grid[9][9], int box_start_row, int box_start_col, int num) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row + box_start_row][col + box_start_col] == num)
                return true;
        }
    }

    return false;
}

bool is_safe(int grid[9][9], int row, int col, int num) {
    return !used_in_row(grid, row, num) && !used_in_col(grid, col, num) &&
           !used_in_box(grid, row - row % 3, col - col % 3, num);
}
// END: Helper functions for solving grid


// START: Create seed grid
void Sudoku::fill_empty_diagonal_box(int idx) {
    int start = idx * 3;
    std::shuffle(this->_guess_num, (this->_guess_num) + 9,
                 std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            this->_grid[start + i][start + j] = _guess_num[i * 3 + j];
        }
    }
}

void Sudoku::create_seed() {
    /* Fill diagonal boxes to form:
        x | . | .
        . | x | .
        . | . | x
    */
    this->fill_empty_diagonal_box(0);
    this->fill_empty_diagonal_box(1);
    this->fill_empty_diagonal_box(2);

    /* Fill the remaining blocks:
        x | x | x
        x | x | x
        x | x | x
    */
    this->solve_grid(); // TODO: not truly random, but still good enough because we generate random diagonals.

    // Saving the solution grid
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            this->_soln_grid[i][j] = this->_grid[i][j];
        }
    }
}
// END: Create seed grid


// START: Intialising
Sudoku::Sudoku() {

    // initialize difficulty level
    this->_difficulty_level = 0;

    // Randomly shuffling the array of removing grid positions
    for (int i = 0; i < 81; i++) {
        this->_grid_pos[i] = i;
    }

    std::shuffle(this->_grid_pos, (this->_grid_pos) + 81,
                 std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

    // Randomly shuffling the guessing number array
    for (int i = 0; i < 9; i++) {
        this->_guess_num[i] = i + 1;
    }

    std::shuffle(this->_guess_num, (this->_guess_num) + 9,
                 std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

    // Initialising the grid
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            this->_grid[i][j] = 0;
        }
    }

    _grid_status = true;
}
// END: Initialising


// START: Custom Initialising with grid passed as argument
Sudoku::Sudoku(std::string grid_str, bool row_major) {
    if (grid_str.length() != 81) {
        _grid_status = false;
        return;
    }

    // First pass: Check if all cells are valid
    for (int i = 0; i < 81; ++i) {
        int curr_num = grid_str[i] - '0';
        if (!((curr_num == 0) || (curr_num > 0 && curr_num < 10))) {
            _grid_status = false;
            return;
        }

        if (row_major)
            _grid[i / 9][i % 9] = curr_num;
        else
            _grid[i % 9][i / 9] = curr_num;
    }

    // Second pass: Check if all columns are valid
    for (int col_num = 0; col_num < 9; ++col_num) {
        bool nums[10] = {false};
        for (int row_num = 0; row_num < 9; ++row_num) {
            int curr_num = _grid[row_num][col_num];
            if (curr_num != 0 && nums[curr_num] == true) {
                _grid_status = false;
                return;
            }
            nums[curr_num] = true;
        }
    }

    // Third pass: Check if all rows are valid
    for (int row_num = 0; row_num < 9; ++row_num) {
        bool nums[10] = {false};
        for (int col_num = 0; col_num < 9; ++col_num) {
            int curr_num = _grid[row_num][col_num];
            if (curr_num != 0 && nums[curr_num] == true) {
                _grid_status = false;
                return;
            }
            nums[curr_num] = true;
        }
    }

    // Fourth pass: Check if all blocks are valid
    for (int block_num = 0; block_num < 9; ++block_num) {
        bool nums[10] = {false};
        for (int cell_num = 0; cell_num < 9; ++cell_num) {
            int curr_num =
                _grid[((int) (block_num / 3)) * 3 + (cell_num / 3)][((int) (block_num % 3)) * 3 + (cell_num % 3)];
            if (curr_num != 0 && nums[curr_num] == true) {
                _grid_status = false;
                return;
            }
            nums[curr_num] = true;
        }
    }

    // Randomly shuffling the guessing number array
    for (int i = 0; i < 9; i++) {
        this->_guess_num[i] = i + 1;
    }

    std::shuffle(this->_guess_num, (this->_guess_num) + 9,
                 std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));

    _grid_status = true;
}
// END: Custom Initialising


// START: Verification status of the custom grid passed
bool Sudoku::verify_grid_status() { return _grid_status; }
// END: Verification of the custom grid passed


// START: Printing the grid
void Sudoku::print_grid() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (_grid[i][j] == 0)
                std::cout << ".";
            else
                std::cout << _grid[i][j];
            std::cout << "|";
        }
        std::cout << std::endl;
    }

    std::cout << "\nDifficulty of current sudoku(0 being easiest): " << this->_difficulty_level;
    std::cout << std::endl;
}
// END: Printing the grid


// START: Modified Sudoku solver
bool Sudoku::solve_grid() {
    int row, col;

    // If there is no unassigned location, we are done
    if (!find_unassigned_location(this->_grid, row, col))
        return true; // success!

    // Consider digits 1 to 9
    for (int num = 0; num < 9; num++) {
        // if looks promising
        if (is_safe(this->_grid, row, col, this->_guess_num[num])) {
            // make tentative assignment
            this->_grid[row][col] = this->_guess_num[num];

            // return, if success, yay!
            if (solve_grid())
                return true;

            // failure, unmake & try again
            this->_grid[row][col] = 0;
        }
    }

    return false; // this triggers backtracking
}
// END: Modified Sudoku Solver


// START: Check if the grid is uniquely solvable
void Sudoku::count_soln(int &number) {
    int row, col;

    if (!find_unassigned_location(this->_grid, row, col)) {
        number++;
        return;
    }


    for (int i = 0; i < 9 && number < 2; i++) {
        if (is_safe(this->_grid, row, col, this->_guess_num[i])) {
            this->_grid[row][col] = this->_guess_num[i];
            count_soln(number);
        }

        this->_grid[row][col] = 0;
    }
}
// END: Check if the grid is uniquely solvable


// START: Gneerate puzzle
void Sudoku::gen_puzzle() {
    for (int i = 0; i < 81; i++) {
        int x = (this->_grid_pos[i]) / 9;
        int y = (this->_grid_pos[i]) % 9;
        int temp = this->_grid[x][y];
        this->_grid[x][y] = 0;

        // If now more than 1 solution , replace the removed cell back.
        int check = 0;
        count_soln(check);
        if (check != 1) {
            this->_grid[x][y] = temp;
        }
    }
}
// END: Generate puzzle


// START: Calculate branch difficulty score
int Sudoku::branch_difficulty_score() {
    int empty_positions = -1;
    int temp_grid[9][9];
    int sum = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            temp_grid[i][j] = this->_grid[i][j];
        }
    }

    while (empty_positions != 0) {
        std::vector<std::vector<int>> empty;

        for (int i = 0; i < 81; i++) {
            if (temp_grid[(int) (i / 9)][(int) (i % 9)] == 0) {
                std::vector<int> temp;
                temp.push_back(i);

                for (int num = 1; num <= 9; num++) {
                    if (is_safe(temp_grid, i / 9, i % 9, num)) {
                        temp.push_back(num);
                    }
                }

                empty.push_back(temp);
            }
        }

        if (empty.size() == 0) {
            // std::cout<<"Hello: "<<sum<<std::endl;
            return sum;
        }

        int min_index = 0;

        int check = empty.size();
        for (int i = 0; i < check; i++) {
            if (empty[i].size() < empty[min_index].size())
                min_index = i;
        }

        int branchFactor = empty[min_index].size();
        int rowIndex = empty[min_index][0] / 9;
        int colIndex = empty[min_index][0] % 9;

        temp_grid[rowIndex][colIndex] = this->_soln_grid[rowIndex][colIndex];
        sum = sum + ((branchFactor - 2) * (branchFactor - 2));

        empty_positions = empty.size() - 1;
    }

    return sum;
}
// END: Finish branch difficulty score


// START: Calculate difficulty level of current grid
void Sudoku::calculate_difficulty() {
    int B = branch_difficulty_score();
    int empty_cells = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (this->_grid[i][j] == 0)
                empty_cells++;
        }
    }

    this->_difficulty_level = B * 100 + empty_cells;
}
// END: calculating difficulty level


std::array<std::array<int, 9>, 9> Sudoku::get_field() {
    std::array<std::array<int, 9>, 9> arr2; // std::array of std::array of int

    for (int i = 0; i < 9; i++) {
        std::copy(this->_grid[i], this->_grid[i] + 9, arr2[i].begin());
    }
    return arr2;
}

bool Sudoku::place(const std::array<int, 2> &pos, int value) {
    // std::cout << this->solnGrid[pos[0]][pos[1]] << " " << pos[0] << "," << pos[1]<<'\n';
    if (value == this->_soln_grid[pos[0]][pos[1]]) {
        this->_grid[pos[0]][pos[1]] = value;
        return true;
    }
    return false;
}

void Sudoku::end_game() {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            this->_grid[y][x] = this->_soln_grid[y][x];
        }
    }
}

bool Sudoku::check_win() {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            if (this->_grid[y][x] != this->_soln_grid[y][x]) {
                return false;
            }
        }
    }
    return true;
}
