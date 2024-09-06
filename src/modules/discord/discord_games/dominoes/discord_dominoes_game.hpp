//
// Created by ilesik on 9/5/24.
//

#pragma once
#include <random>
#include <src/modules/discord/discord_games/discord_game.hpp>

namespace gb {
    namespace dominoes {
        typedef std::array<int, 2> piece;

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
        inline std::vector<piece> list_of_domino = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6},
                                             {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 2},
                                             {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 3}, {3, 4}, {3, 5},
                                             {3, 6}, {4, 4}, {4, 5}, {4, 6}, {5, 5}, {5, 6}, {6, 6}};
    }
    class Discord_dominoes_game : public Discord_game {
        int pieces_in_deck;
        std::vector<dominoes::piece> local_list_of_domino = dominoes::list_of_domino;
        std::random_device rd;
        std::mt19937 e{rd()};

        std::vector<dominoes::piece>  board;

        std::map<dpp::snowflake,std::vector<dominoes::piece>> decks;
        std::map<int,std::map<dpp::snowflake,std::array<Image_ptr,2>>> decks_images; //1st - for person 2nd - for preview
        std::map<dpp::snowflake,dpp::message> messages;
    public:
        Discord_dominoes_game(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players);

        static std::vector<std::pair<std::string, image_generator_t>> get_image_generators();

        //position - top left corner, resolution - resolution of dots image which should be generated
        static void draw_dots(Image_ptr& img, const Vector2i& position, int resolution, int dot);

        dpp::task<void> run(const dpp::button_click_t &event);

        void generate_decks();

        void make_first_turn();
    };

} // namespace gb
