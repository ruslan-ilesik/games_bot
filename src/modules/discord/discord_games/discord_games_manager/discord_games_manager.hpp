//
// Created by ilesik on 7/24/24.
//

#pragma once
#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"

namespace gb {

    /*
    * @brief Forward declaration of Discord_game class to eliminate circular dependencies problem.
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
        Discord_games_manager(const std::string& name, const std::vector<std::string>& dependencies)
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
         * @returns Awaitable request to database which will return unique id of game.
         */
        virtual Task<Database_return_t> add_game(Discord_game* game) = 0;

        /**
         * @brief Removes a game from the manager.
         *
         * This pure virtual function must be implemented by derived classes to
         * remove a game from the collection managed by the manager.
         *
         * @param game Pointer to the Discord game to remove.
         */
        virtual void remove_game(Discord_game* game) = 0;
    };

    /**
    * @typedef Discord_games_manager_ptr
    * @brief A shared pointer type for Discord_games_manager.
    */
    typedef std::shared_ptr<Discord_games_manager> Discord_games_manager_ptr;

} // gb

#include "../discord_game.hpp"