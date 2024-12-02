//
// Created by ilesik on 11/15/24.
//

#include "patreon.hpp"
#include <trantor/net/EventLoop.h>
#include <src/modules/webserver/utils/cookie_manager.hpp>
#include <src/modules/webserver/utils/type_conversions.hpp>

namespace gb {
    enum class SUBSCRIPTION_STATUS { NO_SUBSCRIPTION, BASIC };

    std::string to_string(SUBSCRIPTION_STATUS status) {
        switch (status) {
            case SUBSCRIPTION_STATUS::NO_SUBSCRIPTION:
                return "NO_SUBSCRIPTION";
            case SUBSCRIPTION_STATUS::BASIC:
                return "BASIC";
            default:
                throw std::runtime_error("Unknown conversion of SUBSCRIPTION_STATUS to string");
        }
    }

    std::string calculate_hmac_md5(const std::string &data, const std::string &key) {
        // Create a context for the HMAC operation
        EVP_MAC *mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
        EVP_MAC_CTX *ctx = EVP_MAC_CTX_new(mac);

        if (!ctx) {
            EVP_MAC_free(mac);
            throw std::runtime_error("Failed to create HMAC context");
        }

        // Set up the HMAC operation with MD5
        OSSL_PARAM params[] = {
            OSSL_PARAM_construct_utf8_string("digest", const_cast<char *>("MD5"), 0),
            OSSL_PARAM_construct_end(),
        };

        if (!EVP_MAC_init(ctx, reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), params)) {
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            throw std::runtime_error("Failed to initialize HMAC");
        }

        // Provide the data to compute the HMAC
        if (!EVP_MAC_update(ctx, reinterpret_cast<const unsigned char *>(data.c_str()), data.length())) {
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            throw std::runtime_error("Failed to update HMAC with data");
        }

        // Finalize the HMAC calculation
        unsigned char digest[EVP_MAX_MD_SIZE];
        size_t digest_len = 0;
        if (!EVP_MAC_final(ctx, digest, &digest_len, sizeof(digest))) {
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
            throw std::runtime_error("Failed to finalize HMAC");
        }

        // Clean up
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);

        // Convert the digest to a hexadecimal string
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < digest_len; ++i) {
            ss << std::setw(2) << static_cast<unsigned int>(digest[i]);
        }

        return ss.str();
    }

    void patreon_api(Webserver_impl *server) {
        Prepared_statement patreon_webhook_stmt =
            server->db->create_prepared_statement("CALL patreon_webhook(?,?,?,?);");

        Prepared_statement patreon_logout_stmt =
            server->db->create_prepared_statement("update premium join patreon_discord_link on\n"
                                                  "patreon_discord_link.subscription_id = premium.id\n"
                                                  "set discord_id = 0, premium.user_id = 0\n"
                                                  "where discord_id = ?;");

        Prepared_statement patreon_website_login_stmt =
            server->db->create_prepared_statement("CALL patreon_website_login(?,?,?,?);");

        Prepared_statement patreon_discord_status_stmt = server->db->create_prepared_statement(
            "select is_from_patreon, user_name from patreon_discord_link where discord_id = ?;");

        server->on_stop.push_back([=]() {
            server->db->remove_prepared_statement(patreon_webhook_stmt);
            server->db->remove_prepared_statement(patreon_discord_status_stmt);
            server->db->remove_prepared_statement(patreon_website_login_stmt);
            server->db->remove_prepared_statement(patreon_logout_stmt);
        });

        drogon::app().registerHandler(
            "/action/patreon/logout",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                if (!validation.first) {
                    server->log->error("Error in log out with patreon: User is not logged in with discord");
                    callback(drogon::HttpResponse::newRedirectionResponse("/"));
                    co_return;
                }
                co_await server->db->execute_prepared_statement(patreon_logout_stmt, validation.second.discord_user.id);
                auto dashboard_redirect = drogon::HttpResponse::newRedirectionResponse("/dashboard");
                callback(dashboard_redirect);
                co_return;
            });


        drogon::app().registerHandler(
            "/auth/patreon",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                if (!validation.first) {
                    server->log->error("Error in login with patreon: User is not logged in with discord");
                    callback(drogon::HttpResponse::newRedirectionResponse("/"));
                    co_return;
                }
                auto para = req->getParameters();
                std::string code = para.at("code");
                if (code.empty()) {
                    server->log->error("Error in login with patreon: Empty code");
                    callback(drogon::HttpResponse::newRedirectionResponse("/"));
                    co_return;
                }

                std::string client_id = server->config->get_value("patreon_client_id");
                std::string redirect_url = server->config->get_value("patreon_redirect_url");
                std::string post_data =
                    std::format("code={}&grant_type=authorization_code&client_id={}&client_secret={}&redirect_uri={}",
                                code, client_id, server->config->get_value("patreon_client_secret"),
                                drogon::utils::urlEncode(redirect_url));

                drogon::HttpResponsePtr response;
                drogon::HttpRequestPtr patreon_request;
                drogon::HttpClientPtr client;
                auto loop = trantor::EventLoop();
                try {
                    client = drogon::HttpClient::newHttpClient("https://www.patreon.com");
                    patreon_request = drogon::HttpRequest::newHttpRequest();
                    patreon_request->setPath("/api/oauth2/token");
                    patreon_request->setMethod(drogon::Post);
                    patreon_request->setBody(post_data);
                    patreon_request->setContentTypeString("application/x-www-form-urlencoded");
                    // patreon_request->addHeader("Authorization", auth_header);

                    response = co_await client->sendRequestCoro(patreon_request);
                } catch (const std::exception &e) {
                    server->log->error("Error in sending data to patreon: post: " + post_data +
                                       " error: " + std::string(e.what()));
                    auto error_resp = drogon::HttpResponse::newHttpResponse();

                    error_resp->setStatusCode(drogon::k500InternalServerError);
                    error_resp->setBody("LOGIN with Patreon failed");
                    callback(error_resp);
                    co_return;
                }


                auto tmp = response->getBody();
                std::string body(tmp.begin(), tmp.end());
                if (response->statusCode() != drogon::k200OK || nlohmann::json::parse(body).contains("error")) {
                    auto error_resp = drogon::HttpResponse::newHttpResponse();
                    server->log->error("Login with Patreon failed: " + std::to_string(response->statusCode()) +
                                       " body: " + body + " post data: " + post_data);

                    error_resp->setStatusCode(drogon::k500InternalServerError);
                    error_resp->setBody("LOGIN with Patreon failed");
                    callback(error_resp);
                    co_return;
                }
                nlohmann::json json_body = nlohmann::json::parse(body);
                auto token_type = json_body["token_type"].template get<std::string>();
                auto token = json_body["access_token"].template get<std::string>();
                std::string auth_header = token_type + " " + token;

                patreon_request = drogon::HttpRequest::newHttpRequest();
                patreon_request->setPath("/api/oauth2/v2/identity?fields[user]=full_name,social_connections");
                patreon_request->setMethod(drogon::Get);
                patreon_request->setBody(post_data);
                patreon_request->setContentTypeString("text/plain");
                patreon_request->addHeader("Authorization", auth_header);

                response = co_await client->sendRequestCoro(patreon_request);
                tmp = response->getBody();
                body = {tmp.begin(), tmp.end()};
                if (response->statusCode() != drogon::k200OK || nlohmann::json::parse(body).contains("error")) {
                    auto error_resp = drogon::HttpResponse::newHttpResponse();
                    server->log->error(
                        "Login with Patreon failed (on getting user data): " + std::to_string(response->statusCode()) +
                        " body: " + body + " post data: " + post_data);

                    error_resp->setStatusCode(drogon::k500InternalServerError);
                    error_resp->setBody("LOGIN with Patreon failed");
                    callback(error_resp);
                    co_return;
                }
                json_body = nlohmann::json::parse(body);
                dpp::snowflake discord_id = validation.second.discord_user.id;
                auto patreon_id = json_body["data"]["id"].template get<std::string>();
                auto nickname = json_body["data"]["attributes"]["full_name"].template get<std::string>();
                bool is_from_patreon = false;
                if (json_body["data"]["attributes"]["social_connections"].contains("discord") &&
                    !json_body["data"]["attributes"]["social_connections"]["discord"].is_null()) {
                    discord_id = {json_body["data"]["attributes"]["social_connections"]["discord"]["user_id"]
                                      .template get<std::string>()};
                    is_from_patreon = true;
                }
                co_await server->db->execute_prepared_statement(patreon_website_login_stmt, patreon_id, discord_id,
                                                                is_from_patreon, nickname);
                auto dashboard_redirect = drogon::HttpResponse::newRedirectionResponse("/dashboard");
                callback(dashboard_redirect);
                co_return;
            });

        drogon::app().registerHandler(
            "/api/login-with-patreon-redirect",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::string url_base = server->config->get_value("patreon_login_url_base");
                std::string client_id = server->config->get_value("patreon_client_id");
                std::string redirect_url = server->config->get_value("patreon_redirect_url");
                redirect_url = std::vformat(url_base, std::make_format_args(client_id, redirect_url));
                auto response = drogon::HttpResponse::newRedirectionResponse(redirect_url);
                callback(response);
                co_return;
            });


        drogon::app().registerHandler(
            "/action/patreon/webhook",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::string signature = req->getHeader("X-Patreon-Signature");
                std::string calculated_signature;
                try {
                    calculated_signature = calculate_hmac_md5(std::string(req->getBody()),
                                                              server->config->get_value("patreon_webhook_secret"));
                } catch (std::runtime_error &e) {
                    server->log->error("Patreon webhook signature verification failed (" + std::string(e.what()) +
                                       "): " + std::string(req->getBody()) + " " +
                                       req->getHeader("X-Patreon-Signature"));
                    co_return;
                }

                if (calculated_signature != signature) {
                    server->log->error("Patreon webhook signature verification failed: " + std::string(req->getBody()) +
                                       " " + req->getHeader("X-Patreon-Signature"));
                    co_return;
                }
                nlohmann::json data = nlohmann::json::parse(req->getBody());
                SUBSCRIPTION_STATUS patron_status;

                if (data["data"]["attributes"]["patron_status"].is_null()) {
                    patron_status = SUBSCRIPTION_STATUS::NO_SUBSCRIPTION;
                } else {
                    patron_status = data["data"]["attributes"]["patron_status"] == "active_patron"
                                        ? SUBSCRIPTION_STATUS::BASIC
                                        : SUBSCRIPTION_STATUS::NO_SUBSCRIPTION;
                }
                std::string patreon_id =
                    data["data"]["relationships"]["user"]["data"]["id"].template get<std::string>();
                std::string nickname = data["data"]["attributes"]["full_name"].template get<std::string>();

                std::string discord_id = "0";
                for (auto &i: data["included"]) {
                    if (i["type"].template get<std::string>() == "user" &&
                        i["attributes"]["social_connections"].contains("discord")) {
                        discord_id =
                            i["attributes"]["social_connections"]["discord"]["user_id"].template get<std::string>();
                        break;
                    }
                }

                co_await server->db->execute_prepared_statement(patreon_webhook_stmt, patreon_id, discord_id,
                                                                to_string(patron_status), nickname);


                drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
                callback(resp);
                co_return;
            });

        drogon::app().registerHandler(
            "/api/patreon-login-status",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::pair<bool, Authorization_cookie> validation = co_await validate_authorization_cookie(server, req);
                Json::Value result;
                if (!validation.first) {
                    callback(drogon::HttpResponse::newHttpJsonResponse(result));
                    co_return;
                }

                dpp::user &user = validation.second.discord_user;
                Database_return_t r =
                    co_await server->db->execute_prepared_statement(patreon_discord_status_stmt, user.id);
                callback(drogon::HttpResponse::newHttpJsonResponse(to_json(r)));
                co_return;
            });
    }
} // namespace gb
