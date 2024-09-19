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

        /// Pointer to database object.
        Database_ptr _db;

        /// Prepared statement to create game.
        Prepared_statement _create_game_stmt;

        /// Prepared statement to finish the game.
        Prepared_statement _finish_game_stmt;

        /// Prepared statement to record user game result.
        Prepared_statement _user_game_result_stmt;

    public:
        /**
         * @brief Constructor for Discord_games_manager_impl.
         *
         * Initializes the manager with no dependencies.
         */
        Discord_games_manager_impl();

        /**
         * @brief Adds a game to the manager.
         *
         * @param game Pointer to the Discord game to add.
         * @param channel_id Channel id where game is happening.
         * @param guild_id Guild id where game is happening.
         * @returns Awaitable request to database which will return unique id of game.
         */
        Task<Database_return_t> add_game(Discord_game *game, const dpp::snowflake& channel_id, const dpp::snowflake& guild_id) override;

        /**
         * @brief Removes a game from the manager.
         *
         * @param game Pointer to the Discord game to remove.
         * @param end_reason Reason why game should be removed from games manager.
         * @param additional_data Additional data in json format to be inserted to database.
         */
        void remove_game(Discord_game* game, GAME_END_REASON end_reason,const std::string& additional_data) override;

       /**
        * @brief Records result of specific user to specific game to the database.
        *
        * @param game Pointer to the Discord game.
        * @param player Player id for which we record data.
        * @param result Result of the game which should be recorded.
        */
        void record_user_result(Discord_game* game, const dpp::snowflake& player, const std::string& result) override;

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
        void init(const Modules& modules) override;
    };

    /**
     * @brief Factory function to create a new instance of Discord_games_manager_impl.
     *
     * @return A shared pointer to the newly created Discord_games_manager_impl instance.
     */
    extern "C" Module_ptr create();

} // gb
