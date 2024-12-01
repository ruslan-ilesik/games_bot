//
// Created by ilesik on 11/20/24.
//

#pragma once
#include <src/modules/webserver/webserver_impl.hpp>

namespace gb {

    /**
     * @brief Registers a redirect handler for a specific path.
     *
     * This function registers a handler for the specified HTTP path. When a request
     * is made to this path, it retrieves a configuration value from the server using
     * the provided configuration key and redirects the client to the corresponding URL.
     *
     * @param server Pointer to the Webserver_impl instance managing the configuration and logging.
     * @param path The HTTP path for which the redirect handler will be registered.
     * @param config_key The key used to retrieve the redirect URL from the server's configuration.
     */
    void register_config_redirect_handler(Webserver_impl *server, const std::string &path,
                                          const std::string &config_key);

} // namespace gb
