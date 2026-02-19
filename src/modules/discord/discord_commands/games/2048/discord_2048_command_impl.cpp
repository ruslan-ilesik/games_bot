//
// Created by ilesik on 9/29/24.
//

#include "discord_2048_command_impl.hpp"
#include <src/modules/discord/discord_games/2048/discord_game_2048.hpp>
namespace gb {
    dpp::task<void> Discord_2048_command_impl::_command_callback(const dpp::slashcommand_t &event) {

        auto d = get_game_data_initialization("2048");
        auto game = std::make_unique<Discord_game_2048>(d, std::vector<dpp::snowflake>{event.command.usr.id});
        co_await game->start_game(event);
        co_return;
    }

    Discord_2048_command_impl::Discord_2048_command_impl() : Discord_2048_command("discord_2048_command", {}) {
        lobby_title = "2048";
        lobby_description =
            "2048 is a single-player sliding tile puzzle video game written by Italian web developer Gabriele Cirulli";
        lobby_image_url = "https://cdn.discordapp.com/attachments/1010981120554320003/1031556299919474708/unknown.png";
    }
    void Discord_2048_command_impl::init(const Modules &modules) {
        Discord_2048_command::init(modules);
        _image_processing->cache_create(Discord_game_2048::get_image_generators());
        _bot->add_pre_requirement([this]() {
            dpp::slashcommand command("2048", "Command to start 2048 game", _bot->get_bot()->me.id);

            _command_handler->register_command(_discord->create_discord_command(
                command, _command_executor,
                {"__**Rules**__:\n[External link](https://levelskip.com/puzzle/How-to-play-2048/)"
                 "\n\n\n__**How does it works in bot?**__\n"
                 "Bot will show you grid with buttons, just choose direction (same as with arrows in original game) "
                 "and that`s all :)",
                 {"game", "single-player"}}));
        });
    }
    void Discord_2048_command_impl::run() { Discord_2048_command::run(); }

    void Discord_2048_command_impl::stop() {
        _command_handler->remove_command("2048");
        Discord_2048_command::stop();
        _image_processing->cache_remove(Discord_game_2048::get_image_generators());
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_2048_command_impl>()); }
} // namespace gb
