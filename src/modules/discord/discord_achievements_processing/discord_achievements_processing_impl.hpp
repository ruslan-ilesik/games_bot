//
// Created by ilesik on 8/29/24.
//

#pragma once

#include <src/modules/achievements_processing/achievements_processing.hpp>
#include <src/modules/discord/discord_bot/discord_bot.hpp>
#include "./discord_achievements_processing.hpp"

namespace gb {

    /**
     * @class Discord_achievements_processing_impl
     * @brief Implementation of Discord achievements processing.
     *
     * This class is responsible for managing and processing achievements for users in a Discord environment.
     * It interacts with both the `Achievements_processing` module and the `Discord_bot` module.
     */
    class Discord_achievements_processing_impl : public Discord_achievements_processing {

        /**
         * @brief Pointer to the `Achievements_processing` module.
         */
        Achievements_processing_ptr _achievements_processing;

        /**
         * @brief Pointer to the `Discord_bot` module.
         */
        Discord_bot_ptr _bot;

    public:
        /**
         * @brief Constructs a Discord_achievements_processing_impl object.
         */
        Discord_achievements_processing_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Discord_achievements_processing_impl() = default;

        /**
         * @brief Stops the module and performs necessary cleanup.
         */
        void stop() override;

        /**
         * @brief Runs the module's primary operations.
         */
        void run() override;

        /**
         * @brief Initializes the module with its dependencies.
         *
         * @param modules A map containing pointers to the required modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Checks if a Discord user has a specific achievement.
         *
         * @param name The name of the achievement to check.
         * @param user_id The ID of the Discord user to check.
         * @return True if the user has the achievement, false otherwise.
         */
        bool is_have_achievement(const std::string &name, const dpp::snowflake &user_id) override;

        /**
         * @brief Activates an achievement for a specific user within a Discord channel.
         *
         * @param name The name of the achievement to activate.
         * @param user_id The ID of the Discord user for whom the achievement is activated.
         * @param channel_id The ID of the Discord channel where the activation is to be notified.
         */
        void activate_achievement(const std::string &name, const dpp::snowflake &user_id,
                                  const dpp::snowflake &channel_id) override;


        /**
         * @brief Retrieves an achievements report for a specific Discord user.
         *
         * This method calls the underlying achievements processing system (look into Achievements_processing class for
         * more details) to generate an `Achievements_report` for the given Discord `snowflake` ID.
         *
         * @param user_id The Discord `snowflake` ID of the user.
         * @return Achievements_report The achievements report for the specified user.
         */
        Achievements_report get_achievements_report(const dpp::snowflake &user_id) override;
    };

    /**
     * @brief Factory function to create a new instance of Discord_achievements_processing_impl.
     *
     * @return A shared pointer to the created module instance.
     */
    extern "C" Module_ptr create();

} // namespace gb
