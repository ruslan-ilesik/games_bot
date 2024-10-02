//
// Created by ilesik on 9/30/24.
//

#pragma once

#include <src/modules/config/config.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord_bot/discord_bot.hpp>
#include "./discord_statistics_collector.hpp"

namespace gb {

/**
 * @class Discord_statistics_collector_impl
 * @brief Implementation of the Discord statistics collector that gathers and updates information about Discord servers and users.
 *
 * This class interfaces with a Discord bot and a database to collect statistics such as the number of users and servers,
 * and periodically updates this data. It tracks guild creation, deletion, user additions, and removals, storing this information
 * in the database for historical tracking and reporting.
 */
class Discord_statistics_collector_impl : public Discord_statistics_collector {

  /**
   * @brief Holds a reference to the configuration module, which provides the configuration values for the bot and other modules.
   */
  Config_ptr _config;

  /**
   * @brief Holds a reference to the Discord bot module used to interact with the Discord API and handle events.
   */
  Discord_bot_ptr _bot;

  /**
   * @brief Holds a reference to the database module, used to store and retrieve statistics from the database.
   */
  Database_ptr _db;

  /**
   * @brief Stores the event handler ID for the `on_guild_create` event, which triggers when a new guild (server) is created.
   */
  unsigned long _on_guild_create_handler;

  /**
   * @brief Stores the event handler ID for the `on_guild_delete` event, which triggers when a guild (server) is deleted or the bot leaves.
   */
  unsigned long _on_guild_delete_handler;

  /**
   * @brief Stores the event handler ID for the `on_guild_member_add` event, which triggers when a new user joins a guild.
   */
  unsigned long _on_guild_member_add_handler;

  /**
   * @brief Stores the event handler ID for the `on_guild_member_remove` event, which triggers when a user leaves or is removed from a guild.
   */
  unsigned long _on_guild_member_remove_handler;

  /**
   * @brief Prepared SQL statement for adding a new guild (server) with its user count to the `guilds_users_data` table.
   */
  Prepared_statement _add_guild_stmt;

  /**
   * @brief Prepared SQL statement for removing a guild (server) from the `guilds_users_data` table when it is deleted.
   */
  Prepared_statement _remove_guild_stmt;

  /**
   * @brief Prepared SQL statement for incrementing the user count in a guild when a new user joins.
   */
  Prepared_statement _add_user_stmt;

  /**
   * @brief Prepared SQL statement for decrementing the user count in a guild when a user leaves or is removed.
   */
  Prepared_statement _remove_user_stmt;

  /**
   * @brief Timer that triggers periodic reporting of historical data, storing server and user statistics in the database.
   */
  dpp::timer _report_timer;

  /**
   * @brief Prepared SQL statement for inserting historical statistics into the `guilds_users_count_history` table.
   */
  Prepared_statement _historic_report_stmt;

  /**
   * @brief Prepared SQL statement for retrieving the total number of users across all guilds.
   */
  Prepared_statement _get_users_cnt_stmt;

  /**
   * @brief Prepared SQL statement for retrieving the total number of guilds (servers) tracked by the bot.
   */
  Prepared_statement _get_servers_cnt;

  /**
   * @brief Prepared SQL statement for retrieving the number of users in a specific guild.
   */
  Prepared_statement _get_users_cnt_on_server_stmt;

public:
  /**
   * @brief Constructor for the Discord statistics collector implementation.
   */
  Discord_statistics_collector_impl();

 /**
  * @breif Define destructor.
  */
  virtual ~Discord_statistics_collector_impl() = default;

  /**
   * @brief Starts the statistics collection process, setting up timers and attaching event handlers.
   */
  void run() override;

  /**
   * @brief Stops the statistics collection process, detaching event handlers and stopping timers.
   */
  void stop() override;

  /**
   * @brief Initializes the module by setting up the database, bot, and configuration references.
   *
   * @param modules A map containing the initialized modules required by this class (config, discord bot, database).
   */
  void init(const Modules &modules) override;

  /**
   * @brief Asynchronously retrieves the total number of servers (guilds) tracked by the bot.
   *
   * @return Task<uint64_t> The number of servers.
   */
  Task<uint64_t> get_servers_cnt() override;

  /**
   * @brief Asynchronously retrieves the total number of users across all servers tracked by the bot.
   *
   * @return Task<uint64_t> The total user count.
   */
  Task<uint64_t> get_users_cnt() override;

  /**
   * @brief Asynchronously retrieves the number of users on a specific server.
   *
   * @param guild_id The ID of the Discord guild (server).
   * @return Task<uint64_t> The number of users in the specified server.
   */
  Task<uint64_t> get_users_on_server_cnt(const dpp::snowflake &guild_id) override;
};

/**
 * @brief Factory function for creating an instance of the Discord statistics collector module.
 *
 * @return Module_ptr A shared pointer to the created module.
 */
extern "C" Module_ptr create();

} // gb
