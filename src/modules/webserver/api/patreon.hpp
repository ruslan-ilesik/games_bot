//
// Created by ilesik on 11/15/24.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb{
  /**
   * @brief Initializes the routes and handlers for interacting with the Patreon API.
   *
   * Sets up various HTTP routes for login, logout, webhook handling, and user data synchronization
   * between Patreon and the user's Discord account. Also ensures proper database query preparation
   * and cleanup.
   *
   * @param server The Webserver_impl instance handling HTTP requests and interacting with the database.
   */
  void patreon_api(Webserver_impl *server);
}
