//
// Created by ilesik on 11/20/24.
//

#pragma once

#include <src/modules/webserver/webserver_impl.hpp>

namespace gb {

  /**
   * @brief Registers API endpoints for the dashboard page.
   *
   * This function sets up multiple API routes that provide data for various
   * components of the dashboard, including history tables, activity graphs,
   * user header information, and achievements. It also manages prepared
   * statements and handles proper cleanup during server shutdown.
   *
   * @param server Pointer to the Webserver implementation, providing access to
   * database connections, utilities, and other server resources.
   */
  void dashboard_page_api(Webserver_impl* server);

}
