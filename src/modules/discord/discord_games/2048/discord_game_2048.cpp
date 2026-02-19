//
// Created by ilesik on 9/29/24.
//

#include "discord_game_2048.hpp"

namespace gb {
    namespace n_2048 {

        std::map<int, Color> bg_colors = {
            {0, Color(205, 193, 180)},  {2, Color(238, 228, 218)},   {4, Color(237, 224, 200)},
            {8, Color(242, 177, 121)},  {16, Color(245, 149, 99)},   {32, Color(246, 124, 96)},
            {64, Color(246, 94, 59)},   {128, Color(237, 207, 115)}, {256, Color(237, 204, 98)},
            {512, Color(237, 200, 80)}, {1024, Color(237, 197, 63)}, {2048, Color(237, 194, 45)}};

        std::map<int, Color> txt_colors = {
            {2, Color(119, 110, 101)},    {4, Color(119, 110, 101)},   {8, Color(249, 246, 242)},
            {16, Color(249, 246, 242)},   {32, Color(249, 246, 242)},  {64, Color(249, 246, 242)},
            {128, Color(249, 246, 242)},  {256, Color(249, 246, 242)}, {512, Color(249, 246, 242)},
            {1024, Color(249, 246, 242)}, {2048, Color(249, 246, 242)}};

    } // namespace n_2048

    Discord_game_2048::Discord_game_2048(Game_data_initialization &_data, const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players,&Discord_game_2048::run) {}

    std::vector<std::pair<std::string, image_generator_t>> Discord_game_2048::get_image_generators() { return {}; }

    dpp::task<void> Discord_game_2048::run(dpp::slashcommand_t sevent) {

        dpp::message message;
        message.add_embed(dpp::embed());
        message.channel_id = sevent.command.channel_id;
        message.guild_id = sevent.command.guild_id;
        message.id = 0;
        this->find_clear();
        this->new_peace();
        this->get_possible_moves();
        prepare_message(message);
        dpp::task<Button_click_return> button_click_awaitable =
            _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, 60);
        _data.bot->reply(sevent, message);
        Button_click_return r = co_await button_click_awaitable;
        dpp::button_click_t event;
        while (1) {
            if (r.second) {
                // timeout
                message.components.clear();
                std::string desc = "If you still want to play, you can create new game.\nPlayer " + dpp::utility::user_mention(
                          get_current_player()) + " lost his game.\n";
                message.embeds[0].set_color(dpp::colors::red).set_title("Game Timeout.").set_description(desc);
                message.embeds[0].set_image(add_image(message, create_image()));
                if (message.id !=0) {
                    _data.bot->event_edit_original_response(event,message);
                }
                else {
                    _data.bot->event_edit_original_response(sevent,message);
                }
                remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                break;
            }
            event = r.first;
            message.id = event.command.message_id;
            if (event.custom_id == "up") {
                up(_board);
            } else if (event.custom_id == "down") {
                down(_board);
            } else if (event.custom_id == "left") {
                left(_board);
            } else {
                right(_board);
            }

            // check win condition
            for (int y = 0; y < static_cast<int>(std::size(_board)); y++) {
                for (int x = 0; x < static_cast<int>(std::size(_board[y])); x++) {
                    if (_board[y][x] >= 1024) {
                        _data.achievements_processing->activate_achievement("Half way there", get_current_player(),
                                                               event.command.channel_id);
                    }
                    if (_board[y][x] == 2048) {
                        _data.achievements_processing->activate_achievement("2^11", get_current_player(), event.command.channel_id);
                        message.components.clear();
                        message.embeds[0]
                            .set_color(dpp::colors::green)
                            .set_title("Game over!!")
                            .set_description("Player: " + dpp::utility::user_mention(get_current_player()) +
                                             " got 2048 and **won** the game.");
                        message.embeds[0].set_image(add_image(message, create_image()));
                        remove_player(USER_REMOVE_REASON::WIN,get_current_player());
                        break;
                    }
                }
            }

            auto lose = [this,&message]() {
                message.components.clear();
                message.embeds[0]
                    .set_color(dpp::colors::red)
                    .set_title("Game over!!")
                    .set_description("Player: " + dpp::utility::user_mention(get_current_player()) +
                                     "have not got 2048 and **lose** the game. You will be more lucky next time");
                message.embeds[0].set_image(add_image(message, create_image()));
                remove_player(USER_REMOVE_REASON::LOSE,get_current_player());
            };
            find_clear();
            if (_clear_places.empty()) {
                lose();
                break;
            }
            new_peace();
            get_possible_moves();
            if (!_possible_moves['w'] && !_possible_moves['s'] && !_possible_moves['a'] &&
                !_possible_moves['d']) {
                lose();
                break;
            }
            prepare_message(message);
            button_click_awaitable =
                _data.button_click_handler->wait_for_with_reply(message, {get_current_player()}, 60);
            _data.bot->event_edit_original_response(event, message);
            r = co_await button_click_awaitable;
        }

        co_return;
    }
    Image_ptr Discord_game_2048::create_image() {
        _img_cnt++;
        int img_size = 256;
        Image_ptr img = _data.image_processing->create_image({img_size, img_size}, Color(187, 173, 160));
        int space_size = (img_size / std::size(this->_board)) / 10;
        int square_size = (img_size - space_size * std::size(this->_board) - space_size) / std::size(this->_board);
        for (int i = 0; i < static_cast<int>(std::size(this->_board)); i++) {
            for (int j = 0; j < static_cast<int>(std::size(this->_board[i])); j++) {
                Vector2d s(space_size + j * space_size + square_size * j,
                           space_size + i * space_size + square_size * i);
                img->draw_rectangle(s, Vector2i(s.x + square_size, s.y + square_size),
                                    n_2048::bg_colors.at(_board[i][j]), -1);
                if (_board[i][j]) {
                    img->draw_text(std::to_string(_board[i][j]),
                                   Vector2d(s.x + (4 - std::to_string(_board[i][j]).length()) * (square_size / 8),
                                            s.y + square_size / 1.5),
                                   0.65 * img_size / 256, n_2048::txt_colors.at(_board[i][j]), square_size / 25.0);
                }
            }
        }

        return img;
    }

    void Discord_game_2048::prepare_message(dpp::message &message) {
        message.embeds[0]
            .set_title("2048 Game")
            .set_description(std::format(
                "Player: {}\nTimeout: <t:{}:R>", dpp::utility::user_mention(get_current_player()),
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                        .count() +
                    +60))
            .set_color(dpp::colors::blue);

        message.components.clear();
        message
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("0")
                                                  .set_emoji("clear", 1015646216509468683)

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(!this->_possible_moves['w'])
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("up")
                                                  .set_emoji("⬆️"))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("1")
                                                  .set_emoji("clear", 1015646216509468683)))
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(!this->_possible_moves['a'])
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("left")
                                                  .set_emoji("⬅️")

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("2")
                                                  .set_emoji("clear", 1015646216509468683))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(!this->_possible_moves['d'])
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("right")
                                                  .set_emoji("➡️")))
            .add_component(dpp::component()
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("3")
                                                  .set_emoji("clear", 1015646216509468683)

                                                  )
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(!this->_possible_moves['s'])
                                                  .set_style(dpp::cos_primary)
                                                  .set_id("down")
                                                  .set_emoji("⬇️"))
                               .add_component(dpp::component()
                                                  .set_type(dpp::cot_button)
                                                  .set_disabled(true)
                                                  .set_style(dpp::cos_secondary)
                                                  .set_id("4")
                                                  .set_emoji("clear", 1015646216509468683)));

        message.embeds[0].set_image(add_image(message, create_image()));
    }


    void Discord_game_2048::up(unsigned int (*a)[4]) {
        int i, j, lt, ri;
        for (j = 0; j < 4; j++) {
            lt = 0, ri = j;
            for (i = 1; i < 4; i++) {
                if (a[i][j] != 0) {
                    if (a[i - 1][j] == 0 || a[i - 1][j] == a[i][j]) {
                        if (a[lt][ri] == a[i][j]) {
                            a[lt][ri] *= 2;
                            a[i][j] = 0;
                        } else {
                            if (a[lt][ri] == 0) {
                                a[lt][ri] = a[i][j];
                                a[i][j] = 0;
                            } else {
                                a[++lt][ri] = a[i][j];
                                a[i][j] = 0;
                            }
                        }
                    } else
                        lt++;
                }
            }
        }
    }

    void Discord_game_2048::down(unsigned int (*a)[4]) {
        int i, j, lt, ri;
        for (j = 0; j < 4; j++) {
            lt = 3, ri = j;
            for (i = 2; i >= 0; i--) {
                if (a[i][j] != 0) {
                    if (a[i + 1][j] == 0 || a[i + 1][j] == a[i][j]) {
                        if (a[lt][ri] == a[i][j]) {
                            a[lt][ri] *= 2;
                            a[i][j] = 0;
                        } else {
                            if (a[lt][ri] == 0) {
                                a[lt][ri] = a[i][j];
                                a[i][j] = 0;
                            } else {
                                a[--lt][ri] = a[i][j];
                                a[i][j] = 0;
                            }
                        }
                    } else
                        lt--;
                }
            }
        }
    }

    void Discord_game_2048::left(unsigned int (*a)[4]) {
        int i, j, lt, ri;
        for (i = 0; i < 4; i++) {
            lt = i, ri = 0;
            for (j = 1; j < 4; j++) {
                if (a[i][j] != 0) {
                    if (a[i][j - 1] == 0 || a[i][j - 1] == a[i][j]) {
                        if (a[lt][ri] == a[i][j]) {
                            a[lt][ri] *= 2;
                            a[i][j] = 0;
                        } else {
                            if (a[lt][ri] == 0) {
                                a[lt][ri] = a[i][j];
                                a[i][j] = 0;
                            } else {
                                a[lt][++ri] = a[i][j];
                                a[i][j] = 0;
                            }
                        }
                    } else
                        ri++;
                }
            }
        }
    }

    void Discord_game_2048::right(unsigned int (*a)[4]) {
        int i, j, lt, ri;
        for (i = 0; i < 4; i++) {
            lt = i, ri = 3;
            for (j = 2; j >= 0; j--) {
                if (a[i][j] != 0) {
                    if (a[i][j + 1] == 0 || a[i][j + 1] == a[i][j]) {
                        if (a[lt][ri] == a[i][j]) {
                            a[lt][ri] *= 2;
                            a[i][j] = 0;
                        } else {
                            if (a[lt][ri] == 0) {
                                a[lt][ri] = a[i][j];
                                a[i][j] = 0;
                            } else {
                                a[lt][--ri] = a[i][j];
                                a[i][j] = 0;
                            }
                        }
                    } else
                        ri--;
                }
            }
        }
    }
    void Discord_game_2048::find_clear() {
        this->_clear_places.clear();
        for (int y = 0; y < static_cast<int>(std::size(this->_board)); y++) {
            for (int x = 0; x < static_cast<int>(std::size(this->_board)); x++) {
                if (!_board[y][x]) {
                    this->_clear_places.push_back({y, x});
                }
            }
        }
    }
    void Discord_game_2048::get_possible_moves() {
        this->_possible_moves = {{'w', false}, {'s', false}, {'a', false}, {'d', false}};
        for (int y = 0; y < static_cast<int>(std::size(this->_board)); y++) {
            for (int x = 0; x < static_cast<int>(std::size(this->_board[y])); x++) {
                if (this->_board[y][x] == 0) {
                    if (y && this->_board[y - 1][x]) {
                        this->_possible_moves['s'] = true;
                    }
                    if (y < static_cast<int>(std::size(this->_board) - 1) && this->_board[y + 1][x]) {
                        this->_possible_moves['w'] = true;
                    }
                    if (x && this->_board[y][x - 1]) {
                        this->_possible_moves['d'] = true;
                    }
                    if (x < static_cast<int>(std::size(this->_board[y]) - 1) && this->_board[y][x + 1]) {
                        this->_possible_moves['a'] = true;
                    }
                } else {
                    if ((y && this->_board[y - 1][x] == this->_board[y][x]) ||
                        (y < static_cast<int>(std::size(this->_board) - 1) &&
                         this->_board[y + 1][x] == this->_board[y][x])) {
                        this->_possible_moves['s'] = true;
                        this->_possible_moves['w'] = true;
                    }
                    if ((x && this->_board[y][x - 1] == this->_board[y][x]) ||
                        (x < static_cast<int>(std::size(this->_board[y]) - 1) &&
                         this->_board[y][x + 1] == this->_board[y][x])) {
                        this->_possible_moves['d'] = true;
                        this->_possible_moves['a'] = true;
                    }
                }
            }
        }
    }
    void Discord_game_2048::new_peace() {
        std::uniform_int_distribution<size_t> random_int{0ul, std::size(this->_clear_places) - 1};
        size_t id = random_int(this->_rand_obj);
        std::uniform_int_distribution<size_t> random_int2{0ul, 100ul};
        unsigned int to_put = 2;
        if (random_int2(this->_rand_obj) > 90) {
            to_put = 4;
        }
        this->_board[this->_clear_places[id][0]][this->_clear_places[id][1]] = to_put;
        this->_clear_places.erase(this->_clear_places.begin() + id);
    }

} // namespace gb
