//
// Created by ilesik on 9/11/24.
//

#pragma once
#include "src/games/battleships/battleships.hpp"
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {

    /**
     * @brief This class represents a Battleships game integrated with Discord.
     *
     * The class extends `Discord_game` and manages the game state, rendering of the game view,
     * and interaction with players through Discord. It handles the gameplay, ship placement,
     * and updates to the game's visual representation.
     */
    class Discord_battleships_game : public Discord_game {
        // Static constants for game configurations
        constexpr static int _place_timeout = 10 * 60; ///< Timeout for placing ships (in seconds).
        constexpr static int _field_size = 512; ///< Size of the field (in pixels).
        constexpr static int _distance_between_fields = _field_size / 10; ///< Distance between two player fields.
        constexpr static int _line_width = 2; ///< Width of the grid lines.

        /**
         * @brief The size of one sector in the grid. Calculated based on the field size and line width.
         */
        constexpr static double _sector_size = (_field_size - ((_field_size - _line_width * 11.0) / 10.0) - _line_width * 11.0) / 10.0;

        constexpr static int _line_length = _sector_size * 10 + _line_width * 10; ///< Length of the grid lines.
        constexpr static double _text_scale = _sector_size / 45; ///< Text scaling factor for navigation labels.
        constexpr static double _text_thickness = _sector_size / 25; ///< Text thickness for navigation labels.

        /**
         * @brief The size of the image for rendering the game field.
         * Calculated as the total width and height required to display both player fields side by side.
         */
        constexpr static const Vector2i _image_size = {_field_size * 2 + _distance_between_fields, _field_size};

        // Color constants for the game's visual elements.
        inline static const Color _default_background{32, 42, 68}; ///< Background color of the game.
        inline static const Color _line_color{255, 255, 255}; ///< Color of the grid lines.
        inline static const Color _text_active{251, 192, 45}; ///< Color for active text (e.g., when a player selects something).
        inline static const Color _text_standard{255, 255, 255}; ///< Standard text color.

        // Map linking ship types to colors for ship representation.
        inline static std::map<battleships_engine::Ship_types, Color> _ship_colors
        {
            {battleships_engine::Ship_types::Carrier, {21, 21, 21}},
            {battleships_engine::Ship_types::Battleship, {37, 37, 37}},
            {battleships_engine::Ship_types::Patrol_Boat, {53, 53, 53}},
            {battleships_engine::Ship_types::Submarine, {69, 69, 69}},
            {battleships_engine::Ship_types::Destroyer, {85, 85, 85}}
        };

        inline static const Color _miss_cell_color{128, 128, 128}; ///< Color for missed shots on the grid.
        inline static const Color _damaged_cell_color{255, 0, 0}; ///< Color for damaged ships.
        inline static const Color _occupied_cell_color{255, 255, 255, 0.5}; ///< Color for occupied cells (ships).
        inline static const Color _selected_col_color{251, 192, 45, 0.5}; ///< Color for selected columns (e.g., during ship placement).

        // Data members for managing game state and interactions.
        std::map<dpp::snowflake, dpp::message> _messages; ///< Stores messages sent to Discord users.
        std::map<dpp::snowflake, int> _user_to_player_id; ///< Maps Discord user IDs to player IDs.

        time_t _game_start_time; ///< Timestamp of when the game started.
        std::array<int, 2> _states = {0, 0}; ///< Stores the game states of both players.
        std::array<std::array<int, 2>, 2> _temp_pos; ///< Temporary positions for each player.
        battleships_engine::Battleships _engine; ///< The core Battleships game engine.
        std::array<battleships_engine::Ship*, 2> _temp_ships; ///< Temporary storage for player ships.

        std::array<bool, 2> _last_was_back = {false, false}; ///< Tracks whether the last move was a 'back' operation for each player.
        time_t _move_start; ///< Timestamp of when the current move started.
        int _is_timeout = 0; ///< Flag indicating whether a timeout occurred.
        dpp::message _message; ///< Discord message object for handling game interactions.
        std::mutex _mutex; ///< Mutex for synchronizing game state across threads.

    public:
        /**
         * @brief Constructor for the Discord Battleships game.
         *
         * Initializes the game with the provided data and list of players.
         *
         * @param _data The game initialization data.
         * @param players A vector of Discord user IDs representing the players.
         */
        Discord_battleships_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves a list of image generators for the game.
         *
         * @return A vector of pairs, where each pair contains a string identifier and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Generates a view of the game field.
         *
         * @param state Optional state parameter to customize the view (default is 0).
         * @return An Image_ptr pointing to the generated image of the game field.
         */
        Image_ptr generate_view_field(int state = 0);

        /**
         * @brief Draws the private field for a player, including their ships and any specific visual state.
         *
         * @param image The image to draw on.
         * @param position The position to start drawing from.
         * @param field The field data representing the player's grid.
         * @param ships The container holding the player's ships.
         * @param to_not_draw Optional pointer to a ship that should not be drawn (e.g., during placement).
         */
        void draw_private_field(Image_ptr &image, const std::array<int, 2> &position, const battleships_engine::Field &field,
                                const battleships_engine::Ships_container &ships, battleships_engine::Ship *to_not_draw = nullptr);

        /**
         * @brief Draws a single ship on the game field.
         *
         * @param image The image to draw on.
         * @param position The position to start drawing the ship.
         * @param ship The ship to be drawn.
         */
        void draw_ship(Image_ptr &image, const std::array<int, 2> &position, battleships_engine::Ship *ship);

        /**
         * @brief Draws the public field, which hides ship details but displays hits, misses, and damaged ships.
         *
         * @param image The image to draw on.
         * @param position The position to start drawing from.
         * @param field The field data representing the player's grid.
         * @param ships The container holding the player's ships.
         */
        void draw_public_field(Image_ptr &image, const std::array<int, 2> &position, const battleships_engine::Field &field,
                               const battleships_engine::Ships_container &ships);

        /**
         * @brief Handles the game loop when a player clicks a button in Discord.
         *
         * @param event The button click event.
         * @return A task that processes the event asynchronously.
         */
        dpp::task<void> run(dpp::button_click_t event);

        /**
         * @brief Ends the game due to a timeout.
         *
         * @param is_both_timeout Whether both players timed out (default is false).
         */
        void end_of_game_timeout(bool is_both_timeout = false);

        /**
         * @brief Handles the ship placement phase for a player.
         *
         * @param player The Discord user ID of the player placing ships.
         * @return A task that processes the ship placement asynchronously.
         */
        dpp::task<void> player_place_ships(const dpp::snowflake player);
    };
}; // namespace gb
