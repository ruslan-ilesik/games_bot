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

    /**
     * @brief Represents a user's Discord OAuth2 credentials.
     */
    struct Discord_user_credentials {
        std::string token_type; ///< The type of the OAuth2 token (e.g., "Bearer").
        std::string access_token; ///< The access token for the user.
        uint64_t expires_in; ///< Token expiry time in seconds.
        std::string refresh_token; ///< Token used to refresh the access token.
        std::string scope; ///< OAuth2 scope of the access token.
        time_t soft_expire; ///< Soft expiration time (before the token should be renewed).
        time_t hard_expire; ///< Hard expiration time (when the token fully expires).

        /**
         * @brief Constructs a Discord_user_credentials object with the given parameters.
         * @param token_type The type of the OAuth2 token.
         * @param access_token The user's access token.
         * @param expires_in Token expiry duration in seconds.
         * @param refresh_token Refresh token for obtaining a new access token.
         * @param scope The scope of the access token.
         * @param soft_expire The soft expiration time (optional).
         * @param hard_expire The hard expiration time (optional).
         */
        Discord_user_credentials(const std::string &token_type, const std::string &access_token,
                                 const uint64_t expires_in, const std::string &refresh_token, const std::string &scope,
                                 time_t soft_expire = 0, time_t hard_expire = 0);

        /**
         * @brief Default constructor for Discord_user_credentials.
         */
        Discord_user_credentials();

        /**
         * @brief Converts the credentials to a JSON object.
         * @return A JSON representation of the credentials.
         */
        nlohmann::json to_json() const;

        /**
         * @brief Creates a Discord_user_credentials object from a JSON representation.
         * @param data A JSON object containing the credential fields.
         * @return A populated Discord_user_credentials object.
         */
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
    };;

/**
 * @brief Represents an authorization cookie containing user and credential data.
 */
struct Authorization_cookie {
    dpp::user discord_user; ///< Discord user information.
    Discord_user_credentials credentials; ///< Discord OAuth2 credentials.
    std::string user_agent; ///< The user agent string of the client.
    uint64_t id; ///< Unique identifier for the authorization.

    /**
     * @brief Default constructor for Authorization_cookie.
     */
    Authorization_cookie();

    /**
     * @brief Constructs an Authorization_cookie object with the given parameters.
     * @param discord_user The associated Discord user.
     * @param credentials The user's credentials.
     * @param user_agent The user agent string of the client.
     * @param id Unique identifier for the authorization.
     */
    Authorization_cookie(const dpp::user &discord_user, const Discord_user_credentials &credentials,
                         const std::string &user_agent, uint64_t id);

    /**
     * @brief Converts the authorization cookie to a string representation for storage.
     * @param server A pointer to the Webserver implementation.
     * @return A string representation of the cookie.
     */
    std::string to_cookie_string(Webserver_impl *server) const;
};

/**
 * @brief Retrieves the original host of an HTTP request.
 * @param req The HTTP request object.
 * @return The original host as a string.
 */
std::string get_original_host(drogon::HttpRequestPtr &req);

/**
 * @brief Fetches Discord user data using OAuth2 credentials.
 * @param credentials The user's Discord OAuth2 credentials.
 * @return A coroutine task yielding a Discord user object.
 */
drogon::Task<dpp::user> fetch_discord_user_data(const Discord_user_credentials &credentials);

/**
 * @brief Validates an authorization cookie from the HTTP request.
 * @param server A pointer to the Webserver implementation.
 * @param req The HTTP request object.
 * @param renew Whether to renew the cookie upon validation.
 * @return A coroutine task yielding a pair containing validation status and the Authorization_cookie.
 */
drogon::Task<std::pair<bool, Authorization_cookie>>
validate_authorization_cookie(Webserver_impl *server, drogon::HttpRequestPtr &req, bool renew = false);

/**
 * @brief Sets an authorization cookie in the HTTP response.
 * @param server A pointer to the Webserver implementation.
 * @param cookie The Authorization_cookie to set.
 * @param response The HTTP response object.
 * @param not_clear Whether to retain previous cookies (default: true).
 */
void set_cookie(Webserver_impl *server, const Authorization_cookie &cookie, drogon::HttpResponsePtr &response,
                bool not_clear = true);

/**
 * @brief Sets an authorization cookie in the HTTP response using a validation result.
 * @param server A pointer to the Webserver implementation.
 * @param response The HTTP response object.
 * @param validation_result The result of cookie validation.
 */
void set_cookie(Webserver_impl *server, drogon::HttpResponsePtr &response,
                const std::pair<bool, Authorization_cookie> &validation_result);

/**
 * @brief Renews a user's Discord OAuth2 credentials.
 * @param server A pointer to the Webserver implementation.
 * @param credentials The current credentials to renew.
 * @return A coroutine task yielding a pair containing renewal status and the updated credentials.
 */
drogon::Task<std::pair<bool, Discord_user_credentials>>
renew_discord_user_credentials(Webserver_impl *server, const Discord_user_credentials &credentials);

} // namespace gb
