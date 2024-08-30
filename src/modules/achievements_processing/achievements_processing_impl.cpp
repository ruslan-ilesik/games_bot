//
// Created by ilesik on 8/28/24.
//

#include "achievements_processing_impl.hpp"


namespace gb {
    Achievements_processing_impl::Achievements_processing_impl() :
        Achievements_processing("achievements_processing", {"admin_terminal", "database"}) {}

    void Achievements_processing_impl::stop() {
        _admin_terminal->remove_command("achievements_reload");
        _admin_terminal->remove_command("achievements_list");
        _db->remove_prepared_statement(_activate_achievement_stmt);
        _db->remove_prepared_statement(_check_achievement_stmt);
    }

    void Achievements_processing_impl::run() {}

    void Achievements_processing_impl::init(const Modules &modules) {
        _admin_terminal = std::static_pointer_cast<Admin_terminal>(modules.at("admin_terminal"));
        _db = std::static_pointer_cast<Database>(modules.at("database"));

        _activate_achievement_stmt = _db->create_prepared_statement("INSERT IGNORE INTO `achievements` (`name`,`user_id`,`time_opened`) VALUES (?,?,UTC_TIMESTAMP())");
        _check_achievement_stmt = _db->create_prepared_statement("SELECT EXISTS(SELECT `name` from `achievements` where `name`=? and `user_id`=?) as does_have");

        _admin_terminal->add_command(
            "achievements_reload", "Reloads achievements from a file.", "Arguments: no arguments",
            [this](const std::vector<std::string> &args) {
                try {
                    load_from_file();
                    std::cout << "Achievements processing reloaded achievements from a file." << std::endl;
                } catch (...) {
                    std::cout << "Achievements processing command error: Error loading achievements from file."
                              << std::endl;
                }
            });

        _admin_terminal->add_command("achievements_list", "List achievements loaded into Achievements manager.",
                                     "Arguments: no arguments", [this](const std::vector<std::string> &args) {
                                         std::shared_lock lk(_mutex);
                                         std::ostringstream output;
                                         int counter = 1;

                                         for (const auto &[key, achievement]: _achievements) {
                                             output << counter << ") Achievement: " << key << "\n";
                                             output << "   Name: " << achievement.name << "\n";
                                             output << "   Description: " << achievement.description << "\n";
                                             output << "   Image URL: " << achievement.image_url << "\n";
                                             output << "   Discord Emoji: " << achievement.discord_emoji.first << ":"
                                                    << achievement.discord_emoji.second << "\n";
                                             output << "   Is Secret: " << (achievement.is_secret ? "Yes" : "No")
                                                    << "\n\n";
                                             counter++;
                                         }

                                         output << "Total Achievements: " << _achievements.size() << std::endl;

                                         std::cout << output.str() << std::endl;
                                     });
        load_from_file();
    }
    Achievement Achievements_processing_impl::activate_achievement(const std::string &name,
                                                                   const std::string &user_id) {
        std::shared_lock lk(_mutex);
        if (!_achievements.contains(name)) {
            throw std::runtime_error("Achievement " + name + " does not exist in Achievements manager");
        }
        _db->background_execute_prepared_statement(_activate_achievement_stmt, name, user_id);
        return _achievements.at(name);
    }

    bool Achievements_processing_impl::is_have_achievement(const std::string &name, const std::string &user_id) {
        std::string r = sync_wait(_db->execute_prepared_statement(_check_achievement_stmt,name,user_id)).at(0)["does_have"];
        return r == "1";
    }

    void Achievements_processing_impl::load_from_file() {
        std::unique_lock lk(_mutex);
        std::string file_path = "./data/achievements.json";
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + file_path);
        }

        nlohmann::json j;
        file >> j;
        _achievements.clear();

        for (const auto &item: j["achievements"]) {
            std::string name = item["name"];
            std::string description = item["description"];
            std::string image_url = item["image_url"];
            bool is_secret = item["is_secret"];
            std::pair<std::string, std::string> discord_emoji = {item["discord_emoji"]["name"],
                                                                 item["discord_emoji"]["id"]};
            if (_achievements.contains(name)) {
                throw std::runtime_error("Achievements processing error, achievement " + name +
                                         " already exist, check your achievements config file");
            }
            _achievements.insert({name, Achievement(name, description, image_url, is_secret, discord_emoji)});
        }
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Achievements_processing_impl>()); }

} // namespace gb
