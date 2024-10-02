//
// Created by ilesik on 10/2/24.
//

#pragma once
#include <dpp/snowflake.h>
#include <src/module/module.hpp>
#include <src/utils/coro/coro.hpp>

namespace gb {

    enum class PREMIUM_STATUS { NO_SUBSCRIPTION = 0, BASIC = 1 };

    // Implement the three-way comparison operator
    constexpr std::strong_ordering operator<=>(PREMIUM_STATUS lhs, PREMIUM_STATUS rhs) {
        return static_cast<int>(lhs) <=> static_cast<int>(rhs);
    }

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

    constexpr PREMIUM_STATUS premium_status_from_string(const std::string& status) {
        if (status == "NO_SUBSCRIPTION") {
            return PREMIUM_STATUS::NO_SUBSCRIPTION;
        }
        else if( status == "BASIC") {
            return PREMIUM_STATUS::BASIC;
        }
        throw std::invalid_argument("No known conversion from string for specific PREMIUM_STATUS");
    }

    class Premium_manager : public Module {
    public:
        Premium_manager(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies){};

          virtual Task<PREMIUM_STATUS> get_users_premium_status(const dpp::snowflake& user_id) = 0;
    };

} // namespace gb
