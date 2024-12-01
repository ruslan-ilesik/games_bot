//
// Created by ilesik on 11/5/24.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb {

  /**
   * @brief Registers the API endpoint for fetching a list of commands.
   *
   * This function sets up the `/api/get-commands-list` API endpoint, which returns a JSON array of all available commands.
   *
   * @param server A pointer to the `Webserver_impl` instance used for server configuration and access to handlers.
   */
  void commands_page_api(Webserver_impl* server);

}
