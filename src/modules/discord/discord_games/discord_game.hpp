//
// Created by ilesik on 7/24/24.
//

#pragma once

#include <dpp/dpp.h>
#include <src/modules/discord/discord_interactions_handler/discord_button_click_handler/discord_button_click_handler.hpp>

#include <src/modules/discord/discord_achievements_processing/discord_achievements_processing.hpp>
#include "./discord_games_manager/discord_games_manager.hpp"
#include "src/module/module.hpp"
#include "src/modules/database/database.hpp"
#include "src/modules/discord/discord_bot/discord_bot.hpp"
#include "src/modules/image_processing/image_processing.hpp"
#include "src/modules/logging/logging.hpp"

namespace gb {

    /**
     * @enum USER_REMOVE_REASON
     * @brief Represents the reasons a user might be removed from a game.
     */
    enum class USER_REMOVE_REASON {
        UNKNOWN, ///< Reason unknown or not specified.
        TIMEOUT, ///< User was removed due to timeout.
        LOSE, ///< User was removed due to losing the game.
        WIN, ///< User was removed due to winning the game.
        DRAW, ///< User was removed due to the game ending in a draw.
        SAVED ///< User's game progress was saved and they were removed.
    };

    /**
     * @brief Converts a USER_REMOVE_REASON enum value to a string.
     *
     * @param r The USER_REMOVE_REASON value.
     * @return A string representation of the reason.
     * @throws std::runtime_error If the reason is unknown.
     */
    std::string to_string(USER_REMOVE_REASON r);

    /**
     * @struct Game_data_initialization
     * @brief Holds the initialization data required to start a Discord game.
     */
    struct Game_data_initialization {
        std::string name; ///< The name of the game.
        Database_ptr db; ///< Pointer to the database module.
        Discord_bot_ptr bot; ///< Pointer to the Discord bot module.
        Discord_games_manager_ptr games_manager; ///< Pointer to the games manager module.
        Image_processing_ptr image_processing; ///< Pointer to the image processing module.
        Discord_button_click_handler_ptr button_click_handler; ///< Pointer to the button click handler module.
        Discord_achievements_processing_ptr achievements_processing; ///< Pointer to the achievements processing module.
        Logging_ptr log;
    };

    /**
     * @class Discord_game
     * @brief Base class for managing a game on Discord.
     *
     * This class provides the core functionality to manage players, handle game start/stop events,
     * and interact with Discord through a bot.
     */
    class Discord_game {
    private:
        std::vector<dpp::snowflake> _players; ///< List of players participating in the game.
        size_t _current_player_ind = 0; ///< Index of the current player.
        Task<Database_return_t> _game_create_req; ///< Request to create a game in the database.
        uint64_t _unique_game_id =
            std::numeric_limits<uint64_t>::max(); ///< Unique ID for the game, initialized as max.
        bool _is_game_started = false; ///< Flag which checks if game was started.
        bool _is_game_stopped = false; ///< Flag which checks if game was stopped.

    protected:
        Game_data_initialization _data; ///< Initialization data for the game.
        uint64_t _img_cnt = 0; ///< Counter for the number of images generated.

    public:
        /**
         * @brief Defines a return type for operations involving creating of private messages.
         *
         * This type alias represents a pair where:
         * - The first element is a boolean flag indicating the success or failure of the operation.
         *   `true` means the operation failed, and `false` means it succeeded.
         * - The second element is a map that associates user IDs (`dpp::snowflake`) with the corresponding
         *   `dpp::message` objects. It stores the private messages sent to or received from users.
         */
        typedef std::pair<bool, std::map<dpp::snowflake, dpp::message>> Direct_messages_return;

        /**
         * @brief Destructor for Discord_game.
         *
         * Stops the game and performs necessary cleanup.
         */
        virtual ~Discord_game();

        /**
         * @brief Returns a list of basic dependencies required for any game.
         *
         * @return A vector of strings representing module names.
         */
        static std::vector<std::string> get_basic_game_dependencies();

        /**
         * @brief Returns a list of image generators for the game (should be implemented by child classes).
         *
         * @return A vector of pairs, each containing a string and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Constructs a Discord_game object.
         *
         * @param _data Initialization data for the game.
         * @param players A vector of player IDs participating in the game.
         */
        Discord_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Gets the list of players in the game.
         *
         * @return A vector of player IDs.
         */
        std::vector<dpp::snowflake> get_players();

        /**
         * @brief Advances to the next player in the game.
         *
         * @return The ID of the next player.
         */
        dpp::snowflake next_player();

        /**
         * @brief Gets the current player's ID.
         *
         * @return The ID of the current player.
         */
        dpp::snowflake get_current_player();

        /**
         * @brief Gets the index of the current player.
         *
         * @return The index of the current player.
         */
        size_t get_current_player_index() const;

        /**
         * @brief Removes a player from the game for a specified reason.
         *
         * @param reason The reason for removing the player.
         * @param user The ID of the user to remove.
         */
        void remove_player(USER_REMOVE_REASON reason, const dpp::snowflake &user);

        /**
         * @brief Starts the game and registers it with the game manager.
         *
         * @param channel_id Channel id where game is played.
         * @param guild_id Guild id where game is played.
         */
        void game_start(const dpp::snowflake &channel_id, const dpp::snowflake &guild_id);

        /**
         * @brief Stops the game and deregisters it from the game manager.
         *
         * @param additional_data Optional additional data for a game in json format.
         */
        void game_stop(const std::string& additional_data = "{}");

        /**
         * @brief Gets the name of the game.
         *
         * @return The name of the game as a string.
         */
        std::string get_name() const;

        /**
         * @brief Gets the unique ID of the game.
         *
         * If the game ID has not been set, it will wait for the database creation request to complete.
         *
         * @return The unique ID of the game.
         */
        uint64_t get_uid();

        /**
         * @brief Changes current player index.
         *
         * @param index Index of player to make current.
         *
         * @throw std::runtime_error if index is out of range of players amount.
         */
        void set_current_player_index(size_t index);

        /**
         * @brief Gets the current count of images generated in the game.
         *
         * @return The number of images generated.
         */
        uint64_t get_image_cnt() const;

        dpp::task<Direct_messages_return> get_private_messages(const std::vector<dpp::snowflake> &ids);

        /**
         * @brief Adds an image to a Discord message.
         *
         * @param m The Discord message to which the image will be added.
         * @param image The image to add.
         * @return The attachment URL as a string.
         */
        std::string add_image(dpp::message &m, const Image_ptr &image);
    };

} // namespace gb
