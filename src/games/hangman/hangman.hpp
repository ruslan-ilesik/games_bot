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

    /**
     * @brief Minimum word size for the Hangman game.
     * Words shorter than this will be excluded from the word list.
     */
    inline constexpr unsigned int min_word_size = 6;

    /**
     * @brief A list of words for the Hangman game, loaded from a text file.
     *
     * The words are read from the file `./data/english_words.txt`. Words shorter
     * than the minimum word size are excluded. This list is used for random word generation.
     */
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

    /**
     * @brief Retrieves a random word from the word list.
     *
     * This function selects a random word from the list of pre-loaded words and
     * returns it for the Hangman game.
     *
     * @return A random word as a `std::string`.
     */
    std::string get_random_word();


    /**
     * @brief Represents a player in the Hangman game.
     *
     * The `Player` class handles a single player's state, including their guessed
     * letters, number of errors, and whether they have won, lost, or been eliminated.
     */
    class Player {
    private:
        std::vector<char> word_to_guess;      /**< The word the player is trying to guess. */
        std::vector<char> guessed_word;       /**< The word as guessed so far (with underscores). */
        std::vector<char> unused_characters;  /**< The characters that have not been used by the player. */
        std::vector<char> used_characters = {}; /**< The characters that have been used by the player. */
        bool is_elimin = false;               /**< Indicates if the player is eliminated. */

        /**
         * @brief Private constructor that initializes the player with the word to guess.
         *
         * @param word The word that the player will try to guess.
         */
        explicit Player(const std::string &word);

    public:
        unsigned int error_cnt = 0;  /**< Number of incorrect guesses made by the player. */
        unsigned int moves_cnt = 0;  /**< Total number of moves made by the player. */

        /**
         * @brief Factory method to create a new player with a given word.
         *
         * @param word The word that the player will try to guess.
         * @return A `shared_ptr` to the newly created `Player`.
         */
        static std::shared_ptr<Player> create(const std::string &word);

        /**
         * @brief Makes a move by guessing a letter.
         *
         * Updates the game state based on the guessed letter. If the game has
         * already finished, throws an exception.
         *
         * @param move The guessed letter.
         * @return `true` if the game is finished after this move, otherwise `false`.
         */
        bool make_move(char move);

        /**
         * @brief Checks if the player has finished the game.
         *
         * A player is considered finished if they either win, make 11 errors, or are eliminated.
         *
         * @return `true` if the player has finished the game, otherwise `false`.
         */
        bool is_finished() const;

        /**
         * @brief Checks if the player has won the game.
         *
         * A player wins if they have guessed the entire word correctly.
         *
         * @return `true` if the player has won, otherwise `false`.
         */
        bool is_win() const;

        /**
         * @brief Eliminates the player from the game.
         */
        void eliminate();

        /**
         * @brief Checks if the player has been eliminated.
         *
         * @return `true` if the player has been eliminated, otherwise `false`.
         */
        bool is_eliminated() const;

        /**
         * @brief Retrieves the word the player is trying to guess.
         *
         * @return A vector of characters representing the word.
         */
        std::vector<char> get_word_to_guess();

        /**
         * @brief Retrieves the characters the player has not used yet.
         *
         * @return A vector of unused characters.
         */
        std::vector<char> get_unused_characters();

        /**
         * @brief Retrieves the characters the player has already used.
         *
         * @return A vector of used characters.
         */
        std::vector<char> get_used_characters();

        /**
         * @brief Retrieves the word guessed so far.
         *
         * @return A vector of characters representing the guessed word.
         */
        std::vector<char> get_guessed_word();
    };

    typedef std::shared_ptr<Player> Player_ptr;

    /**
     * @brief Manages the Hangman game with multiple players.
     *
     * The `Hangman` class is responsible for managing the state of the game, including
     * tracking players, checking game completion, and determining the winners.
     */
    class Hangman {
    private:
        std::vector<Player_ptr> players;  /**< List of players participating in the game. */

    public:
        /**
         * @brief Constructor that initializes the Hangman game with a list of players.
         *
         * @param players A vector of shared pointers to the players in the game.
         */
        explicit Hangman(const std::vector<Player_ptr> &players);

        /**
         * @brief Default constructor for the Hangman game.
         */
        Hangman() = default;

        /**
         * @brief Makes a move for a given player by guessing a letter.
         *
         * @param player A shared pointer to the player making the move.
         * @param move The guessed letter.
         * @return `true` if the game is finished after this move, otherwise `false`.
         */
        bool make_move(const Player_ptr &player, char move);

        /**
         * @brief Retrieves the list of players in the game.
         *
         * @return A vector of shared pointers to the players.
         */
        std::vector<Player_ptr> get_players();

        /**
         * @brief Checks if the game is finished.
         *
         * A game is finished if all players have finished or the player list is empty.
         *
         * @return `true` if the game is finished, otherwise `false`.
         */
        bool is_game_finished();

        /**
         * @brief Checks if all players have been eliminated.
         *
         * @return `true` if all players have been eliminated, otherwise `false`.
         */
        bool is_all_eliminated();

        /**
         * @brief Retrieves the winners of the game.
         *
         * The winners are the players who have guessed their words with the fewest errors.
         *
         * @return A vector of shared pointers to the winning players.
         */
        std::vector<Player_ptr> get_winners();

        /**
         * @brief Adds a new player to the Hangman game.
         *
         * @param player A shared pointer to the player to be added.
         */
        void add_player(const std::shared_ptr<Player> &player);
    };

} // namespace hangman
