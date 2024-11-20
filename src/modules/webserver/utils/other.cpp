//
// Created by ilesik on 11/20/24.
//

#include "other.hpp"

namespace gb {
    void register_config_redirect_handler(Webserver_impl *server, const std::string &path, const std::string &config_key) {
        drogon::app().registerHandler(
            path,
            [server, config_key](drogon::HttpRequestPtr req,
                                 std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                try {
                    auto redirect_url = server->config->get_value(config_key);
                    auto response = drogon::HttpResponse::newRedirectionResponse(redirect_url);
                    callback(response);
                } catch (const std::exception &e) {
                    server->log->error("Redirect error: " + std::string(e.what()));
                    auto error_response = drogon::HttpResponse::newHttpResponse();
                    error_response->setStatusCode(drogon::k500InternalServerError);
                    error_response->setBody("Error: Redirect error");
                    callback(error_response);
                }

                co_return;
            });
    }
}