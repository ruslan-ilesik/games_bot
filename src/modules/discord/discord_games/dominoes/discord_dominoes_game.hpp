//
// Created by ilesik on 9/5/24.
//

#pragma once
#include <random>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {
    namespace dominoes {
        // Typedef for a domino piece, represented as an array of two integers.
        typedef std::array<int, 2> piece;

        /**
         * @brief Map of domino pieces to their corresponding emoji and Discord snowflake IDs.
         *
         * This map links each domino piece (represented as an array of two integers) to a tuple containing
         * the emoji representation of the piece and its Discord snowflake ID.
         */
        inline std::map<piece, std::tuple<std::string, dpp::snowflake>> emojis = {
            {
                {0, 0},
                {"0_0", 1026947488407638077},
            },
            {
                {0, 1},
                {"0_1", 1026947486432100362},
            },
            {
                {0, 2},
                {"0_2", 1026947484632756265},
            },
            {
                {0, 3},
                {"0_3", 1026947483131191306},
            },
            {
                {0, 4},
                {"0_4", 1026947481587695727},
            },
            {
                {0, 5},
                {"0_5", 1026947479956099132},
            },
            {
                {0, 6},
                {"0_6", 1026947476315451522},
            },
            {
                {1, 1},
                {"1_1", 1026947477867343903},
            },
            {
                {1, 2},
                {"1_2", 1026947474784526427},
            },
            {
                {1, 3},
                {"1_3", 1026947473387823144},
            },
            {
                {1, 4},
                {"1_4", 1026947471731085332},
            },
            {
                {1, 5},
                {"1_5", 1026947469612953661},
            },
            {
                {1, 6},
                {"1_6", 1026947468203667609},
            },
            {
                {2, 2},
                {"2_2", 1026947466655973466},
            },
            {
                {2, 3},
                {"2_3", 1026947465104085112},
            },
            {
                {2, 4},
                {"2_4", 1026947461039788052},
            },
            {
                {2, 5},
                {"2_5", 1026947462813974569},
            },
            {
                {2, 6},
                {"2_6", 1026947459429187685},
            },
            {
                {3, 3},
                {"3_3", 1026947457776627743},
            },
            {
                {3, 4},
                {"3_4", 1026947456128254033},
            },
            {
                {3, 5},
                {"3_5", 1026947452655370351},
            },
            {
                {3, 6},
                {"3_6", 1026947454404407336},
            },
            {
                {4, 4},
                {"4_4", 1026947450818285689},
            },
            {
                {4, 5},
                {"4_5", 1026947447731269772},
            },
            {
                {4, 6},
                {"4_6", 1026947449417375785},
            },
            {
                {5, 5},
                {"5_5", 1026947446162608168},
            },
            {
                {5, 6},
                {"5_6", 1026947444367433860},
            },
            {{6, 6}, {"6_6", 1026947443071406270}},
        };

        /**
         * @brief List of all domino pieces.
         *
         * This vector contains all possible combinations of domino pieces used in the game.
         */
        inline std::vector<piece> list_of_domino = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6},
                                                    {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 2},
                                                    {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 3}, {3, 4}, {3, 5},
                                                    {3, 6}, {4, 4}, {4, 5}, {4, 6}, {5, 5}, {5, 6}, {6, 6}};
    } // namespace dominoes
    /**
     * @brief Class representing a Dominoes game played through a Discord bot.
     *
     * This class handles all game logic, deck generation, move validation, and game flow
     * for a game of dominoes being played through Discord.
     */
    class Discord_dominoes_game : public Discord_game {
        int _pieces_in_deck; ///< Number of pieces in the deck.
        std::vector<dominoes::piece> _local_list_of_domino =
            dominoes::list_of_domino; ///< Local copy of all domino pieces.
        std::random_device _rd; ///< Random device used for shuffling the deck.

        dpp::message _main_message; ///< Main message used for game updates.
        std::vector<dpp::snowflake> _original_players_list; ///< List of players in the game.

        std::vector<dominoes::piece> _board; ///< Domino pieces currently placed on the board.
        std::vector<dominoes::piece> _possible_moves; ///< Possible moves based on current board state.

        std::map<dpp::snowflake, Image_ptr> _hidden_deck_images; ///< Map of hidden deck images for each player.
        std::map<dpp::snowflake, std::vector<dominoes::piece>> _decks; ///< Decks of each player.
        dominoes::piece _to_place; ///< Piece currently being placed.
        std::map<dpp::snowflake, dpp::message> _messages; ///< Messages sent to players.

    public:
        /**
         * @brief Constructor for the Discord Dominoes game.
         *
         * Initializes the game with the provided game data and player list.
         *
         * @param _data Game data initialization structure.
         * @param players Vector of player snowflake IDs.
         */
        Discord_dominoes_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        /**
         * @brief Retrieves a list of image generators for the game.
         *
         * @return Vector of pairs containing image generator names and their corresponding generators.
         */
        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        /**
         * @brief Draws the dots on a domino piece image.
         *
         * @param img Reference to the image object where dots will be drawn.
         * @param position The top-left position to start drawing the dots.
         * @param resolution The resolution of the dots image to be generated.
         * @param dot The number of dots to draw on the piece.
         */
        static void draw_dots(Image_ptr &img, const Vector2i &position, int resolution, int dot);

        /**
         * @brief Starts the game and handles user interaction through Discord buttons.
         *
         * @param event The button click event from Discord.
         * @return A task representing the asynchronous operation of running the game.
         */
        dpp::task<void> run(const dpp::button_click_t &event);

        /**
         * @brief Generates the decks for all players.
         *
         * Shuffles the available pieces and assigns them to players.
         */
        void generate_decks();

        /**
         * @brief Calculates and retrieves the possible moves for the current player.
         *
         * This checks the current board and the player's hand to determine available moves.
         */
        void get_possible_moves();

        /**
         * @brief Makes the first move in the game, placing a piece on the board.
         */
        void make_first_turn();

        /**
         * @brief Constructs message for current user.
         *
         * This method is responsible for sending game updates to players through Discord.
         */
        void make_message();

        /**
         * @brief Custom condition for determining the end of the game (when no move can be made but everyone have dominoes).
         *
         * This method allows for setting specific conditions under which the game should end.
         */
        void end_game_custom_condition();

        /**
         * @brief Generates the game image and attaches it to the Discord message.
         *
         * @param m Reference to the Discord message where the image will be attached.
         * @param embed Reference to the embed that will hold additional information.
         */
        void generate_image(dpp::message &m, dpp::embed &embed);

        /**
         * @brief Generates a hidden deck image for a player.
         *
         * @param player The Discord snowflake ID of the player.
         * @param resolution The resolution of the generated image.
         */
        void generate_hidden_deck_image(const dpp::snowflake &player, const Vector2i &resolution);

        /**
         * @brief Declares the winner of the game.
         *
         * This method is called when a player wins the game.
         */
        void win();

        /**
         * @brief Generates an image of a player's deck.
         *
         * @param player The Discord snowflake ID of the player.
         * @param resolution The resolution of the generated image.
         * @return The generated image.
         */
        Image_ptr generate_deck_image(const dpp::snowflake &player, const Vector2i &resolution);
    };

} // namespace gb
