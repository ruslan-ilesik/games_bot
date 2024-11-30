//
// Created by ilesik on 9/18/24.
//

#include "discord_rubiks_cube_game.hpp"

namespace rubiks_cube {
    gb::Vector2i isometric_data::get_dot(gb::Vector2d coord) {
        return {static_cast<int>(start_p.x + coord.x * vector_x.x + coord.y * vector_y.x),
                static_cast<int>(start_p.y + coord.y * vector_y.y + coord.x * vector_x.y)};
    }

    isometric_data::isometric_data(gb::Vector2d start_p, gb::Vector2d vector_x, gb::Vector2d vector_y) :
        start_p(start_p), vector_x(vector_x), vector_y(vector_y) {}
} // namespace rubiks_cube


namespace gb {
    Discord_rubiks_cube_game::Discord_rubiks_cube_game(Game_data_initialization &_data,
                                                       const std::vector<dpp::snowflake> &players) :
        Discord_game(_data, players) {}


    std::vector<std::pair<std::string, image_generator_t>> Discord_rubiks_cube_game::get_image_generators() {
        return {};
    }

    void Discord_rubiks_cube_game::prepare_message(dpp::message &message) {
        message.components.clear();
        message.embeds[0]
            .set_title("Rubik`s cube game")
            .set_description(std::format(
                "Player: {}\nAmount of moves: {}\nTimeout: <t:{}:R>", dpp::utility::user_mention(get_current_player()),
                _amount_moves,
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                        .count() +
                    60));
        if (_is_view) {
            message
                .add_component(
                    dpp::component()
                        .add_component(dpp::component().set_id("rotate left up").set_emoji("wlup", 1034473536762433577))
                        .add_component(
                            dpp::component().set_id("rotate right up").set_emoji("wrup", 1034473540776378479)))
                .add_component(
                    dpp::component()
                        .add_component(dpp::component().set_id("rotate left").set_emoji("wvleft", 1034473542248579092))
                        .add_component(
                            dpp::component().set_id("rotate right").set_emoji("wvright", 1034473543414595695)))
                .add_component(
                    dpp::component()
                        .add_component(
                            dpp::component().set_id("rotate left down").set_emoji("wldown", 1034473534774321373))
                        .add_component(
                            dpp::component().set_id("rotate right down").set_emoji("wrdown", 1034473538842791946))
                        .add_component(
                            dpp::component().set_id("moving mode").set_emoji("zcamera", 1034467866726121592)));
        } else {
            message
                .add_component(
                    dpp::component()
                        .add_component(
                            dpp::component().set_id("left left up").set_emoji("lleftup", 1034201296644882472))
                        .add_component(
                            dpp::component().set_id("left right up").set_emoji("lrightup", 1034201300570755083))
                        .add_component(dpp::component().set_id("rmode").set_emoji("zcamera", 1034467866726121592))
                        .add_component(
                            dpp::component().set_id("right left up").set_emoji("rleftup", 1034201274712866888))
                        .add_component(
                            dpp::component().set_id("right right up").set_emoji("rrightup", 1034201279850872832)))
                .add_component(
                    dpp::component()
                        .add_component(
                            dpp::component().set_id("vertical up left").set_emoji("vupleft", 1034201288067535039))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("left center up")
                                           .set_emoji("lcenterup", 1034201293264269342))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("vertical center left")
                                           .set_emoji("vcenterleft", 1034201281520226404))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("right center up")
                                           .set_emoji("rcenterup", 1034201305297731747))
                        .add_component(
                            dpp::component().set_id("vertical up right").set_emoji("vupright", 1034201290084987000)))
                .add_component(
                    dpp::component()
                        .add_component(
                            dpp::component().set_id("vertical down left").set_emoji("vdownleft", 1034201284363956305))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("left center down")
                                           .set_emoji("lcenterdown", 1034201291775291443))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("vertical center right")
                                           .set_emoji("vcenterright", 1034201282812071946))
                        .add_component(dpp::component()
                                           .set_style(dpp::cos_secondary)
                                           .set_id("right center down")
                                           .set_emoji("rcenterdown", 1034201303078928405))
                        .add_component(dpp::component()
                                           .set_id("vertical down right")
                                           .set_emoji("vdownright", 1034201285878087750)))
                .add_component(
                    dpp::component()
                        .add_component(
                            dpp::component().set_id("left left down").set_emoji("lleftdown", 1034201294761623552))
                        .add_component(
                            dpp::component().set_id("left right down").set_emoji("lrightdown", 1034201298783969280))
                        .add_component(dpp::component().set_id("rmode2").set_emoji("zcamera", 1034467866726121592))
                        .add_component(
                            dpp::component().set_id("right left down").set_emoji("rleftdown", 1034201272716382218))
                        .add_component(
                            dpp::component().set_id("right right down").set_emoji("rrightdown", 1034201277145559072)));
        }

        message.embeds[0].set_image(add_image(message, create_image()));
    }

    Image_ptr Discord_rubiks_cube_game::create_image() {

        _img_cnt++;
        int image_size = 256;
        int block_size = image_size / 8;
        int offset = (image_size - block_size) / 6;
        Vector2d cube_size(std::pow(27.0, 0.5) * block_size, 6 * block_size);
        Vector2d offset_for_middle((image_size - cube_size.x) / 2, (image_size - cube_size.y) / 2);
        Image_ptr img = _data.image_processing->create_image({image_size, image_size}, {187, 173, 160});
        rubiks_cube::isometric_data sides[3] = {
            rubiks_cube::isometric_data(
                {static_cast<float>(offset_for_middle.x + std::pow(3.0 / 4.0, 0.5) * block_size * 3),
                 static_cast<float>(offset_for_middle.y + 0 * block_size)},
                {static_cast<float>(std::pow(3.0 / 4.0, 0.5)), 0.5},
                {static_cast<float>(-std::pow(3.0 / 4.0, 0.5)), 0.5}),
            rubiks_cube::isometric_data({static_cast<float>(offset_for_middle.x + 0 * block_size),
                                         static_cast<float>(offset_for_middle.y + 0.5 * block_size * 3)},
                                        {static_cast<float>(std::pow(3.0 / 4.0, 0.5)), 0.5}, {0, 1}),
            rubiks_cube::isometric_data(
                {static_cast<float>(offset_for_middle.x + std::pow(3.0 / 4.0, 0.5) * block_size * 3),
                 static_cast<float>(offset_for_middle.y + 1 * block_size * 3)},
                {static_cast<float>(std::pow(3.0 / 4.0, 0.5)), -0.5}, {0, 1})};

        _engine.update_for_draw();
        for (int y = 0; y < static_cast<int>(std::size(_engine.draw[0])); y++) {
            for (int x = 0; x < static_cast<int>(std::size(_engine.draw[0][y])); x++) {
                for (int i = 0; i < static_cast<int>(std::size(sides)); i++) {
                    std::vector<Vector2i> contour;
                    contour.push_back(sides[i].get_dot(Vector2d(0 + block_size * x, 0 + block_size * y)));
                    contour.push_back(sides[i].get_dot(Vector2d(0 + block_size * x, block_size + block_size * y)));
                    contour.push_back(
                        sides[i].get_dot(Vector2d(block_size + block_size * x, block_size + block_size * y)));
                    contour.push_back(sides[i].get_dot(Vector2d(block_size + block_size * x, 0 + block_size * y)));
                    img->draw_polygon(contour, rubiks_cube::colors.at(_engine.draw[i][y][x]), {0, 0, 0},
                                      image_size / 70);
                }
            }
        }
        return img;
    }

    bool Discord_rubiks_cube_game::is_win() {
        for (int i = 0; i < static_cast<int>(std::size(_engine.cube)); i++) {
            for (int y = 0; y < static_cast<int>(std::size(_engine.cube[i])); y++) {
                for (int x = 0; x < static_cast<int>(std::size(_engine.cube[i][y])); x++) {
                    if (_engine.cube[i][y][x] != _engine.cube[i][0][0]) {
                        return false;
                    }
                }
            }
        }
        return true;
    };


    dpp::task<void> Discord_rubiks_cube_game::run(dpp::slashcommand_t sevent) {
        game_start(sevent.command.channel_id, sevent.command.guild_id);


        // seeding and making puzzle
        _rand_obj.seed(std::chrono::system_clock::now().time_since_epoch().count());
        using func_type = void (rubiks_cube::Rubiks_cube_engine::*)();
        constexpr func_type f[] = {&rubiks_cube::Rubiks_cube_engine::b, &rubiks_cube::Rubiks_cube_engine::d,
                                   &rubiks_cube::Rubiks_cube_engine::e, &rubiks_cube::Rubiks_cube_engine::f,
                                   &rubiks_cube::Rubiks_cube_engine::l, &rubiks_cube::Rubiks_cube_engine::m,
                                   &rubiks_cube::Rubiks_cube_engine::r, &rubiks_cube::Rubiks_cube_engine::s,
                                   &rubiks_cube::Rubiks_cube_engine::u};
        for (int i = 0; i < 41; i++) {
            std::uniform_int_distribution<size_t> random_int{0ul, std::size(f) - 1};
            size_t id = random_int(_rand_obj);
            (_engine.*(f[id]))();
        }

        dpp::message message;
        message.add_embed(dpp::embed());
        message.channel_id = sevent.command.channel_id;
        message.guild_id = sevent.command.guild_id;
        message.id = 0;

        Button_click_return r;
        prepare_message(message);
        dpp::task<Button_click_return> button_click_awaitable =
        _data.button_click_handler->wait_for(message, {get_current_player()}, 60);
        _data.bot->reply(sevent, message);
        r = co_await button_click_awaitable;
        dpp::button_click_t event;
        while (1) {
            if (r.second) {
                message.components.clear();
                message.embeds[0]
                    .set_title("Game Timeout!!")
                    .set_description("If you still want to play, you can create new game.\nPlayer " +
                                     dpp::utility::user_mention(get_current_player()) +
                                     " lost his game.\nMoves amount: " + std::to_string(_amount_moves))
                .set_color(dpp::colors::red);
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


            if (_is_view) {
                if (event.custom_id == "rotate right up") {
                    _engine.x();
                } else if (event.custom_id == "rotate left up") {
                    _engine.z();
                } else if (event.custom_id == "rotate left") {
                    _engine.y();
                } else if (event.custom_id == "rotate right") {
                    for (int i = 0; i < 3; i++) {
                        _engine.y();
                    }
                } else if (event.custom_id == "rotate right down") {
                    for (int i = 0; i < 3; i++) {
                        _engine.x();
                    }
                } else if (event.custom_id == "rotate left down") {
                    for (int i = 0; i < 3; i++) {
                        _engine.z();
                    }
                } else if (event.custom_id == "moving mode") {
                    _is_view = false;
                }
            }

            else {
                _amount_moves++;
                if (event.custom_id == "left left up") {
                    for (int i = 0; i < 3; i++) {
                        _engine.b();
                    }
                } else if (event.custom_id == "left right up") {
                    _engine.f();
                } else if (event.custom_id == "right left up") {
                    for (int i = 0; i < 3; i++) {
                        _engine.l();
                    }
                } else if (event.custom_id == "right right up") {
                    _engine.r();
                } else if (event.custom_id == "vertical up left") {
                    _engine.u();
                } else if (event.custom_id == "left center up") {
                    _engine.s();
                } else if (event.custom_id == "vertical center left") {
                    for (int i = 0; i < 3; i++) {
                        _engine.e();
                    }
                } else if (event.custom_id == "right center up") {
                    for (int i = 0; i < 3; i++) {
                        _engine.m();
                    }
                } else if (event.custom_id == "vertical up right") {
                    for (int i = 0; i < 3; i++) {
                        _engine.u();
                    }
                } else if (event.custom_id == "vertical down left") {
                    for (int i = 0; i < 3; i++) {
                        _engine.d();
                    }
                } else if (event.custom_id == "left center down") {
                    for (int i = 0; i < 3; i++) {
                        _engine.s();
                    }
                } else if (event.custom_id == "vertical center right") {
                    _engine.e();
                } else if (event.custom_id == "right center down") {
                    _engine.m();
                } else if (event.custom_id == "vertical down right") {
                    _engine.d();
                } else if (event.custom_id == "left left down") {
                    _engine.b();
                } else if (event.custom_id == "left right down") {
                    for (int i = 0; i < 3; i++) {
                        _engine.f();
                    }
                } else if (event.custom_id == "right left down") {
                    _engine.l();
                } else if (event.custom_id == "right right down") {
                    for (int i = 0; i < 3; i++) {
                        _engine.r();
                    }
                } else if (event.custom_id == "rmode2" || event.custom_id == "rmode") {
                    _is_view = true;
                    _amount_moves--;
                }
            }
            if (is_win()) {
                // win
                if (_amount_moves < 21) {
                    _data.achievements_processing->activate_achievement("3.134 seconds", get_current_player(),
                                                                        event.command.channel_id);
                }
                message.components.clear();
                message.embeds[0]
                    .set_title("Game over!!")
                    .set_description("Player " + dpp::utility::user_mention(get_current_player()) +
                                     " solved Rubik`s cube in " + std::to_string(_amount_moves) + " moves.")
                    .set_color(dpp::colors::green);
                message.embeds[0].set_image(add_image(message, create_image()));
                _data.bot->reply(event, message);
                remove_player(USER_REMOVE_REASON::WIN, get_current_player());
                break;
            }
            prepare_message(message);
            button_click_awaitable = _data.button_click_handler->wait_for(message, {get_current_player()}, 60);
            _data.bot->reply(event, message);
            r = co_await button_click_awaitable;
        }

        nlohmann::json json{{"moves", _amount_moves}};
        game_stop(json.dump());
        co_return;
    }
} // namespace gb
