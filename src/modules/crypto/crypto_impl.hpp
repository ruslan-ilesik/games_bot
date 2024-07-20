//
// Created by ilesik on 7/20/24.
//

#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>
#include "./crypto.hpp"

namespace gb {

    /**
     * @class Crypto_impl
     * @brief Implements the Crypto class with OpenSSL-based AES-256-CBC encryption and decryption.
     */
    class Crypto_impl : public Crypto {
        static const unsigned int KEY_SIZE = 32; ///< Size of the encryption key (256 bits)
        static const unsigned int IV_SIZE = 16;  ///< Size of the initialization vector (128 bits)
        unsigned char key[KEY_SIZE]{}; ///< Encryption key
        unsigned char iv[IV_SIZE]{};   ///< Initialization vector

    public:
        /**
         * @brief Constructor for Crypto_impl.
         *        Initializes the encryption key and IV with random bytes.
         * @throws std::runtime_error if key or IV generation fails.
         */
        Crypto_impl();

        /**
         * @brief Default destructor.
         */
        virtual ~Crypto_impl() = default;

        /**
         * @brief Initializes the module with the provided modules.
         * @param modules Reference to the modules.
         */
        void innit(const Modules &modules) override;

        /**
         * @brief Starts the Crypto_impl module.
         */
        void run() override;

        /**
         * @brief Stops the Crypto_impl module.
         */
        void stop() override;

        /**
         * @brief Encrypts the given plaintext string using AES-256-CBC.
         * @param m The plaintext string to be encrypted.
         * @return The encrypted ciphertext string.
         * @throws std::runtime_error if encryption fails.
         */
        std::string encrypt(const std::string &m) override;

        /**
         * @brief Decrypts the given ciphertext string using AES-256-CBC.
         * @param ciphertext The ciphertext string to be decrypted.
         * @return The decrypted plaintext string.
         * @throws std::runtime_error if decryption fails.
         */
        std::string decrypt(const std::string& ciphertext) override;
    };

    /**
     * @brief Factory function to create an instance of Crypto_impl.
     * @return A shared pointer to a new Crypto_impl instance.
     */
    extern "C" Module_ptr create();

} // gb
