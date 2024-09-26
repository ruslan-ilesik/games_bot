//
// Created by ilesik on 9/19/24.
//

#include "discord_minesweeper_game.hpp"

namespace minesweeper {
    gb::Color ground[] = {{170, 215, 81}, {162, 209, 73}};
    gb::Color digged_ground[] = {{229, 194, 159}, {215, 184, 153}};
    gb::Color numbers[] = {{55, 124, 185}, {67, 138, 67}, {180, 61, 47}, {56, 129, 146},
                           {255, 143, 0},  {0, 151, 167}, {66, 66, 66},  {168, 158, 146}};
    std::array<std::array<gb::Color, 2>, 8> mines = {
        std::array<gb::Color, 2>{gb::Color(237, 68, 181), gb::Color(154, 44, 118)},
        std::array<gb::Color, 2>{gb::Color(72, 133, 237), gb::Color(47, 86, 154)},
        std::array<gb::Color, 2>{gb::Color(182, 72, 242), gb::Color(118, 47, 157)},
        std::array<gb::Color, 2>{gb::Color(244, 194, 13), gb::Color(159, 126, 8)},
        std::array<gb::Color, 2>{gb::Color(0, 135, 68), gb::Color(0, 88, 44)},
        std::array<gb::Color, 2>{gb::Color(244, 132, 13), gb::Color(159, 86, 8)},
        std::array<gb::Color, 2>{gb::Color(219, 50, 54), gb::Color(142, 33, 35)},
        std::array<gb::Color, 2>{gb::Color(72, 230, 241), gb::Color(47, 150, 157)}};


    std::array<gb::Color, 2> side_num = {{{255, 255, 255}, {251, 192, 45}}};
} // namespace minesweeper


namespace gb {

    Discord_minesweeper_game::Discord_minesweeper_game(Game_data_initialization &_data,
                                                       const std::vector<dpp::snowflake> &players, int level) :
        Discord_game(_data, players), _engine(level), _level(level) {
        _timeout_time = std::array<int, 3>({2 * 60, 3 * 60, 5 * 60})[level - 1];
    }

    std::vector<std::pair<std::string, image_generator_t>> Discord_minesweeper_game::get_image_generators() {
        std::vector<std::pair<std::string, image_generator_t>> generators;
        std::string file_name = "./data/img/minesweeper/flag.png";
        generators.emplace_back("minesweeper_flag",
                                [=](const Image_processing_ptr &image_processing, const Vector2i &resolution) {
                                    Image_ptr img = image_processing->create_image(file_name);
                                    img->resize(resolution);
                                    return img;
                                });
        return generators;
    }
    void Discord_minesweeper_game::prepare_message(dpp::message &message) {
        message.components.clear();
        message.embeds[0]
            .set_title("Minesweeper game")
            .set_description(std::format(
                "Player: {}\nState: {}\nFlags left: **{}**\nTimeout: <t:{}:R>",
                dpp::utility::user_mention(get_current_player()),
                (_state == SELECT_COL ? "select column"
                                      : (_state == SELECT_ROW ? "select row" : "select action to do")),
                _engine.flags_cnt,
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                        .count() +
                    _timeout_time));

        int start_point = _next_btn ? 24 : 0;

        if (_state == SELECT_COL || _state == SELECT_ROW) {
            auto p_m = _engine.get_possible_moves(_state == SELECT_ROW ? _action_position[0] : -1);
            for (int i = start_point; i < static_cast<int>(p_m.size()); i++) {
                if ((i - start_point) % 5 == 0) {
                    message.components.push_back(dpp::component().set_type(dpp::cot_action_row));
                }
                message.components[message.components.size() - 1].add_component(
                    dpp::component()
                        .set_type(dpp::cot_button)
                        .set_label(std::string(1, _numeration[p_m[i]]))
                        .set_id(std::to_string(p_m[i])));

                if (i - start_point == 23) {
                    message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                        .set_type(dpp::cot_button)
                                                                                        .set_style(dpp::cos_success)
                                                                                        .set_label("next")
                                                                                        .set_id("next")
                                                                                        .set_emoji("➡"));
                    break;
                }
            }
            if (start_point || _state == SELECT_ROW) {
                if (message.components[message.components.size() - 1].components.size() == 5) {
                    message.components.push_back(dpp::component().set_type(dpp::cot_action_row));
                }
                message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                    .set_type(dpp::cot_button)
                                                                                    .set_style(dpp::cos_danger)
                                                                                    .set_label("back")
                                                                                    .set_id("back")
                                                                                    .set_emoji("⬅"));
            }
        }

        else {
            message.components.push_back(dpp::component().set_type(dpp::cot_action_row));

            if (_engine.visible_board[_action_position[0]][_action_position[1]] == 0) {
                message.components[message.components.size() - 1].add_component(
                    dpp::component()
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_label("dig")
                        .set_id("dig")
                        .set_emoji("spade", 1075102678234968147));
            }
            if (_engine.visible_board[_action_position[0]][_action_position[1]] == 0 && _engine.flags_cnt) {
                message.components[message.components.size() - 1].add_component(
                    dpp::component()
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_label("place flag")
                        .set_id("place flag")
                        .set_emoji("flag", 1075102785051316234));
            }
            if (_engine.visible_board[_action_position[0]][_action_position[1]] == 10) {
                message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                    .set_type(dpp::cot_button)
                                                                                    .set_style(dpp::cos_primary)
                                                                                    .set_label("remove flag")
                                                                                    .set_id("remove flag")
                                                                                    .set_emoji("✖"));
                message.components[message.components.size() - 1].add_component(
                    dpp::component()
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_label("remove flag and dig")
                        .set_id("remove flag and dig")
                        .set_emoji("spade", 1075102678234968147));
            }
            if ((_engine.visible_board[_action_position[0]][_action_position[1]] > 0 &&
                 _engine.visible_board[_action_position[0]][_action_position[1]] < 9) ||
                _engine.visible_board[_action_position[0]][_action_position[1]] == 11) {
                message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                    .set_type(dpp::cot_button)
                                                                                    .set_style(dpp::cos_primary)
                                                                                    .set_label("open all around")
                                                                                    .set_id("open all around")
                                                                                    .set_emoji("🔄"));
            }
            message.components[message.components.size() - 1].add_component(dpp::component()
                                                                                .set_type(dpp::cot_button)
                                                                                .set_style(dpp::cos_danger)
                                                                                .set_label("back")
                                                                                .set_id("back")
                                                                                .set_emoji("⬅"));
        }
        message.embeds[0].set_image(add_image(message, create_image()));
    }

    Image_ptr Discord_minesweeper_game::create_image() {
        _img_cnt++;
        int size_per_sector = 30;
        Image_ptr img = _data.image_processing->create_image(
            {static_cast<int>(size_per_sector + size_per_sector * _engine.visible_board.size()),
             static_cast<int>(size_per_sector + size_per_sector * _engine.visible_board[0].size())},
            {0, 0, 0});

        for (int x = 0; x < static_cast<int>(_engine.visible_board.size()); x++) {
            for (int y = 0; y < static_cast<int>(_engine.visible_board[0].size()); y++) {
                Vector2i s(size_per_sector * x, size_per_sector * y);
                img->draw_rectangle(s, {s.x + size_per_sector, s.y + size_per_sector},
                                    _engine.visible_board[x][y] == 0 || _engine.visible_board[x][y] == 10 ||
                                            _engine.visible_board[x][y] == 12
                                        ? minesweeper::ground[(x + y) % 2]
                                        : minesweeper::digged_ground[(x + y) % 2],
                                    -1);

                if (_engine.visible_board[x][y] > 0 && _engine.visible_board[x][y] < 9) {
                    img->draw_text(std::to_string(_engine.visible_board[x][y]),
                                   Vector2d{s.x + size_per_sector * 0.2, s.y + size_per_sector * 0.80},
                                   size_per_sector / 35.0, minesweeper::numbers[_engine.visible_board[x][y] - 1],
                                   size_per_sector / 15.0);

                } else if (_engine.visible_board[x][y] == 9) {
                    std::uniform_int_distribution<size_t> dist(0, std::size(minesweeper::mines) - 1);
                    auto colors = minesweeper::mines[dist(_rd)];
                    img->draw_rectangle(s, {s.x + size_per_sector, s.y + size_per_sector}, colors[0], -1);
                    img->draw_circle({s.x + size_per_sector / 2, s.y + size_per_sector / 2}, size_per_sector / 4,
                                     colors[1], -1);
                } else if (_engine.visible_board[x][y] == 10) {
                    Image_ptr flag_img =
                        _data.image_processing->cache_get("minesweeper_flag", {size_per_sector, size_per_sector});
                    img->overlay_image(flag_img, {size_per_sector * x, size_per_sector * y});
                } else if (_engine.visible_board[x][y] == 12) {
                    img->draw_text("X", Vector2d{s.x + size_per_sector * 0.2, s.y + size_per_sector * 0.80},
                                   size_per_sector / 35.0,
                                   {
                                       255,
                                       0,
                                       0,
                                   },
                                   size_per_sector / 15.0);
                }
            }
            Vector2d s(size_per_sector * x, size_per_sector * _engine.visible_board[0].size());
            img->draw_text(std::string(1, _numeration[x]),
                           Vector2d{s.x + size_per_sector * 0.2, s.y + size_per_sector * 0.80}, size_per_sector / 35.0,
                           minesweeper::side_num[_state == SELECT_COL], size_per_sector / 15.0);
        }
        for (int y = 0; y < static_cast<int>(_engine.visible_board[0].size()); y++) {
            Vector2d s(size_per_sector * _engine.visible_board.size(), y * size_per_sector);
            img->draw_text(std::string(1, _numeration[y]),
                           Vector2d{s.x + size_per_sector * 0.2, s.y + size_per_sector * 0.80}, size_per_sector / 35.0,
                           minesweeper::side_num[_state == SELECT_ROW], size_per_sector / 15.0);
        }
        // draw opacity color on selected column
        if (_state == SELECT_ROW || _state == SELECT_ACTION) {
            if (_state == SELECT_ACTION) {
                Vector2i pos = {_action_position[0] * size_per_sector, _action_position[1] * size_per_sector};
                img->draw_rectangle(pos, pos + Vector2i{size_per_sector, size_per_sector}, {125, 125, 0, 0.5}, -1);
            } else {
                Vector2i pos = {_action_position[0] * size_per_sector, 0};
                img->draw_rectangle(pos,
                                    pos + Vector2i{size_per_sector,
                                                   static_cast<int>(_engine.visible_board[0].size() * size_per_sector)},
                                    {125, 125, 0, 0.5}, -1);
            }
        }

        return img;
    }

    dpp::task<void> Discord_minesweeper_game::run(dpp::slashcommand_t sevent) {
        game_start(sevent.command.channel_id, sevent.command.guild_id);
        _rd.seed(std::chrono::system_clock::now().time_since_epoch().count());
        Button_click_return r;
        dpp::task<Button_click_return> button_click_awaitable;
        dpp::message message;
        message.channel_id = sevent.command.channel_id;
        message.guild_id = sevent.command.guild_id;
        message.id = 0;
        message.add_embed(dpp::embed());
        prepare_message(message);
        button_click_awaitable = _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
        _data.bot->reply(sevent, message);
        dpp::button_click_t event;
        while (1) {
            r = co_await button_click_awaitable;
            if (r.second) {
                // timeout
                message.components.clear();
                _state = END_OF_GAME;
                _engine.end_of_game();
                std::string desc = "If you still want to play, you can create a new game.\nPlayer " +
                                   dpp::utility::user_mention(get_current_player()) + " lost his game.\n";
                message.embeds[0].set_color(dpp::colors::red).set_title("Game Timeout.").set_description(desc);
                if (message.id != 0) {
                    _data.bot->message_edit(message);
                } else {
                    _data.bot->message_create(message);
                }
                remove_player(USER_REMOVE_REASON::TIMEOUT, get_current_player());
                break;
            }
            event = r.first;
            message.id = event.command.message_id;

            if (_state == SELECT_COL) {
                if (event.custom_id == "next") {
                    _next_btn = true;
                    prepare_message(message);
                    button_click_awaitable =
                        _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                    _data.bot->reply(event, message);
                    continue;
                }
                if (event.custom_id == "back") {
                    _next_btn = false;
                    prepare_message(message);
                    button_click_awaitable =
                        _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                    _data.bot->reply(event, message);
                    continue;
                }
                _action_position[0] = std::stoi(event.custom_id);
                _state = SELECT_ROW;
                _next_btn = false;
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                _data.bot->reply(event, message);
            } else if (_state == SELECT_ROW) {
                if (event.custom_id == "back") {
                    _next_btn = false;
                    _state = SELECT_COL;
                    prepare_message(message);
                    button_click_awaitable =
                        _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                    _data.bot->reply(event, message);
                    continue;
                }
                _action_position[1] = std::stoi(event.custom_id);
                _state = SELECT_ACTION;
                _next_btn = false;
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                _data.bot->reply(event, message);
            }

            else if (_state == SELECT_ACTION) {
                minesweeper_engine::STATES state;
                if (event.custom_id == "back") {
                    _next_btn = false;
                    _state = SELECT_ROW;
                    prepare_message(message);
                    button_click_awaitable =
                        _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                    _data.bot->reply(event, message);
                    continue;
                } else if (event.custom_id == "dig") {
                    if (_is_fst_move) {
                        _is_fst_move = false;
                        _engine.generate_safe_field(_action_position);
                    }
                    state = _engine.make_action(minesweeper_engine::DIG, _action_position);
                } else if (event.custom_id == "place flag") {
                    state = _engine.make_action(minesweeper_engine::PLACE_FLAG, _action_position);
                } else if (event.custom_id == "remove flag") {
                    state = _engine.make_action(minesweeper_engine::REMOVE_FLAG, _action_position);
                } else if (event.custom_id == "remove flag and dig") {
                    state = _engine.make_action(minesweeper_engine::REMOVE_FLAG_AND_DIG, _action_position);
                } else if (event.custom_id == "open all around") {
                    state = _engine.make_action(minesweeper_engine::OPEN_ALL_AROUND, _action_position);
                }

                if (state == minesweeper_engine::WIN) {
                    _engine.end_of_game();
                    _state = END_OF_GAME;

                    if (_level == 3) {
                        _data.achievements_processing->activate_achievement("7355608", get_current_player(),
                                                                            event.command.channel_id);
                    }
                    message.components.clear();
                    message.embeds[0]
                        .set_color(dpp::colors::green)
                        .set_title("Game over!!")
                        .set_description("Player: " + dpp::utility::user_mention(get_current_player()) +
                                         " marked all mines and **won** the game.");

                    message.embeds[0].set_image(add_image(message, create_image()));
                    _data.bot->reply(event, message);
                    remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                    break;
                } else if (state == minesweeper_engine::LOSE) {
                    _engine.end_of_game();
                    _state = END_OF_GAME;
                    message.components.clear();
                    message.embeds[0]
                        .set_color(dpp::colors::red)
                        .set_title("Game over!!")
                        .set_description("Player: " + dpp::utility::user_mention(get_current_player()) +
                                         "opened a mine and **lost** the game. You will be more lucky next time");

                    message.embeds[0].set_image(add_image(message, create_image()));
                    _data.bot->reply(event, message);
                    remove_player(USER_REMOVE_REASON::LOSE, get_current_player());
                    break;
                }

                _state = SELECT_COL;
                _next_btn = false;
                prepare_message(message);
                button_click_awaitable =
                    _data.button_click_handler->wait_for(message, {get_current_player()}, _timeout_time);
                _data.bot->reply(event, message);
            }
        }
        game_stop("{\"level\":"+std::to_string(_level)+"}");
        co_return;
    }
} // namespace gb
