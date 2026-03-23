//
// Created by ilesik on 3/23/26.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb {
    /**
     * @brief Registers the topgg webhook related API endpoints with the server.
     *
     * This function registers the `/api/topgg-webhook` endpoint for handling webhooks.
     *
     * @param server Pointer to the Webserver_impl instance.
     */
    void topgg_webhook(Webserver_impl *server);
}
