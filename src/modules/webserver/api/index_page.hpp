//
// Created by ilesik on 11/3/24.
//

#pragma once
#include "../webserver_impl.hpp"

namespace gb {

  /**
   * @brief Registers API endpoints for handling index page related requests.
   *
   * This function registers the following API endpoints:
   * - `/api/get-index-page-counters`: Retrieves counts for users, servers, images, and games.
   * - `/api/get-reviews`: Retrieves reviews from the database based on the provided `start_id`.
   *
   * @param server A pointer to the `Webserver_impl` instance to register the handlers.
   */
  void index_page_api(Webserver_impl* server);

} // gb
