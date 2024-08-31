//
// Created by ilesik on 8/30/24.
//

#include "discord_command_achievements_impl.hpp"

#include <src/modules/discord/discord/discord.hpp>

namespace gb {
    void Discord_command_achievements_impl::init(const Modules &modules) {
        _achievements_processing =
            std::static_pointer_cast<Discord_achievements_processing>(modules.at("discord_achievements_processing"));
        this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_command_handler =
            std::static_pointer_cast<Discord_command_handler>(modules.at("discord_command_handler"));
        auto discord = std::static_pointer_cast<Discord>(modules.at("discord"));

        _discord_bot->add_pre_requirement([this, discord]() {
            dpp::slashcommand command("achievements", "Command to get information about user achievements.",
                                      _discord_bot->get_bot()->me.id);
            command.add_option(dpp::command_option(dpp::co_string, "filter", "What achievements to show", false)
                                   .add_choice(dpp::command_option_choice("All", std::string("all")))
                                   .add_choice(dpp::command_option_choice("Unlocked only", std::string("unlocked")))
                                   .add_choice(dpp::command_option_choice("Locked only", std::string("locked"))));

            _command_handler->register_command(discord->create_discord_command(
                command,
                [this](const dpp::slashcommand_t &event) -> dpp::task<void> {
                    command_start();
                    const auto parameter = event.get_parameter("filter");
                    std::string filter = "all";
                    if (std::holds_alternative<std::string>(parameter)) {
                        filter = std::get<std::string>(parameter);
                        if (filter == "locked") {
                            _achievements_processing->activate_achievement(
                                "In Love with the Unobtained", event.command.usr.id, event.command.channel_id);
                        }
                    }
                    Achievements_report user_achievements =
                        _achievements_processing->get_achievements_report(event.command.usr.id);
                    dpp::message m;
                    if (filter == "unlocked" || filter == "all") {
                        dpp::embed e = dpp::embed().set_title("Unlocked achievements").set_color(dpp::colors::green);
                        for (auto &achievement_data: user_achievements.unlocked_usual) {
                            auto d_emoji = achievement_data.first.discord_emoji ;
                            e.add_field(
                                std::format("{} | {}", dpp::emoji(d_emoji.first,d_emoji.second).get_mention(),
                                            achievement_data.first.name),
                                std::format("{}\nUnlocked at: {}", achievement_data.first.description,
                                            dpp::utility::timestamp(achievement_data.second,
                                                                    dpp::utility::time_format::tf_long_datetime)),
                                false);
                        }
                        m.add_embed(e);
                        e = dpp::embed().set_title("Unlocked secret achievements").set_color(dpp::colors::green);
                        for (auto &achievement_data: user_achievements.unlocked_secret) {
                            auto d_emoji = achievement_data.first.discord_emoji ;
                            e.add_field(
                                std::format("{} | {}", dpp::emoji(d_emoji.first,d_emoji.second).get_mention(),
                                            achievement_data.first.name),
                                std::format("{}\nUnlocked at: {}", achievement_data.first.description,
                                            dpp::utility::timestamp(achievement_data.second,
                                                                    dpp::utility::time_format::tf_long_datetime)),
                                false);
                        }
                        m.add_embed(e);
                    }
                    if (filter == "locked" || filter == "all") {
                        dpp::embed e = dpp::embed().set_title("Locked achievements");

                        for (auto &achievement: user_achievements.locked_usual) {
                            e.add_field(std::format("{} | {}",
                                                    dpp::utility::emoji_mention("hidden", 1178025079200825456, false),
                                                    achievement.name),
                                        achievement.description, false);
                        }
                        e.add_field(
                            std::format("{} | {}", dpp::utility::emoji_mention("hidden", 1178025079200825456, false),
                                        "secret achievements"),
                            std::format("{} secret achievements are locked", user_achievements.locked_secret.size()),
                            false);
                        m.add_embed(e);
                    }
                    m.set_flags(dpp::m_ephemeral);
                    _discord_bot->reply(event,m);
                    command_end();
                    co_return;
                },
                {"Gives detailed information about user achievements, sorted by category (secret / not secret) and "
                 "allows to filter achievements which will be shown (unlocked, not unlocked).",
                 {"other"}}));
        });
    }

    void Discord_command_achievements_impl::run() {}
    Module_ptr create() {
        return std::dynamic_pointer_cast<Discord_command_achievements>(
            std::make_shared<Discord_command_achievements_impl>());
    }

    Discord_command_achievements_impl::Discord_command_achievements_impl():Discord_command_achievements("discord_command_achievements",{"discord_achievements_processing","discord_command_handler","discord_bot","discord"}){}
} // namespace gb
