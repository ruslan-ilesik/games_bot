//
// Created by ilesik on 3/24/26.
//

#pragma once
#include "gamescoin_manager.hpp"

namespace gb {
    class Gamescoin_manager_impl : public Gamescoin_manager {

        Database_ptr _db;

        Prepared_statement _get_user_balance_stmt;
        Prepared_statement _get_user_last_transaction_stmt;
        Prepared_statement _get_user_transactions_between_stmt;
        Prepared_statement _execute_transaction_stmt;


        Gamescoin_transaction db_return_to_transaction(Database_return_record_t& data);
        std::vector<Gamescoin_transaction> db_records_to_transactions(Database_return_t& data);

    public:
        Gamescoin_manager_impl();

        virtual ~Gamescoin_manager_impl() = default;

        void run() override;

        void stop() override;

        void init(const Modules &modules) override;

        Task<uint32_t> get_user_balance(const dpp::snowflake &user_id) override;

        Task<std::vector<Gamescoin_transaction>>
        get_user_last_transactions(const dpp::snowflake &user_id, uint32_t amount, uint32_t offset = 0) override;

        Task<std::vector<Gamescoin_transaction>>
        get_user_transactions_between(const dpp::snowflake &user_id, uint64_t t_start, uint64_t t_end,
                                      uint32_t limit = std::numeric_limits<uint32_t>::max()) override;

        Task<void> execute_transaction(const dpp::snowflake& user_id, GAMESCOIN_TRANSACTION_TYPE type, int32_t amount) override;
    };

    extern "C" Module_ptr create();
    ;
} // namespace gb
