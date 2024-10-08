//
// Created by ilesik on 8/28/24.
//

#pragma once
#include <nlohmann/json.hpp>
#include <src/modules/admin_terminal/admin_terminal.hpp>
#include <src/modules/database/database.hpp>
#include "./achievements_processing.hpp"

namespace gb {

    /**
     * @class Achievements_processing_impl
     * @brief Implementation of the Achievements_processing interface for managing achievements .
     *
     * This class handles the initialization, loading, and management of achievements. It interfaces with a
     * database and an admin terminal to manage user achievements.
     */
    class Achievements_processing_impl : public Achievements_processing {

        Admin_terminal_ptr _admin_terminal; ///< Pointer to the admin terminal module.
        Database_ptr _db; ///< Pointer to the database module.
        Prepared_statement _activate_achievement_stmt; ///< Prepared statement for activating an achievement.
        Prepared_statement _check_achievement_stmt; ///< Prepared statement for checking if a user has an achievement.
        Prepared_statement
            _get_user_achievements; ///< Prepared statement to get achievements unlocked by specific user.
        std::shared_mutex _mutex; ///< Mutex for thread-safe access to the achievements map.
        std::map<std::string, Achievement> _achievements; ///< Map of achievement names to Achievement objects.

        /**
         * @brief Loads achievements from a JSON file into the achievements map.
         *
         * This method reads a JSON file containing achievements data and populates the internal map of achievements.
         * Throws an exception if the file cannot be opened or if there are duplicate achievements in the file.
         */
        void load_from_file();

    public:
        /**
         * @brief Constructs an instance of Achievements_processing_impl.
         */
        Achievements_processing_impl();

        /**
         * @brief Default destructor.
         */
        virtual ~Achievements_processing_impl() = default;

        /**
         * @brief Stops the achievements processing, removing related commands and statements.
         *
         * This method is called to clean up resources and remove commands/statements related to achievements.
         */
        void stop() override;

        /**
         * @brief Runs the achievements processing.
         *
         * This method is currently a placeholder and does not perform any operations.
         */
        void run() override;

        /**
         * @brief Initializes the achievements processing with the provided modules.
         *
         * This method sets up necessary connections to the admin terminal and database modules.
         *
         * @param modules The modules to be used by this class.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Activates an achievement for a specific user.
         *
         * @param name The name of the achievement.
         * @param user_id The ID of the user.
         * @return The activated Achievement object.
         *
         * @throws std::runtime_error if the achievement does not exist in the map.
         */
        Achievement activate_achievement(const std::string &name, const std::string &user_id) override;

        /**
         * @brief Checks if a user has a specific achievement.
         *
         * @param name The name of the achievement.
         * @param user_id The ID of the user.
         * @return True if the user has the achievement, false otherwise.
         */
        bool is_have_achievement(const std::string &name, const std::string &user_id) override;

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
        Achievements_report get_achievements_report(const std::string &user_id) override;
    };

    /**
     * @brief Creates an instance of the Achievements_processing module.
     *
     * This function is used to create an instance of the Achievements_processing_impl class.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
