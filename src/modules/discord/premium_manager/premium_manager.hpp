//
// Created by ilesik on 10/2/24.
//

#pragma once
#include <dpp/snowflake.h>
#include <src/module/module.hpp>
#include <src/utils/coro/coro.hpp>
#include <stdexcept> // For std::invalid_argument
#include <string>

namespace gb {

    /**
     * Enum class representing the different premium service providers.
     */
    enum class PREMIUM_PROVIDER {
        PATREON, ///< Represents a Patreon premium provider.
        MANUAL   ///< Represents a manually managed premium provider.
    };

    /**
     * Converts the PREMIUM_PROVIDER enum to its corresponding string representation.
     *
     * @param provider The PREMIUM_PROVIDER enum value.
     * @return A string representing the enum value.
     * @throws std::invalid_argument if the enum value is not recognized.
     */
    constexpr std::string to_string(PREMIUM_PROVIDER provider) {
        switch (provider) {
            case PREMIUM_PROVIDER::PATREON:
                return "PATREON";
            case PREMIUM_PROVIDER::MANUAL:
                return "MANUAL";
            default:
                throw std::invalid_argument("No known conversion to string for specific PREMIUM_PROVIDER");
        }
    }

    /**
     * Enum class representing the premium status of a user.
     */
    enum class PREMIUM_STATUS {
        NO_SUBSCRIPTION = 0, ///< No active premium subscription.
        BASIC = 1            ///< Basic premium subscription.
    };

    /**
     * Three-way comparison operator for PREMIUM_STATUS, allowing comparison between statuses.
     *
     * @param lhs The left-hand side PREMIUM_STATUS.
     * @param rhs The right-hand side PREMIUM_STATUS.
     * @return std::strong_ordering indicating the result of the comparison.
     */
    constexpr std::strong_ordering operator<=>(PREMIUM_STATUS lhs, PREMIUM_STATUS rhs) {
        return static_cast<int>(lhs) <=> static_cast<int>(rhs);
    }

    /**
     * Converts the PREMIUM_STATUS enum to its corresponding string representation.
     *
     * @param s The PREMIUM_STATUS enum value.
     * @return A string representing the enum value.
     * @throws std::invalid_argument if the enum value is not recognized.
     */
    constexpr std::string to_string(PREMIUM_STATUS s) {
        switch (s) {
            case PREMIUM_STATUS::NO_SUBSCRIPTION:
                return "NO_SUBSCRIPTION";
            case PREMIUM_STATUS::BASIC:
                return "BASIC";
            default:
                throw std::invalid_argument("No known conversion to string for specific PREMIUM_STATUS");
        }
    }

    /**
     * Converts a string to its corresponding PREMIUM_STATUS enum value.
     *
     * @param status The string representation of the premium status.
     * @return The corresponding PREMIUM_STATUS enum value.
     * @throws std::invalid_argument if the string value is not recognized.
     */
    constexpr PREMIUM_STATUS premium_status_from_string(const std::string& status) {
        if (status == "NO_SUBSCRIPTION") {
            return PREMIUM_STATUS::NO_SUBSCRIPTION;
        }
        else if( status == "BASIC") {
            return PREMIUM_STATUS::BASIC;
        }
        throw std::invalid_argument("No known conversion from string for specific PREMIUM_STATUS");
    }

    /**
     * Abstract base class for managing premium subscriptions.
     *
     * This class provides the interface for handling user premium status.
     */
    class Premium_manager : public Module {
    public:
        /**
         * Constructor for Premium_manager.
         *
         * @param name The name of the module.
         * @param dependencies A list of module dependencies required by this manager.
         */
        Premium_manager(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies) {}

        /**
         * Retrieves the premium status of a user.
         *
         * @param user_id The Discord user ID (snowflake) of the user.
         * @return A Task object representing the user's premium status.
         */
        virtual Task<PREMIUM_STATUS> get_users_premium_status(const dpp::snowflake& user_id) = 0;

        /**
         *
         * @return A url to buy subscription.
         **/
        virtual std::string get_premium_buy_url() = 0;
    };

    /**
     * @brief Typedef for Premium_manager_ptr.
     */
    typedef std::shared_ptr<Premium_manager> Premium_manager_ptr;


} // namespace gb
