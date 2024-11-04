//
// Created by ilesik on 9/30/24.
//

#pragma once
#include <src/module/module.hpp>
#include <src/utils/coro/coro.hpp>
#include <dpp/dpp.h>

namespace gb {

/**
 * @class Discord_statistics_collector
 * @brief An abstract class that defines the interface for collecting Discord server and user statistics.
 *
 * This class serves as an API for gathering and providing information about the number of servers (guilds) and users
 * managed by a Discord bot. It also allows retrieval of user counts for specific servers.
 */
class Discord_statistics_collector : public Module {
public:
  /**
   * @brief Constructor for the Discord_statistics_collector class.
   *
   * @param name The name of the module.
   * @param dependencies A vector of dependency module names that this module depends on.
   */
  Discord_statistics_collector(const std::string& name, const std::vector<std::string>& dependencies) : Module(name, dependencies) {}

  /**
   * @brief Retrieves the total number of servers (guilds) managed by the bot.
   *
   * This method should be implemented by a derived class to provide the current count of Discord guilds tracked.
   *
   * @return Task<uint64_t> An asynchronous task that returns the number of guilds.
   */
  virtual Task<uint64_t> get_servers_cnt() = 0;

  /**
   * @brief Retrieves the total number of users across all servers (guilds).
   *
   * This method should be implemented by a derived class to provide the current total number of users tracked.
   *
   * @return Task<uint64_t> An asynchronous task that returns the total number of users.
   */
  virtual Task<uint64_t> get_users_cnt() = 0;

  /**
   * @brief Retrieves the number of users on a specific server (guild).
   *
   * This method should be implemented by a derived class to provide the user count for a specific Discord guild.
   *
   * @param guild_id The unique identifier of the Discord guild (server).
   * @return Task<uint64_t> An asynchronous task that returns the number of users on the specified guild.
   */
  virtual Task<uint64_t> get_users_on_server_cnt(const dpp::snowflake& guild_id) = 0;
};

/**
 * @typedef Discord_statistics_collector_ptr
 * @brief A shared pointer to the Discord_statistics_collector class.
 */
typedef std::shared_ptr<Discord_statistics_collector> Discord_statistics_collector_ptr;

} // namespace gb
