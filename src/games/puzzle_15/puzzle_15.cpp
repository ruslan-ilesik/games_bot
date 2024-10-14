//
// Created by rusla on 5/24/2024.
//

#include "puzzle_15.hpp"

namespace puzzle_15 {
    Puzzle_15::Puzzle_15() {_moves_cnt=0;}

    std::vector<Direction> Puzzle_15::get_possible_moves() const {
        std::vector<Direction> r;
        if (_zero_position[0] != 0) {
            r.push_back(RIGHT);
        }
        if (_zero_position[1] != 0) {
            r.push_back(DOWN);
        }
        if (_zero_position[0] != 3) {
            r.push_back(LEFT);
        }
        if (_zero_position[1] != 3) {
            r.push_back(UP);
        }
        return r;
    }

    void Puzzle_15::move(const Direction &d) {
        auto directions = get_possible_moves();
        if (std::ranges::find(directions, d) == directions.end()) {
            throw std::invalid_argument("Invalid move");
        }
        _moves_cnt++;
        switch (d) {
            case UP:
                _zero_position[1]++;
                std::swap(_field[_zero_position[1]][_zero_position[0]], _field[_zero_position[1] - 1][_zero_position[0]]);
                break;

            case DOWN:
                _zero_position[1]--;
                std::swap(_field[_zero_position[1]][_zero_position[0]], _field[_zero_position[1] + 1][_zero_position[0]]);
                break;

            case RIGHT:
                _zero_position[0]--;
                std::swap(_field[_zero_position[1]][_zero_position[0]], _field[_zero_position[1]][_zero_position[0] + 1]);
                break;

            case LEFT:
                _zero_position[0]++;
                std::swap(_field[_zero_position[1]][_zero_position[0]], _field[_zero_position[1]][_zero_position[0] - 1]);
                break;
        }
    }

    bool Puzzle_15::is_win() const {
        unsigned short int expected_value = 1;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i == 3 && j == 3) {
                    return _field[i][j] == 0;
                }
                if (_field[i][j] != expected_value) {
                    return false;
                }
                ++expected_value;
            }
        }
        return true;
    }

    void Puzzle_15::shuffle() {
        for (int i = 0; i <= 81; i++) {
            auto moves = get_possible_moves();
            std::uniform_int_distribution<int> dist(0, moves.size()-1);
            auto my_move = moves[dist(_random)];
            move(my_move);
        }
        _moves_cnt-=82;
    }

    std::array<std::array<unsigned short int, 4>, 4> Puzzle_15::get_field() const {
        return _field;
    }

    int Puzzle_15::get_moves_cnt() const {
        return _moves_cnt;
    }

    Puzzle_15::operator std::string() const {
        std::string str;
        for (auto &i: _field) {
            for (auto &j: i) {
                str += std::to_string(j);
                if (j < 10) {
                    str += " ";
                }
                str += " | ";
            }
            str += "\n";
        }
        return str;
    }
} // Puzzle_15
