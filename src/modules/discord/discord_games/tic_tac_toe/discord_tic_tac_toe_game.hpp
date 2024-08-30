//
// Created by ilesik on 7/31/24.
//

#pragma once
#include "../discord_game.hpp"

namespace gb {

/**
 * @brief Represents the Tic Tac Toe game within the Discord environment.
 *
 * This class is a derived class of `Discord_game`, implementing a Tic Tac Toe
 * game that users can play through Discord. The game manages player turns, game
 * state, and user interactions through Discord's button click events.
 */
class Discord_tic_tac_toe_game : public Discord_game {
public:
  /**
   * @brief Enum to represent the signs used in the Tic Tac Toe game.
   */
  enum class SIGNS {
    X = 0, ///< Represents the "X" sign.
    O = 1, ///< Represents the "O" sign.
    EMPTY  ///< Represents an empty cell.
  };

private:
  dpp::button_click_t _event; ///< Stores the current button click event.

  /// Represents the Tic Tac Toe board as a 3x3 grid of SIGNS.
  std::array<std::array<SIGNS, 3>, 3> board = {
      {{SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
       {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY},
       {SIGNS::EMPTY, SIGNS::EMPTY, SIGNS::EMPTY}}};

  /**
   * @brief Creates an image representation of the current game state.
   *
   * @param m The Discord message object to which the image will be attached.
   * @param embed The Discord embed object where the image will be shown.
   */
  void create_image(dpp::message &m, dpp::embed &embed);

  /**
   * @brief Creates the interactive components for the game board.
   *
   * These components allow players to click on buttons to make their moves.
   *
   * @param m The Discord message object to which the components will be attached.
   */
  void create_components(dpp::message &m);

  /**
   * @brief Handles the win scenario and returns a message to be sent.
   *
   * @param timeout Indicates whether the win was due to a timeout.
   * @return dpp::message The message indicating the game has been won.
   */
  dpp::message win(bool timeout = false);

  /**
   * @brief Handles the draw scenario and returns a message to be sent.
   *
   * @return dpp::message The message indicating the game ended in a draw.
   */
  dpp::message draw();

public:
  /**
   * @brief Constructs a new Discord Tic Tac Toe game.
   *
   * @param _data The initialization data required to start the game.
   * @param players The list of players participating in the game.
   */
  Discord_tic_tac_toe_game(Game_data_initialization &_data,
                           const std::vector<dpp::snowflake> &players);

  /**
   * @brief Returns the image generators for the game.
   *
   * These generators create the base images used in the Tic Tac Toe game.
   *
   * @return std::vector<std::pair<std::string, image_generator_t>> The image generators.
   */
  static std::vector<std::pair<std::string, image_generator_t>>
  get_image_generators();

  /**
   * @brief Runs the Tic Tac Toe game.
   *
   * This function manages the game loop, handling player turns, checking for
   * wins or draws, and responding to user interactions.
   *
   * @param _event The initial button click event that starts the game.
   * @return dpp::task<void> A task representing the game's execution.
   */
  dpp::task<void> run(const dpp::button_click_t &_event);
};

} // namespace gb
