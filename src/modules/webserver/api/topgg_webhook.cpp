//
// Created by ilesik on 3/23/26.
//

#include "topgg_webhook.hpp"
#include <openssl/hmac.h>
#include <openssl/crypto.h>

void gb::topgg_webhook(Webserver_impl *server) {
    Prepared_statement vote_webhook_stmt = server->db->create_prepared_statement("CALL topgg_webhook(?);");

    server->on_stop.emplace_back([=]() {
        server->db->remove_prepared_statement(vote_webhook_stmt);
    });

     drogon::app().registerHandler(
        "/api/topgg-webhook",
        [=](drogon::HttpRequestPtr req,
            std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {

            auto make_response = [&](drogon::HttpStatusCode code) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(code);
                callback(resp);
            };

            std::string secret = server->config->get_value("topgg_webhook");
            std::string raw_body{req->getBody()};

            server->log->info(std::format("topgg webhook data: {}", raw_body));

            // ---- Header check ----
            auto header_it = req->getHeaders().find("x-topgg-signature");
            if (header_it == req->getHeaders().end()) {
                server->log->warn("Missing x-topgg-signature header");
                make_response(drogon::k400BadRequest);
                co_return;
            }

            std::string signature_header = header_it->second;

            // ---- Parse header ----
            size_t t_pos = signature_header.find("t=");
            size_t v1_pos = signature_header.find("v1=");

            if (t_pos == std::string::npos || v1_pos == std::string::npos) {
                server->log->warn("Malformed x-topgg-signature header");
                make_response(drogon::k400BadRequest);
                co_return;
            }

            std::string timestamp =
                signature_header.substr(t_pos + 2, v1_pos - t_pos - 3);
            std::string signature =
                signature_header.substr(v1_pos + 3);

            // ---- Compute HMAC (thread-safe) ----
            std::string message = timestamp + "." + raw_body;

            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int digest_len = 0;

            HMAC(EVP_sha256(),
                 secret.data(), secret.size(),
                 reinterpret_cast<const unsigned char*>(message.data()), message.size(),
                 digest, &digest_len);

            std::ostringstream oss;
            oss << std::hex << std::setfill('0');
            for (unsigned int i = 0; i < digest_len; ++i) {
                oss << std::setw(2) << static_cast<int>(digest[i]);
            }

            std::string expected_signature = oss.str();

            // ---- Constant-time compare ----
            if (expected_signature.size() != signature.size() ||
                CRYPTO_memcmp(expected_signature.data(), signature.data(), expected_signature.size()) != 0) {
                server->log->warn("Top.gg webhook signature mismatch");
                make_response(drogon::k401Unauthorized);
                co_return;
            }

            // ---- Parse JSON ----
            nlohmann::json data;
            try {
                data = nlohmann::json::parse(raw_body);
            } catch (const std::exception &e) {
                server->log->warn(std::format("Failed to parse JSON: {}", e.what()));
                make_response(drogon::k400BadRequest);
                co_return;
            }

            server->log->info(std::format("Valid Top.gg webhook received: {}", data.dump()));

            // ---- Validate type ----
            if (!data.contains("type")) {
                make_response(drogon::k400BadRequest);
                co_return;
            }

            std::string type = data["type"].get<std::string>();

            if (type != "vote.create" && type != "webhook.test") {
                server->log->warn("Unknown type: " + type);
                make_response(drogon::k200OK);
                co_return;
            }

            // ---- Extract user id safely ----
            try {
                auto user_id_str = data.at("data").at("user").at("platform_id").get<std::string>();
                dpp::snowflake user_id = user_id_str;

                co_await server->db->execute_prepared_statement(vote_webhook_stmt, user_id);
            } catch (const std::exception &e) {
                server->log->warn(std::format("Invalid payload structure: {}", e.what()));
                make_response(drogon::k400BadRequest);
                co_return;
            }

            make_response(drogon::k200OK);
            co_return;
        });
}
