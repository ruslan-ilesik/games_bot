//
// Created by ilesik on 8/28/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

class Achievement {
public:
    std::string name;
    std::string description;
    std::string image_url;
    std::pair<std::string, std::string> discord_emoji;
    bool is_secret;

    Achievement(const std::string &name, const std::string &description,
                const std::string &image_url, bool is_secret = false,
                const std::pair<std::string, std::string> &discord_emoji = {"",
                                                                            ""})
        : name(name), description(description), image_url(image_url),
          is_secret(is_secret), discord_emoji(discord_emoji) {}
};

class Achievements_processing : public Module {
public:
    Achievements_processing(const std::string &name,
                            const std::vector<std::string> &dependencies)
        : Module(name, dependencies){};

    virtual Achievement activate_achievement(const std::string& name, const std::string& user_id) = 0;

    virtual bool is_have_achievement(const std::string& name, const std::string& user_id) = 0;
};

    typedef std::shared_ptr<Achievements_processing> Achievements_processing_ptr;

} // namespace gb
