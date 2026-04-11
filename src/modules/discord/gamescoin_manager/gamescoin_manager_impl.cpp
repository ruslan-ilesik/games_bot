//
// Created by ilesik on 3/24/26.
//

#include "gamescoin_manager_impl.hpp"

#include <ranges>

namespace gb {

    Gamescoin_transaction Gamescoin_manager_impl::db_return_to_transaction(Database_return_record_t &data) {
        return {static_cast<int32_t>(std::stol(data["amount"])), gamescoin_transaction_type_from_string(data["source"]), std::stoull(data["transaction_time"])};
    }

    std::vector<Gamescoin_transaction> Gamescoin_manager_impl::db_records_to_transactions(Database_return_t &data) {
        auto view = data | std::views::transform([this](auto& i) {
            return db_return_to_transaction(i);
        });

        return {view.begin(), view.end()};
    }

    Gamescoin_manager_impl::Gamescoin_manager_impl() : Gamescoin_manager("gamescoin_manager", {"database"}) {}

    void Gamescoin_manager_impl::run() {}

    void Gamescoin_manager_impl::stop() {
        _db->remove_prepared_statement(_get_user_balance_stmt);
        _db->remove_prepared_statement(_get_user_last_transaction_stmt);
        _db->remove_prepared_statement(_get_user_transactions_between_stmt);
        _db->remove_prepared_statement(_execute_transaction_stmt);
    }

    void Gamescoin_manager_impl::init(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _execute_transaction_stmt = _db->create_prepared_statement("CALL gamescoin_transaction(?,?,?);");
        _get_user_balance_stmt =
            _db->create_prepared_statement("SELECT balance from gamescoin_balances where user_id=?;");
        _get_user_last_transaction_stmt =
            _db->create_prepared_statement("select amount,`source`,transaction_time from gamescoin_transactions where "
                                           "user_id = ? ORDER BY transaction_time DESC LIMIT ? OFFSET ?;");
        _get_user_transactions_between_stmt = _db->create_prepared_statement("select amount,`source`,transaction_time from gamescoin_transactions where user_id = ? and transaction_time >= ? and transaction_time <= ? Order by transaction_time ASC LIMIT ?;");
    }

    Task<uint32_t> Gamescoin_manager_impl::get_user_balance(const dpp::snowflake &user_id) {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_user_balance_stmt, user_id);
        if (r.empty()) {
            co_return 0;
        }
        co_return static_cast<uint32_t>(std::stoul(r[0]["balance"]));
    };

    Task<std::vector<Gamescoin_transaction>>
    Gamescoin_manager_impl::get_user_last_transactions(const dpp::snowflake &user_id, uint32_t amount, uint32_t offset) {
        auto r = co_await _db->execute_prepared_statement(_get_user_last_transaction_stmt, user_id, amount, offset);
        co_return db_records_to_transactions(r);
    }

    Task<std::vector<Gamescoin_transaction>>
    Gamescoin_manager_impl::get_user_transactions_between(const dpp::snowflake &user_id, uint64_t t_start,
                                                          uint64_t t_end, uint32_t limit) {
        auto r = co_await _db->execute_prepared_statement(_get_user_transactions_between_stmt,user_id,t_start,t_end,limit);
        co_return db_records_to_transactions(r);
    }

    Task<void> Gamescoin_manager_impl::execute_transaction(const dpp::snowflake &user_id,
                                                           GAMESCOIN_TRANSACTION_TYPE type, int32_t amount) {
        co_await _db->execute_prepared_statement(_execute_transaction_stmt, user_id, amount,to_string(type));
        co_return;
    }


    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Gamescoin_manager_impl>()); };
}; // namespace gb
