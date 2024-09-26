//
// Created by ilesik on 9/19/24.
//

#pragma once
#include <src/games/minesweeper/minesweeper.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief A class representing a Discord-based Minesweeper game.
     *
     * This class manages the flow of a Minesweeper game played within a Discord channel,
     * handling player interactions, game states, and visual representation.
     */
    class Discord_minesweeper_game : public Discord_game {
        minesweeper_engine::Minesweeper _engine; ///< The Minesweeper game engine instance.
        constexpr static auto _numeration = "123456789ABCDEFGHIJKLMNOPQRSTYVWXYZ"; ///< Column/Row numeration for the game board.
        bool _is_fst_move = true; ///< Flag indicating whether it's the first move of the game.
        std::array<int, 2> _action_position; ///< Stores the current selected position (column and row).

        /**
         * @brief Enumeration representing the various states of the game.
         */
        enum Move_states {
            SELECT_COL, ///< State for selecting the column.
            SELECT_ROW, ///< State for selecting the row.
            SELECT_ACTION, ///< State for selecting the action to perform.
            END_OF_GAME ///< State representing the end of the game.
        };

        Move_states _state = SELECT_COL; ///< Current state of the game.
        bool _next_btn = false; ///< Flag indicating if the 'next' button was pressed for pagination.
        int _level; ///< Game difficulty level.
        int _timeout_time; ///< Timeout duration for player response.
        std::default_random_engine _rd; ///< Random engine used for mine generation.

    public:
        /**
         * @brief Constructs a new Discord Minesweeper Game.
         *
         * @param _data Reference to the game data initialization object.
         * @param players List of players participating in the game.
         * @param level Difficulty level of the game (1 = Easy, 2 = Medium, 3 = Hard).
         */
        Discord_minesweeper_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players, int level);

        /**
         * @brief Retrieves a list of image generators for the game.
         *
         * @return A vector of pairs, where each pair contains a string identifier and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Prepares the Discord message to be sent or updated with the current game state.
         *
         * @param message The Discord message object to be prepared.
         */
        void prepare_message(dpp::message &message);

        /**
         * @brief Creates and returns an image representation of the current game board.
         *
         * @return A pointer to the generated image.
         */
        Image_ptr create_image();

        /**
         * @brief The main game loop, executed as a coroutine.
         *
         * Manages the game flow, handles user input, and updates the game state accordingly.
         *
         * @param sevent The slash command event that triggered the game.
         * @return A dpp::task<void> representing the asynchronous execution of the game loop.
         */
        dpp::task<void> run(dpp::slashcommand_t sevent);
    };

} // namespace gb
