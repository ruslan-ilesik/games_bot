//
// Created by ilesik on 11/17/24.
//

#include "premium.hpp"

#include <src/modules/webserver/utils/cookie_manager.hpp>

namespace gb {


    void premium_api(Webserver_impl *server) {

        drogon::app().registerHandler(
            "/api/get-user-premium-status",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                if (!validation.first) {
                    Json::Value v;
                    v["status"] = to_string(PREMIUM_STATUS::NO_SUBSCRIPTION);
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(v);
                    set_cookie(server, resp, validation);
                    callback(resp);
                    co_return;
                }

                PREMIUM_STATUS p_s =
                    co_await server->premium_manager->get_users_premium_status(validation.second.discord_user.id);
                Json::Value v;
                v["status"] = to_string(p_s);
                auto resp = drogon::HttpResponse::newHttpJsonResponse(v);
                set_cookie(server, resp, validation);
                callback(resp);
                co_return;
            });
    }
} // namespace gb
