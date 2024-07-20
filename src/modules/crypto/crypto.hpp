//
// Created by ilesik on 7/20/24.
//

#pragma once

#include "src/module/module.hpp"
#include <string>
#include <vector>

namespace gb {

    /**
     * @class Crypto
     * @brief Abstract base class for cryptographic operations.
     *        Inherits from the Module class.
     */
    class Crypto : public Module {
    public:
        /**
         * @brief Constructor for Crypto.
         * @param name The name of the module.
         * @param dependencies The list of dependencies for the module.
         */
        Crypto(const std::string& name, const std::vector<std::string>& dependencies)
            : Module(name, dependencies) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~Crypto() = default;

        /**
         * @brief Pure virtual function to encrypt a plaintext string.
         * @param m The plaintext string to be encrypted.
         * @return The encrypted ciphertext string.
         */
        virtual std::string encrypt(const std::string& m) = 0;

        /**
         * @brief Pure virtual function to decrypt a ciphertext string.
         * @param ciphertext The ciphertext string to be decrypted.
         * @return The decrypted plaintext string.
         */
        virtual std::string decrypt(const std::string& ciphertext) = 0;
    };

} // namespace gb
