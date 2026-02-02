//
// Created by ilesik on 8/28/24.
//

#pragma once
#include <ctime>
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
     * @struct Achievements_report
     * @brief A structure to report the status of achievements for a user.
     *
     * This structure categorizes achievements into different types:
     * - `unlocked_usual`: A vector of pairs where each pair contains an `Achievement` and the timestamp when it was
     * unlocked. This list contains the usual (non-secret) achievements that have been unlocked by the user.
     * - `unlocked_secret`: Similar to `unlocked_usual`, but for secret achievements.
     * - `locked_usual`: A list of usual achievements that the user has not yet unlocked.
     * - `locked_secret`: A list of secret achievements that the user has not yet unlocked.
     */
    struct Achievements_report {
        std::vector<std::pair<Achievement, std::time_t>> unlocked_usual;
        std::vector<std::pair<Achievement, std::time_t>> unlocked_secret;
        std::vector<Achievement> locked_usual;
        std::vector<Achievement> locked_secret;
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

        /**
         * @brief Retrieves an achievements report for a specific user.
         *
         * This method generates an `Achievements_report` that categorizes the achievements
         * of a user based on whether they have been unlocked or remain locked. It differentiates
         * between usual (non-secret) and secret achievements, returning both the unlocked achievements
         * along with the time they were unlocked and the locked achievements that the user has yet to achieve.
         *
         * @param user_id The ID of the user for whom the achievements report is being generated.
         * @return Achievements_report A report detailing the user's unlocked and locked achievements.
         *
         * @throws std::runtime_error if the `time_opened` string cannot be parsed.
         *
         * The report contains:
         * - `unlocked_usual`: Usual achievements that the user has unlocked, along with the UTC timestamp of when they
         * were unlocked.
         * - `unlocked_secret`: Secret achievements that the user has unlocked, along with the UTC timestamp of when
         * they were unlocked.
         * - `locked_usual`: Usual achievements that the user has not yet unlocked.
         * - `locked_secret`: Secret achievements that the user has not yet unlocked.
         */
        virtual Achievements_report get_achievements_report(const std::string &user_id) = 0;
    };

    /**
     * @typedef Achievements_processing_ptr
     * @brief A shared pointer type for Achievements_processing.
     */
    typedef std::shared_ptr<Achievements_processing> Achievements_processing_ptr;

} // namespace gb
