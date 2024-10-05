//
// Created by rusla on 4/10/2024.
//

#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <string>
#include <vector>

#include "hangman.hpp"


namespace hangman {
    inline constexpr unsigned int min_word_size = 6;
    inline auto words = []() {
        std::vector<std::string> words;
        std::string filename = "./data/english_words.txt";
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string word;
            while (std::getline(file, word)) {
                while (word[word.size() - 1] == '\n' || word[word.size() - 1] == '\r') {
                    word = word.substr(0, word.size() - 1);
                }
                if (word.size() >= min_word_size) {
                    words.push_back(word);
                }
            }
            file.close();
        } else {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
        }
        return words;
    }();

    std::string get_random_word();


    class Player {
    private:
        std::vector<char> word_to_guess;
        std::vector<char> guessed_word;
        std::vector<char> unused_characters;
        std::vector<char> used_characters = {};
        bool is_elimin = false;
        explicit Player(const std::string &word);

    public:
        unsigned int error_cnt = 0;
        unsigned int moves_cnt = 0;

        // we assume that u give us word from words list
        static std::shared_ptr<Player> create(const std::string &word);


        // returns if player has ended the game or still alive (true -> ended)
        bool make_move(char move);
        bool is_finished() const;
        bool is_win() const;
        void eliminate();
        bool is_eliminated() const;

        std::vector<char> get_word_to_guess();

        std::vector<char> get_unused_characters();

        std::vector<char> get_used_characters();

        std::vector<char> get_guessed_word();
    };

    typedef std::shared_ptr<Player> Player_ptr;

    class Hangman {
    private:
        std::vector<Player_ptr> players;

    public:
        explicit Hangman(const std::vector<Player_ptr> &players);
        Hangman() = default;
        // returns if player has ended the game or still alive (true -> ended)
        bool make_move(const Player_ptr &player, char move);

        std::vector<Player_ptr> get_players();

        bool is_game_finished();
        bool is_all_eliminated();

        // returns best players (one player if there is only one winner and 0 if there is no winners)
        std::vector<Player_ptr> get_winners();
        void add_player(const std::shared_ptr<Player> &player);
    };

} // namespace hangman
