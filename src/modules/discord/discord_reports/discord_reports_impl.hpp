//
// Created by ilesik on 10/1/24.
//

#pragma once

#include <dpp/timer.h>
#include <src/modules/config/config.hpp>
#include <src/modules/discord/discord_bot/discord_bot.hpp>
#include <src/modules/discord/discord_statistics_collector/discord_statistics_collector.hpp>

#include "./discord_reports.hpp"

namespace gb {

/**
 * @class Discord_reports_impl
 * @brief Implementation of Discord bot reporting functionality.
 *
 * This class is responsible for collecting and submitting Discord bot statistics (such as the number of servers
 * and users) to various external bot listing services. It interacts with the Discord bot and statistics collector
 * modules and ensures the data is periodically submitted.
 */
class Discord_reports_impl: public Discord_reports {
  /**
   * @brief Pointer to the Discord statistics collector module.
   *
   * This module is used to gather statistics such as the number of servers and users in which the bot is active.
   */
  Discord_statistics_collector_ptr _discord_stats;

  /**
   * @brief Pointer to the Discord bot module.
   *
   * This represents the Discord bot instance and allows interaction with the bot's API, such as setting presence
   * and sending HTTP requests.
   */
  Discord_bot_ptr _bot;

  /**
   * @brief Pointer to the configuration module.
   *
   * The configuration module is used to retrieve tokens and other necessary configuration details for making
   * requests to bot listing websites.
   */
  Config_ptr _config;

  /**
   * @brief Timer object for scheduling periodic tasks.
   *
   * This timer is used to schedule the periodic submission of bot statistics to external services.
   */
  dpp::timer _loop_timer;

public:
  /**
   * @brief Constructor for the Discord_reports_impl class.
   *
   * Initializes the class and sets up dependencies for reporting functionality.
   */
  Discord_reports_impl();

  /**
   * @brief Starts the reporting process.
   *
   * This method starts the periodic timer to submit bot statistics at regular intervals to the configured
   * external bot listing services.
   */
  void run() override;

  /**
   * @brief Stops the reporting process.
   *
   * This method stops the periodic timer and halts any further submission of bot statistics.
   */
  void stop() override;

  /**
   * @brief Initializes the module with the necessary dependencies.
   *
   * This method initializes the module by retrieving the required dependencies (such as the Discord bot,
   * statistics collector, and configuration modules) from the module system.
   *
   * @param modules A map of available modules from which dependencies are acquired.
   */
  void init(const Modules &modules) override;
};

/**
 * @brief Factory function to create a new instance of Discord_reports_impl.
 *
 * @return A shared pointer to the newly created Discord_reports_impl instance.
 */
extern "C" Module_ptr create();

} // gb
