//
// Created by ilesik on 11/15/24.
//

#include "patreon.hpp"

namespace gb {
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
        drogon::app().registerHandler(
            "/action/patreon/webhook",
            [=](drogon::HttpRequestPtr req,
                std::function<void(const drogon::HttpResponsePtr &)> callback) -> drogon::Task<> {
                std::string signature = req->getHeader("X-Patreon-Signature");
                std::string calculated_signature;
                try {
                    calculated_signature = calculate_hmac_md5(std::string(req->getBody()),
                                                              server->config->get_value("patreon_webhook_secret"));
                }
                catch (std::runtime_error& e) {
                    server->log->error("Patreon webhook signature verification failed ("+std::string(e.what())+"): " + std::string(req->getBody()) +
                                       " " + req->getHeader("X-Patreon-Signature"));
                    co_return;
                }

                if (calculated_signature != signature) {
                    server->log->error("Patreon webhook signature verification failed: " + std::string(req->getBody()) +
                                       " " + req->getHeader("X-Patreon-Signature"));
                    co_return;
                }
                co_return;
            });
    }
} // namespace gb
