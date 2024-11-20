//
// Created by ilesik on 11/9/24.
//

#include "discord_login.hpp"

#include <src/modules/webserver/utils/cookie_manager.hpp>
#include <src/modules/webserver/utils/other.hpp>
void gb::discord_login_api(Webserver_impl *server) {

    Prepared_statement insert_cookie = server->db->create_prepared_statement(
        "insert into website_cookies (id,expire_time,renew_time) values (?,from_unixtime(?),from_unixtime(?));");
    server->on_stop.push_back([=]() { server->db->remove_prepared_statement(insert_cookie); });

    register_config_redirect_handler(server,"/api/login-with-discord-redirect","login_with_discord_url");

    drogon::app().registerHandler("/api/discord/get-display-user-data",
                                  [=](drogon::HttpRequestPtr req,
                                      std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                                      std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req, true);
                                      std::string json_string;
                                      if (validation.first == true) {
                                          json_string = validation.second.discord_user.to_json().dump();
                                      }
                                      Json::Reader reader;
                                      Json::Value value;
                                      reader.parse( json_string, value );
                                      auto resp = drogon::HttpResponse::newHttpJsonResponse(value);
                                      if (json_string.empty()) {
                                          resp->setStatusCode(drogon::k403Forbidden);
                                      }
                                      set_cookie(server, resp, validation);
                                      callback(resp);
                                  });

    drogon::app().registerHandler(
        "/auth/discord",
        [=](drogon::HttpRequestPtr req,
            std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
            auto para = req->getParameters();
            std::string code = para.at("code");
            if (code.empty()) {
                auto error_resp = drogon::HttpResponse::newHttpResponse();
                server->log->error("Login with Discord failed, no code given");
                error_resp->setStatusCode(drogon::k500InternalServerError);
                error_resp->setBody("LOGIN with Discord failed");
                callback(error_resp);
                co_return;
            }

            std::string website_base = server->config->get_value("website_base_url");
            std::string post_data =
                "grant_type=authorization_code&code=" + code + "&redirect_uri=" + (website_base + "auth/discord");

            std::string auth_string = server->config->get_value("discord_login_client_id") + ":" +
                                      server->config->get_value("discord_login_client_secret");
            std::string auth_header = "Basic " + drogon::utils::base64Encode(auth_string);

            drogon::HttpClientPtr client = drogon::HttpClient::newHttpClient("https://discord.com");
            drogon::HttpRequestPtr discord_request = drogon::HttpRequest::newHttpRequest();
            discord_request->setPath("/api/oauth2/token");
            discord_request->setMethod(drogon::Post);
            discord_request->setBody(post_data);
            discord_request->setContentTypeString("application/x-www-form-urlencoded");
            discord_request->addHeader("Authorization", auth_header);

            drogon::HttpResponsePtr response = co_await client->sendRequestCoro(discord_request);

            auto tmp = response->getBody();
            std::string body(tmp.begin(), tmp.end());
            if (response->statusCode() != drogon::k200OK || nlohmann::json::parse(body).contains("error")) {
                auto error_resp = drogon::HttpResponse::newHttpResponse();
                server->log->error("Login with Discord failed: " + std::to_string(response->statusCode()) +
                                   " body: " + body + " post data: " + post_data);

                error_resp->setStatusCode(drogon::k500InternalServerError);
                error_resp->setBody("LOGIN with Discord failed");
                callback(error_resp);
                co_return;
            }

            auto json_data = nlohmann::json::parse(body);
            auto resp = drogon::HttpResponse::newRedirectionResponse("/");
            auto data = nlohmann::json::parse(body);
            Discord_user_credentials credentials = Discord_user_credentials::from_json(data);

            dpp::user u = co_await fetch_discord_user_data(credentials);
            Authorization_cookie cookie = {u, credentials, req->getHeader("User-Agent"), server->current_jwt_id++};
            co_await server->db->execute_prepared_statement(insert_cookie, cookie.id, cookie.credentials.hard_expire,
                                                            cookie.credentials.soft_expire);

            set_cookie(server, cookie, resp);
            callback(resp);

            co_return;
        });
}
