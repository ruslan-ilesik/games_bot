//
// Created by ilesik on 11/10/24.
//

#include "cookie_manager.hpp"

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>

namespace gb {
    drogon::Task<dpp::user> fetch_user_data(const Discord_user_credentials& credentials) {
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
            throw std::runtime_error("Failed to fetch user data by given credentials "+ body);
        };
        dpp::user u;
        auto parsed = nlohmann::json::parse(body);
        u.fill_from_json(&parsed);
        co_return u;
    }
}

