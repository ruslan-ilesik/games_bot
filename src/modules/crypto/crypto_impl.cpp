//
// Created by ilesik on 7/20/24.
//

#include "crypto_impl.hpp"

namespace gb {
    void Crypto_impl::run() {}

    void Crypto_impl::stop() {}

    void Crypto_impl::innit(const Modules &modules) {}

    Crypto_impl::Crypto_impl() : Crypto("crypto",{}){
        if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
            throw std::runtime_error("Crypto_impl Error generating random bytes for key and IV.");
        }
    }

    std::string Crypto_impl::decrypt(const std::string &ciphertext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
        }

        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv)) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize decryption.");
        }

        std::vector<unsigned char> plaintext(ciphertext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
        int len;
        int plaintext_len;

        if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size())) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to decrypt.");
        }
        plaintext_len = len;

        if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize decryption.");
        }
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        std::string output(reinterpret_cast<const char*>(plaintext.data()), plaintext_len);
        return output;
    }

    std::string Crypto_impl::
    encrypt(const std::string &m) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX.");
        }

        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv)) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize encryption.");
        }

        std::vector<unsigned char> ciphertext(m.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
        int len;
        int ciphertext_len;

        if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(m.data()), m.size())) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to encrypt.");
        }
        ciphertext_len = len;

        if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize encryption.");
        }
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        std::string output(reinterpret_cast<const char*>(ciphertext.data()), ciphertext_len);
        return output;
    }


    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Crypto_impl>());
    }
} // gb