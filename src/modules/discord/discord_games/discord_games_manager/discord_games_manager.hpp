//
// Created by ilesik on 7/24/24.
//

#pragma once
#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include <dpp/dpp.h>

namespace gb {

/**
 * @enum GAME_END_REASON
 * @brief Enumerates the possible reasons for ending a Discord game.
 *
 * The `GAME_END_REASON` enum provides a set of constants that represent
 * the possible outcomes or reasons for ending a game managed by the
 * `Discord_games_manager`. These reasons help determine how the game
 * should be finalized, recorded, or potentially resumed.
 *
 * @var GAME_END_REASON::FINISHED
 * Indicates that the game has reached its natural conclusion.
 * This could be due to a player winning, a draw, or any other
 * standard end condition defined by the game's rules.
 *
 * @var GAME_END_REASON::ERROR
 * Indicates that the game was terminated prematurely due to an
 * unexpected error or issue. This could be caused by technical
 * difficulties, such as a server crash or
 * an unexpected exception during gameplay.
 *
 * @var GAME_END_REASON::SAVED
 * Indicates that the game was intentionally ended with the
 * intention of resuming it at a later time. This is typically
 * used when the current game state is saved for future continuation.
 *
 */
enum class GAME_END_REASON { FINISHED, ERROR, SAVED };

/*
 * @brief Forward declaration of Discord_game class to eliminate circular
 * dependencies problem.
 */
class Discord_game;

/**
 * @class Discord_games_manager
 * @brief Abstract base class for managing Discord games.
 *
 * This class defines the interface for managing a collection of Discord games,
 * including methods for adding and removing games.
 */
class Discord_games_manager : public Module {
public:
  /**
   * @brief Constructor for Discord_games_manager.
   *
   * Initializes the manager with the specified name and dependencies.
   *
   * @param name The name of the manager.
   * @param dependencies The list of module dependencies.
   */
  Discord_games_manager(const std::string &name,
                        const std::vector<std::string> &dependencies)
      : Module(name, dependencies) {}

  /**
   * @brief Virtual destructor for Discord_games_manager.
   */
  virtual ~Discord_games_manager() = default;

  /**
   * @brief Adds a game to the manager.
   *
   * This pure virtual function must be implemented by derived classes to
   * add a game to the collection managed by the manager.
   *
   * @param game Pointer to the Discord game to add.
   * @param channel_id Channel id where game is happening.
   * @param guild_id Guild id where game is happening.
   * @returns Awaitable request to database which will return unique id of game.
   */
  virtual Task<Database_return_t> add_game(Discord_game *game, const dpp::snowflake& channel_id, const dpp::snowflake& guild_id) = 0;

  /**
   * @brief Removes a game from the manager.
   *
   * This pure virtual function must be implemented by derived classes to
   * remove a game from the collection managed by the manager.
   *
   * @param game Pointer to the Discord game to remove.
   * @param end_reason Reason why game should be removed from games manager.
   * @param additional_data Additional data in json format to be inserted to database.
   */
  virtual void remove_game(Discord_game *game, GAME_END_REASON end_reason, const std::string& additional_data="{}") = 0;

  /**
   * @brief Records result of specific user to specific game.
   *
   * This pure virtual function must be implemented by derived classes to
   * record a game result of the game.
   *
   * @param game Pointer to the Discord game.
   * @param player Player id for which we record data.
   * @param result Result of the game which should be recorded.
   */
  virtual void record_user_result(Discord_game *game,
                                  const dpp::snowflake &player,
                                  const std::string &result) = 0;
};

/**
 * @typedef Discord_games_manager_ptr
 * @brief A shared pointer type for Discord_games_manager.
 */
typedef std::shared_ptr<Discord_games_manager> Discord_games_manager_ptr;

} // namespace gb

#include "../discord_game.hpp"