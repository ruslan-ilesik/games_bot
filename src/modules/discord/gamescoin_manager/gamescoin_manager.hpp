//
// Created by ilesik on 3/24/26.
//

#pragma once
#include <dpp/snowflake.h>


#include "src/module/module.hpp"
#include "src/utils/coro/coro.hpp"

namespace gb {
    enum class GAMESCOIN_TRANSACTION_TYPE { TOPGG_WEBHOOK };

    struct Gamescoin_transaction {
        int32_t amount;
        GAMESCOIN_TRANSACTION_TYPE type;
        uint64_t timestamp;
    };

    class Gamescoin_manager : public Module {
    public:
        Gamescoin_manager(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies) {}

        virtual ~Gamescoin_manager() {}

        virtual Task<uint32_t> get_user_balance(const dpp::snowflake &user_id) = 0;

        //always ordered
        virtual Task<std::vector<Gamescoin_transaction>> get_user_last_transaction(const dpp::snowflake& user_id, uint32_t amount, uint32_t offset = 0) = 0;

        //always ordered
        virtual  Task<std::vector<Gamescoin_transaction>> get_user_transactions_between (const dpp::snowflake& user_id, uint64_t t_start, uint64_t t_end, uint32_t limit = std::numeric_limits<uint32_t>::max()) = 0;


    };
} // namespace gb
