//
// Created by ilesik on 8/29/24.
//

#pragma once
#include <dpp/dpp.h>
#include <src/module/module.hpp>
#include <src/modules/achievements_processing/achievements_processing.hpp>

namespace gb {


  /**
   * @class Discord_achievements_processing
   * @brief Abstract base class for managing Discord achievements.
   *
   * This class provides an interface for processing achievements within a Discord environment,
   * including activating achievements for users and checking if users have specific achievements.
   */
  class Discord_achievements_processing : public Module {
  public:
    /**
     * @brief Constructs a Discord_achievements_processing object.
     *
     * @param name The name of the module.
     * @param dependencies The list of module dependencies.
     */
    Discord_achievements_processing(const std::string &name,
                  const std::vector<std::string> &dependencies)
          : Module(name, dependencies){};

    /**
     * @brief Activates an achievement for a specific user within a Discord channel.
     *
     * @param name The name of the achievement to activate.
     * @param user_id The ID of the Discord user for whom the achievement is activated.
     * @param channel_id The ID of the Discord channel where the activation is to be notified.
     */
    virtual void activate_achievement(const std::string& name, const dpp::snowflake& user_id, const dpp::snowflake& channel_id) = 0;

    /**
     * @brief Checks if a Discord user has a specific achievement.
     *
     * @param name The name of the achievement to check.
     * @param user_id The ID of the Discord user to check.
     * @return True if the user has the achievement, false otherwise.
     */
    virtual bool is_have_achievement(const std::string& name, const dpp::snowflake& user_id) = 0;


    /**
     * @brief Retrieves an achievements report for a specific Discord user.
     *
     * This method calls the underlying achievements processing system (look into Achievements_processing class for more
     * details) to generate an `Achievements_report` for the given Discord `snowflake` ID.
     *
     * @param user_id The Discord `snowflake` ID of the user.
     * @return Achievements_report The achievements report for the specified user.
     */
    virtual Achievements_report get_achievements_report(const dpp::snowflake &user_id) = 0;
  };

  /**
   * @typedef Discord_achievements_processing_ptr
   * @brief A shared pointer type for Discord_achievements_processing.
   */
  typedef std::shared_ptr<Discord_achievements_processing> Discord_achievements_processing_ptr;

} // namespace gb
