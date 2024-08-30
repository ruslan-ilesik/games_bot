//
// Created by ilesik on 8/28/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

    /**
     * @class Achievement
     * @brief Represents an achievement in the system.
     *
     * This class holds the details of an achievement, including its name, description,
     * image URL, associated Discord emoji, and whether it is a secret achievement.
     */
    class Achievement {
    public:
        std::string name; ///< The name of the achievement.
        std::string description; ///< A brief description of the achievement.
        std::string image_url; ///< URL to the image representing the achievement.
        std::pair<std::string, std::string> discord_emoji; ///< Pair containing the Discord emoji name and ID.
        bool is_secret; ///< Indicates whether the achievement is a secret.

        /**
         * @brief Constructs an Achievement object.
         *
         * @param name The name of the achievement.
         * @param description A brief description of the achievement.
         * @param image_url URL to the image representing the achievement.
         * @param is_secret Indicates whether the achievement is a secret. Default is false.
         * @param discord_emoji A pair containing the Discord emoji name and ID. Default is an empty pair.
         */
        Achievement(const std::string &name, const std::string &description, const std::string &image_url,
                    bool is_secret = false, const std::pair<std::string, std::string> &discord_emoji = {"", ""}) :
            name(name), description(description), image_url(image_url), is_secret(is_secret),
            discord_emoji(discord_emoji) {}
    };

    /**
     * @class Achievements_processing
     * @brief Abstract base class for managing achievements.
     *
     * This class provides an interface for managing and processing achievements, including
     * activating achievements for users and checking if users have specific achievements.
     */
    class Achievements_processing : public Module {
    public:
        /**
         * @brief Constructs an Achievements_processing object.
         *
         * @param name The name of the module.
         * @param dependencies The list of module dependencies.
         */
        Achievements_processing(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies){};

        /**
         * @brief Activates an achievement for a specific user.
         *
         * @param name The name of the achievement to activate.
         * @param user_id The ID of the user for whom the achievement is activated.
         * @return The activated Achievement object.
         */
        virtual Achievement activate_achievement(const std::string &name, const std::string &user_id) = 0;

        /**
         * @brief Checks if a user has a specific achievement.
         *
         * @param name The name of the achievement to check.
         * @param user_id The ID of the user to check.
         * @return True if the user has the achievement, false otherwise.
         */
        virtual bool is_have_achievement(const std::string &name, const std::string &user_id) = 0;
    };

    /**
     * @typedef Achievements_processing_ptr
     * @brief A shared pointer type for Achievements_processing.
     */
    typedef std::shared_ptr<Achievements_processing> Achievements_processing_ptr;

} // namespace gb
