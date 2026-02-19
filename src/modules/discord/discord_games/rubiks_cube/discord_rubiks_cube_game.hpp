//
// Created by ilesik on 9/18/24.
//

#pragma once
#include <random>
#include <src/games/rubiks_cube/rubiks_cube.hpp>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace rubiks_cube {

    /**
     * @struct isometric_data
     * @brief Stores isometric transformation data for Rubik's cube rendering.
     *
     * This structure holds the starting point and directional vectors used to compute
     * isometric projections of Rubik's cube sides.
     */
    struct isometric_data {
        gb::Vector2d start_p; /**< The starting point of the isometric projection. */
        gb::Vector2d vector_x; /**< The X-axis vector for isometric transformation. */
        gb::Vector2d vector_y; /**< The Y-axis vector for isometric transformation. */

        /**
         * @brief Constructor for initializing isometric_data.
         * @param start_p The starting point for the projection.
         * @param vector_x The X-axis vector for the projection.
         * @param vector_y The Y-axis vector for the projection.
         */
        isometric_data(gb::Vector2d start_p, gb::Vector2d vector_x, gb::Vector2d vector_y);

        /**
         * @brief Computes the isometric projection of a given 2D coordinate.
         * @param coord The 2D coordinate to be transformed.
         * @return The transformed coordinate in isometric projection.
         */
        gb::Vector2i get_dot(gb::Vector2d coord);
    };

    /**
     * @brief A map that defines the colors of the Rubik's cube faces.
     *
     * This inline map links Rubik's cube face identifiers (characters) to their
     * corresponding RGB color values.
     */
    inline std::map<char, gb::Color> colors = {
        {'B', gb::Color(0, 0, 255)}, /**< Blue color for 'B' face */
        {'W', gb::Color(255, 255, 255)}, /**< White color for 'W' face */
        {'R', gb::Color(255, 0, 0)}, /**< Red color for 'R' face */
        {'G', gb::Color(0, 255, 0)}, /**< Green color for 'G' face */
        {'Y', gb::Color(255, 255, 0)}, /**< Yellow color for 'Y' face */
        {'O', gb::Color(255, 102, 0)} /**< Orange color for 'O' face */
    };
} // namespace rubiks_cube

namespace gb {

    /**
     * @class Discord_rubiks_cube_game
     * @brief Represents a Rubik's Cube game played through Discord.
     *
     * This class handles the game logic, image generation, and message updates for a Rubik's Cube game
     * hosted on Discord, allowing players to interact with the game through commands and buttons.
     */
    class Discord_rubiks_cube_game : public Discord_game {
        rubiks_cube::Rubiks_cube_engine _engine; /**< The Rubik's Cube game engine. */
        bool _is_view = false; /**< Whether the game is in view (rotation) mode. */
        int _amount_moves = 0; /**< The number of moves made in the game. */
        std::default_random_engine _rand_obj; /**< Random engine for shuffling the Rubik's cube. */

        /**
         * @brief Main game loop that runs the Rubik's Cube game.
         * @param sevent The slash command event that triggered the game.
         * @return A task representing the asynchronous execution of the game loop with additional data for game record.
         */
        dpp::task<std::string> run(dpp::slashcommand_t sevent);

    public:
        /**
         * @brief Constructor for initializing the Rubik's Cube game.
         * @param _data The game data initialization object.
         * @param players A vector of player IDs participating in the game.
         */
        Discord_rubiks_cube_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_rubiks_cube_game() = default;

        /**
         * @brief Returns a list of image generators for the game.
         * @return A vector of pairs, each containing a string description and an image generator function.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Prepares the Discord message containing the game state and components.
         * @param message The message to be updated with game details.
         */
        void prepare_message(dpp::message &message);

        /**
         * @brief Creates an image representing the current state of the Rubik's Cube.
         * @return A pointer to the generated image.
         */
        Image_ptr create_image();

        /**
         * @brief Checks if the Rubik's Cube is solved.
         * @return True if the cube is solved, false otherwise.
         */
        bool is_win();
    };

} // namespace gb
