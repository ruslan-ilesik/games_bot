//
// Created by ilesik on 3/23/26.
//

#include "topgg_webhook.hpp"
#include <openssl/hmac.h>

void gb::topgg_webhook(Webserver_impl *server) {
    drogon::app().registerHandler(
        "/api/topgg-webhook",
        [=](drogon::HttpRequestPtr req,
            std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
            std::string secret = server->config->get_value("topgg_webhook");
            std::string raw_body{req->getBody()};


            server->log->info(std::format("topgg webhook data: {}", raw_body));

            // Extract signature header
            auto header_it = req->getHeaders().find("x-topgg-signature");
            if (header_it == req->getHeaders().end()) {
                server->log->warn("Missing x-topgg-signature header");
                co_return;
            }

            std::string signature_header = header_it->second;
            std::string timestamp, signature;

            // Parse header: t={timestamp},v1={signature}
            size_t t_pos = signature_header.find("t=");
            size_t v1_pos = signature_header.find("v1=");
            if (t_pos == std::string::npos || v1_pos == std::string::npos) {
                server->log->warn("Malformed x-topgg-signature header");
                co_return;
            }
            timestamp = signature_header.substr(t_pos + 2, v1_pos - t_pos - 3); // exclude comma
            signature = signature_header.substr(v1_pos + 3);

            // Compute HMAC SHA-256 of "{timestamp}.{rawBody}"
            std::string message = timestamp + "." + raw_body;
            unsigned char *digest;
            digest = HMAC(EVP_sha256(), secret.c_str(), secret.size(),
                          reinterpret_cast<const unsigned char *>(message.c_str()), message.size(), nullptr, nullptr);

            // Convert digest to lowercase hex string
            std::ostringstream oss;
            for (int i = 0; i < 32; ++i) {
                oss << std::hex << std::setw(2) << std::setfill('0') << (int) digest[i];
            }
            std::string expected_signature = oss.str();


            if (expected_signature != signature) {
                server->log->warn("Top.gg webhook signature mismatch");
                co_return;
            }

            nlohmann::json data;
            try {
                data = nlohmann::json::parse(raw_body);
            } catch (const std::exception &e) {
                server->log->warn(std::format("Failed to parse JSON: {}", e.what()));
                co_return;
            }

            server->log->info(std::format("Valid Top.gg webhook received: {}", data.dump()));

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k200OK);
            callback(resp);
            co_return;
        });
}
