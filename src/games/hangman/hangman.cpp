//
// Created by rusla on 4/10/2024.
//

#include "hangman.hpp"


using namespace hangman;

std::string hangman::get_random_word() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, words.size() - 1);
    return hangman::words[dist(gen)];
}

Player::Player(const std::string &word) {
    word_to_guess.resize(word.size());
    guessed_word.resize(word.size());

    std::ranges::copy(word, word_to_guess.begin());

    std::ranges::fill(guessed_word, '_');

    // Initialize used_characters with all lowercase letters
    for (char i = 'a'; i <= 'z'; i++) {
        unused_characters.push_back(i);
    }
}

std::shared_ptr<Player> Player::create(const std::string &word) { return std::shared_ptr<Player>(new Player(word)); }

bool Player::make_move(char move) {
    if (is_finished()) {
        throw std::runtime_error("Player can not move after he finished a game");
    }
    if (std::ranges::find(used_characters, move) != used_characters.end()) {
        throw std::invalid_argument("Character was already used in guessed word");
    }
    moves_cnt++;
    used_characters.push_back(move);
    std::erase(unused_characters, move);
    if (std::ranges::find(word_to_guess, move) == word_to_guess.end()) {
        error_cnt++;
        return is_finished();
    }

    for (size_t i = 0; i < word_to_guess.size(); i++) {
        if (word_to_guess[i] == move) {
            guessed_word[i] = move;
        }
    }

    return is_finished();
}

bool Player::is_finished() const { return error_cnt >= 11 || is_win() || is_eliminated(); }

bool Player::is_win() const { return this->word_to_guess == this->guessed_word; }

void Player::eliminate() { is_elimin = true; }

bool Player::is_eliminated() const { return is_elimin; }

std::vector<char> Player::get_word_to_guess() { return word_to_guess; }

std::vector<char> Player::get_unused_characters() { return unused_characters; }

std::vector<char> Player::get_used_characters() { return used_characters; }

std::vector<char> Player::get_guessed_word() { return guessed_word; }

Hangman::Hangman(const std::vector<Player_ptr> &players) {
    if (players.empty()) {
        throw std::invalid_argument("Players vector can not be empty");
    }
    this->players = players;
}

bool Hangman::make_move(const Player_ptr &player, char move) { return player->make_move(move); }

std::vector<Player_ptr> Hangman::get_players() { return players; }

bool Hangman::is_game_finished() {
    return std::ranges::all_of(players, [](const auto &i) { return i->is_finished(); }) || players.empty();
}

bool Hangman::is_all_eliminated() {
    return std::ranges::all_of(players, [](const auto &i) { return i->is_eliminated(); });
}

std::vector<Player_ptr> Hangman::get_winners() {
    if (!is_game_finished()) {
        throw std::runtime_error("Can not get winner of not ended game");
    }
    std::ranges::sort(players, [](auto &p1, auto &p2) {
        if (p1->error_cnt == p2->error_cnt) {
            return p1->moves_cnt < p2->moves_cnt;
        }
        return p1->error_cnt < p2->error_cnt;
    });
    std::vector<std::shared_ptr<Player>> result;
    for (auto &i: players) {
        if (i->is_win() &&
            (result.empty() || (result[0]->moves_cnt == i->moves_cnt && result[0]->error_cnt == i->error_cnt))) {
            result.push_back(i);
        } else {
            break;
        }
    }
    return result;
}

void Hangman::add_player(const std::shared_ptr<Player> &player) { this->players.push_back(player); }
