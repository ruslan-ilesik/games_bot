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
    enum Direction {UP,DOWN,LEFT,RIGHT};
    class Puzzle_15 {
    private:
        std::random_device _random;
        int _moves_cnt;
        std::array<std::array<unsigned short int, 4>, 4> _field = {
            {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 0}}
        };
        std::array<int, 2> _zero_position = {{3, 3}};

    public:
        Puzzle_15();

        [[nodiscard]] std::vector<Direction> get_possible_moves() const;
        void move(const Direction& d);

        [[nodiscard]] bool is_win() const;

        void shuffle();

        [[nodiscard]] std::array<std::array<unsigned short int, 4>, 4>  get_field() const;

        [[nodiscard]] int get_moves_cnt() const;

        explicit operator std::string() const;
    };
} // Puzzle_15

