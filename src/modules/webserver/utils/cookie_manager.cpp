//
// Created by ilesik on 11/10/24.
//

#include "cookie_manager.hpp"

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>

namespace gb {
    Discord_user_credentials::Discord_user_credentials(const std::string &token_type, const std::string &access_token,
                                                       const uint64_t expires_in, const std::string &refresh_token,
                                                       const std::string &scope, time_t soft_expire,
                                                       time_t hard_expire) {
        this->token_type = token_type;
        this->access_token = access_token;
        this->expires_in = expires_in;
        this->refresh_token = refresh_token;
        this->scope = scope;
        if (soft_expire == 0) {
            soft_expire =
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count() +
                expires_in / 2;
        }
        if (hard_expire == 0) {
            hard_expire =
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count() +
                expires_in - 10;
        }
        this->soft_expire = soft_expire;
        this->hard_expire = hard_expire;
    }
    Discord_user_credentials::Discord_user_credentials(): expires_in(0), soft_expire(0), hard_expire(0) {}

    nlohmann::json Discord_user_credentials::to_json() {
        nlohmann::json data;
        data.emplace("token_type", token_type);
        data.emplace("access_token", access_token);
        data.emplace("expires_in", expires_in);
        data.emplace("refresh_token", refresh_token);
        data.emplace("scope", scope);
        data.emplace("soft_expire", soft_expire);
        data.emplace("hard_expire", hard_expire);
        return data;
    }


    Authorization_cookie::Authorization_cookie() : credentials("", "", 0, "", ""), id(0) {}
    Authorization_cookie::Authorization_cookie(const dpp::user &discord_user,
                                               const Discord_user_credentials &credentials,
                                               const std::string &user_agent, uint64_t id) : credentials(credentials) {
        this->user_agent = user_agent;
        this->discord_user = discord_user;
        this->id = id;
    }

    std::string Authorization_cookie::to_cookie_string(Webserver_impl *server) {
        jwt::jwt_object obj{jwt::params::algorithm("HS256"),
                            jwt::params::payload({{"id", std::to_string(id)},
                                                  {"discord_user", discord_user.to_json().dump()},
                                                  {"user_agent", user_agent},
                                                  {"credentials", credentials.to_json().dump()}}),
                            jwt::params::secret(server->config->get_value("jwt_secret"))};
        return obj.signature();
    }

    std::string get_original_host(drogon::HttpRequestPtr &req) {
        std::string host = req->getHeader("X-Forwarded-Host");
        if (host.empty()) {
            host = req->getHeader("Host");
        }
        return host;
    }

    drogon::Task<dpp::user> fetch_discord_user_data(const Discord_user_credentials &credentials) {
        std::string auth_header = credentials.token_type + " " + credentials.access_token;

        drogon::HttpClientPtr client = drogon::HttpClient::newHttpClient("https://discord.com");
        drogon::HttpRequestPtr discord_request = drogon::HttpRequest::newHttpRequest();
        discord_request->setPath("/api/users/@me");
        discord_request->setMethod(drogon::Get);
        discord_request->setContentTypeString("text/json");
        discord_request->addHeader("Authorization", auth_header);
        drogon::HttpResponsePtr response = co_await client->sendRequestCoro(discord_request);

        auto tmp = response->getBody();
        std::string body(tmp.begin(), tmp.end());
        if (response->statusCode() != drogon::k200OK || nlohmann::json::parse(body).contains("error")) {
            throw std::runtime_error("Failed to fetch user data by given credentials " + body);
        };
        dpp::user u;
        auto parsed = nlohmann::json::parse(body);
        u.fill_from_json(&parsed);
        co_return u;
    }

    drogon::Task<std::pair<bool, Authorization_cookie>> validate_authorization_cookie(Webserver_impl *server,
                                                                                      drogon::HttpRequestPtr &req) {
        std::string cookie_string = req->getCookie("Authorization");
        if (cookie_string.empty()) {
            co_return std::pair<bool, Authorization_cookie>{false, {}};
        }
        try {

            auto data = jwt::decode(cookie_string, jwt::params::algorithms({"HS256"}),
                                    jwt::params::secret(server->config->get_value("jwt_secret")))
                            .payload();
            uint64_t id;
            std::istringstream iss(data.get_claim_value<std::string>("id"));
            iss >> id;

            // check if such cookie record exists.
            Task<bool> existance_checker = server->check_if_cookie_exists(id);

            dpp::user user;
            auto u_json = nlohmann::json::parse(data.get_claim_value<std::string>("discord_user"));
            user.fill_from_json(&u_json);
            std::string user_agent = data.get_claim_value<std::string>("user_agent");
            Discord_user_credentials credentials = Discord_user_credentials::from_json(
                nlohmann::json::parse(data.get_claim_value<std::string>("credentials")));

            bool exist = co_await existance_checker;
            if (!exist) {
                server->log->error("Cookie does not exist! " +  cookie_string);
                co_return std::pair<bool, Authorization_cookie>{false, {}};
            }

            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count() > credentials.hard_expire) {
                co_return std::pair<bool, Authorization_cookie>{false, {}};
            }

            if (req->getHeader("User-Agent") != user_agent) {
                // enforce cookie delete on wrong user agent as someone can try to bruteforce user agent via replay
                // attacks.
                server->log->error("Cookie user agent does not match! " +  cookie_string + " " + user_agent);
                co_await server->delete_cookie(id);
                co_return std::pair<bool, Authorization_cookie>{false, {}};
            }

            // renew only if cookie actually exists in db
            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count() < credentials.soft_expire) {
                co_await server->delete_cookie(id);

                auto task1 = renew_discord_user_credentials(server,credentials);
                std::pair<bool,Discord_user_credentials> result =  co_await task1;
                if (!result.first) {
                    co_return std::pair<bool, Authorization_cookie>{false, {}};
                }
                credentials = result.second;
                // renew credentials and make new cookie
            }
            Authorization_cookie cookie = {user, credentials, user_agent, id};

            co_return std::pair<bool, Authorization_cookie>{true, cookie};

        } catch (const std::exception &e) {
            server->log->error("Error decoding JWT: " + std::string(e.what()) + " Cookie: " + cookie_string);
            co_return std::pair<bool, Authorization_cookie>{false, {}};
        }
    }

    drogon::Task<std::pair<bool, Discord_user_credentials>>
    renew_discord_user_credentials(Webserver_impl* server, const Discord_user_credentials &credentials) {
        std::string post_data = "grant_type=refresh_token&refresh_token=" + credentials.refresh_token;
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
            server->log->error("Failed to renew user token by given credentials " + body);
            co_return std::pair<bool, Discord_user_credentials>{false, {}};
        };
        auto data = nlohmann::json::parse(body);
        Discord_user_credentials returns_credentials = Discord_user_credentials::from_json(data);
        co_return std::pair<bool, Discord_user_credentials>{true, {returns_credentials}};

    }
} // namespace gb
