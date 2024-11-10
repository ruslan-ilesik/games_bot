//
// Created by ilesik on 11/9/24.
//

#include "discord_login.hpp"

#include <src/modules/webserver/utils/cookie_manager.hpp>
void gb::discord_login_api(Webserver_impl *server) {

    drogon::app().registerHandler("/api/login-with-discord-redirect",
                                  [=](drogon::HttpRequestPtr req,
                                      std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                                      auto redirect_url = server->config->get_value("login_with_discord_url");
                                      auto resp = drogon::HttpResponse::newRedirectionResponse(redirect_url);
                                      callback(resp);
                                      co_return;
                                  });

    drogon::app().registerHandler(
        "/auth/discord",
        [=](drogon::HttpRequestPtr req,
            std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
            static auto custom_url_encode = [](const std::string &input) {
                std::string encoded = drogon::utils::urlEncode(input);
                size_t pos = 0;
                while ((pos = encoded.find("/", pos)) != std::string::npos) {
                    encoded.replace(pos, 1, "%2F");
                    pos += 3; // Move past the replaced substring
                }
                return encoded;
            };

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
            std::string post_data = "grant_type=authorization_code&code=" + code +
                                    "&redirect_uri=" + (website_base + "auth/discord");

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
                server->log->error("Login with Discord failed: " + std::to_string(response->statusCode()) + " body: " + body + " post data: " + post_data);

                error_resp->setStatusCode(drogon::k500InternalServerError);
                error_resp->setBody("LOGIN with Discord failed");
                callback(error_resp);
                co_return;
            }

            auto json_data = nlohmann::json::parse(body);
            auto resp = drogon::HttpResponse::newRedirectionResponse("/");
            auto data = nlohmann::json::parse(body);
            Discord_user_credentials credentials = {
                .token_type = data["token_type"].get<std::string>(),
                .access_token = data["access_token"].get<std::string>(),
                .expires_in =  data["expires_in"].get<uint64_t>(),
                .refresh_token =  data["refresh_token"].get<std::string>(),
                .scope =  data["scope"].get<std::string>()
            };
            dpp::user u = co_await fetch_user_data(credentials);
            std::cout << u.get_avatar_url() << std::endl;
            // jwt::jwt_object obj{jwt::params::algorithm("HS256"), jwt::params::payload({{"discord_id",}})
            // ,jwt::params::secret(server->config->get_value("jwt_secret"))};

            // resp->addHeader("Set-Cookie","")
            callback(resp);

            co_return;
        });
}
