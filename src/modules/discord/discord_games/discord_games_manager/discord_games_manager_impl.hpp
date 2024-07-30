//
// Created by ilesik on 7/24/24.
//

#pragma once
#include "./discord_games_manager.hpp"
#include <condition_variable>

namespace gb {

    /**
     * @class Discord_games_manager_impl
     * @brief Implementation of the Discord games manager.
     *
     * This class manages a collection of Discord games, allowing games to be added and removed.
     */
    class Discord_games_manager_impl : public Discord_games_manager {
        /// Mutex for synchronizing access to the games list.
        std::mutex _mutex;

        /// Condition variable to notify when games are removed.
        std::condition_variable _cv;

        /// Vector holding pointers to the Discord games.
        std::vector<Discord_game*> _games;

    public:
        /**
         * @brief Constructor for Discord_games_manager_impl.
         *
         * Initializes the manager with no dependencies.
         */
        Discord_games_manager_impl();;

        /**
         * @brief Adds a game to the manager.
         *
         * @param game Pointer to the Discord game to add.
         */
        void add_game(Discord_game* game) override;

        /**
         * @brief Removes a game from the manager.
         *
         * @param game Pointer to the Discord game to remove.
         */
        void remove_game(Discord_game* game) override;

        /**
         * @brief Stops the manager's operation.
         *
         * Waits until all games are removed.
         */
        void stop() override;

        /**
         * @brief Starts the manager's operation.
         */
        void run() override;

        /**
         * @brief Initializes the manager with the necessary modules.
         *
         * @param modules A map of module names to their shared pointers.
         */
        void innit(const Modules& modules) override;
    };

    /**
     * @brief Factory function to create a new instance of Discord_games_manager_impl.
     *
     * @return A shared pointer to the newly created Discord_games_manager_impl instance.
     */
    extern "C" Module_ptr create();

} // gb
