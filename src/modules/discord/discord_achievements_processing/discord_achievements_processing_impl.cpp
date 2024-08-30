//
// Created by ilesik on 8/29/24.
//

#include "discord_achievements_processing_impl.hpp"

namespace gb {
    Discord_achievements_processing_impl::Discord_achievements_processing_impl() :
        Discord_achievements_processing("discord_achievements_processing", {"achievements_processing","discord_bot"}) {}

    void Discord_achievements_processing_impl::stop() {}

    void Discord_achievements_processing_impl::run() {}

    void Discord_achievements_processing_impl::init(const Modules &modules) {
        _achievements_processing =
            std::static_pointer_cast<Achievements_processing>(modules.at("achievements_processing"));

        _bot =  std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
    }

    bool Discord_achievements_processing_impl::is_have_achievement(const std::string &name,
                                                                   const dpp::snowflake &user_id) {
        return _achievements_processing->is_have_achievement(name, user_id.str());
    }

    void Discord_achievements_processing_impl::activate_achievement(const std::string &name,
                                                                    const dpp::snowflake &user_id,
                                                                    const dpp::snowflake &channel_id) {
        if (this->is_have_achievement(name, user_id)) {
            return;
        }

        Achievement a = _achievements_processing->activate_achievement(name, user_id.str());

        dpp::message m;
        m.channel_id = channel_id;
        dpp::embed embed;
        embed.set_color(dpp::colors::gold)
            .set_title(std::format("🎉| New {}achievement unlocked!", (a.is_secret ? "Secret " : "")))
            .set_description(std::format("Unlocked by: {}", dpp::utility::user_mention(user_id)))
            .add_field("Achievement", a.name)
            .add_field("Description", a.description)
            .set_thumbnail(a.image_url);

        _bot->message_create(m.add_embed(embed));
    }


} // namespace gb
