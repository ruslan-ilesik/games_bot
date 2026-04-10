//
// Created by ilesik on 3/24/26.
//

#include "gamescoin_manager_impl.hpp"

namespace gb {

    Gamescoin_manager_impl::Gamescoin_manager_impl() : Gamescoin_manager("gamescoin_manager", {"database"}) {}

    void Gamescoin_manager_impl::run() {}

    void Gamescoin_manager_impl::stop() {}

    void Gamescoin_manager_impl::init(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
    }

    Task<uint32_t> Gamescoin_manager_impl::get_user_balance(const dpp::snowflake &user_id) {
        co_return 0;
    }

    Task<std::vector<Gamescoin_transaction>>
    Gamescoin_manager_impl::get_user_last_transaction(const dpp::snowflake &user_id, uint32_t amount, uint32_t offset) {
        co_return {};
    }
    Task<std::vector<Gamescoin_transaction>>
    Gamescoin_manager_impl::get_user_transactions_between(const dpp::snowflake &user_id, uint64_t t_start,
                                                          uint64_t t_end, uint32_t limit) {
        co_return {};
    }


    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Gamescoin_manager_impl>()); };
}; // namespace gb
