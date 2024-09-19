//
// Created by ilesik on 9/18/24.
//

#include "discord_rubiks_cube_command_impl.hpp"

#include <src/modules/discord/discord_games/rubiks_cube/discord_rubiks_cube_game.hpp>

namespace gb {
    Discord_rubiks_cube_command_impl::Discord_rubiks_cube_command_impl() :
        Discord_rubiks_cube_command("discord_rubiks_cube_command", {}) {
        lobby_title = "Rubik's cube";
        lobby_description = "The Rubik's Cube is a 3-D combination puzzle originally invented in 1974 by Hungarian "
                            "sculptor and professor of architecture Ernő Rubik";
        lobby_image_url = "https://cdn.discordapp.com/attachments/1010981120554320003/1031936352591290458/unknown.png";
    }
    void Discord_rubiks_cube_command_impl::run() { Discord_rubiks_cube_command::run(); }

    void Discord_rubiks_cube_command_impl::init(const Modules &modules) {
        Discord_rubiks_cube_command::init(modules);
        _image_processing->cache_create(Discord_rubiks_cube_game::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("rubiks_cube", "Command to start rubik's cube game", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    this->command_start();
                    auto d = get_game_data_initialization("rubiks_cube");
                    auto game = std::make_unique<Discord_rubiks_cube_game>(d, std::vector<dpp::snowflake>{event.command.usr.id});
                    co_await game->run(event);
                    this->command_end();
                    co_return;
                },
                {"__**Rules**__:\n[External link](http://www.rubiksplace.com/)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will give you buttons to rotate sides of cube. There will be icons which shows directions of "
                 "rotation and what will be rotated. Note: blue buttons - rotate sides, grey buttons - rotate middle "
                 "part. To change perspective you need to place 'camera' button. there will be 6 buttons which "
                 "represents a directions of camera movement. To return back to moving parts press 'camera' button.",
                 {"game", "single-player"}}));
        });
    }

    void Discord_rubiks_cube_command_impl::stop() {
        _command_handler->remove_command("rubiks_cube");
        Discord_rubiks_cube_command::stop();
        _image_processing->cache_remove(Discord_rubiks_cube_game::get_image_generators());
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_rubiks_cube_command_impl>());
    }


} // namespace gb
