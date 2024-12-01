//
// Created by ilesik on 11/9/24.
//


#pragma once
#include "../webserver_impl.hpp"

namespace gb {

  /**
   * @brief Registers routes and handlers for Discord login functionality.
   *
   * This function sets up the necessary HTTP request handlers for handling Discord login, logout, and user data
   * retrieval. It interacts with the Discord API for OAuth2 token validation and revocation and sets/cleans cookies
   * as necessary. The handlers include:
   * - `/api/discord/get-display-user-data`: Retrieves user data from Discord if the user is authenticated.
   * - `/action/discord/logout`: Logs out the user by revoking the Discord OAuth2 token and clearing the session.
   * - `/auth/discord`: Handles the callback after Discord authentication and retrieves an access token.
   *
   * @param server The web server object that handles the routes and interacts with the database and cookies.
   */
  void discord_login_api(Webserver_impl* server);
}
