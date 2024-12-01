//
// Created by ilesik on 11/17/24.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb {

  /**
   * @brief Registers the premium-related API endpoints with the server.
   *
   * This function registers the `/api/buy-premium-redirect` endpoint for redirection
   * to the premium purchase URL and the `/api/get-user-premium-status` endpoint
   * for fetching the premium status of a user.
   *
   * @param server Pointer to the Webserver_impl instance.
   */
  void premium_api(Webserver_impl *server);

} // namespace gb

