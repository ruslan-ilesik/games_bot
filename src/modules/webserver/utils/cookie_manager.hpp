//
// Created by ilesik on 11/10/24.
//

#pragma once
#include <dpp/user.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <src/modules/webserver/webserver_impl.hpp>
#include <string>

namespace gb {

    struct Discord_user_credentials {
        std::string token_type;
        std::string access_token;
        uint64_t expires_in;
        std::string refresh_token;
        std::string scope;
        time_t soft_expire;
        time_t hard_expire;

        // default soft expire = 1/2 expires_in.
        Discord_user_credentials(const std::string &token_type, const std::string &access_token,
                                 const uint64_t expires_in, const std::string &refresh_token, const std::string &scope,
                                 time_t soft_expire = 0, time_t hard_expire = 0);

        Discord_user_credentials();

        nlohmann::json to_json() const;

        static inline Discord_user_credentials from_json(const nlohmann::json &data) {
            time_t soft_expire = 0;
            time_t hard_expire = 0;
            if (data.contains("soft_expire")) {
                soft_expire = data["soft_expire"].get<time_t>();
            }
            if (data.contains("hard_expire")) {
                hard_expire = data["hard_expire"].get<time_t>();
            }
            return {data["token_type"].get<std::string>(),
                    data["access_token"].get<std::string>(),
                    data["expires_in"].get<uint64_t>(),
                    data["refresh_token"].get<std::string>(),
                    data["scope"].get<std::string>(),
                    soft_expire,
                    hard_expire};
        };
    };

    struct Authorization_cookie {
        dpp::user discord_user;
        Discord_user_credentials credentials;
        std::string user_agent;
        uint64_t id;

        Authorization_cookie();
        Authorization_cookie(const dpp::user &discord_user, const Discord_user_credentials &credentials,
                             const std::string &user_agent, uint64_t id);


        std::string to_cookie_string(Webserver_impl *server) const;
    };


    std::string get_original_host(drogon::HttpRequestPtr &req);

    drogon::Task<dpp::user> fetch_discord_user_data(const Discord_user_credentials &credentials);

    drogon::Task<std::pair<bool, Authorization_cookie>> validate_authorization_cookie(Webserver_impl *server,
                                                                                      drogon::HttpRequestPtr &req, bool renew = false);

    void set_cookie(Webserver_impl *server, const Authorization_cookie &cookie, drogon::HttpResponsePtr& response,
                    bool not_clear = true);

    void set_cookie(Webserver_impl *server,drogon::HttpResponsePtr& response, const std::pair<bool, Authorization_cookie>& validation_result);

    drogon::Task<std::pair<bool, Discord_user_credentials>>
    renew_discord_user_credentials(Webserver_impl* server, const Discord_user_credentials &credentials);
} // namespace gb
