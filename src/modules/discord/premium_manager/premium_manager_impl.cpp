//
// Created by ilesik on 10/2/24.
//

#include "premium_manager_impl.hpp"

namespace gb {
    Premium_manager_impl::Premium_manager_impl() : Premium_manager("premium_manager", {"database"}) {}

    void Premium_manager_impl::init(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _remove_all_expired_subscriptions_stmt = _db->create_prepared_statement(
            "UPDATE `premium` SET status='ENDED' where `end_time` IS NOT NULL AND `end_time` < UTC_TIMESTAMP()");
        _get_user_premium_status_stmt = _db->create_prepared_statement(
            "SELECT `type` from `premium` where `status`='ACTIVE' and `user_id`=? order by `type` desc limit 1;");
    }

    void Premium_manager_impl::run() {
        _background_worker = std::thread{[this]() {
            while (1) {
                std::unique_lock lk(_mutex);
                // Wait for either _stop == true or a 60-second timeout
                bool result = _cv.wait_for(lk, std::chrono::seconds(60), [this]() { return _stop; });

                if (result) {
                    break;
                }
                _db->background_execute_prepared_statement(_remove_all_expired_subscriptions_stmt);
            }
            return;
        }};
    }

    void Premium_manager_impl::stop() {
        {
            std::unique_lock lk(_mutex);
            _stop = true;
            _cv.notify_all();
        }
        _background_worker.join();
        _db->remove_prepared_statement(_remove_all_expired_subscriptions_stmt);
        _db->remove_prepared_statement(_get_user_premium_status_stmt);
    }

    Task<PREMIUM_STATUS> Premium_manager_impl::get_users_premium_status(const dpp::snowflake &user_id) {
        Database_return_t r = co_await _db->execute_prepared_statement(_get_user_premium_status_stmt, user_id);
        if (r.empty()) {
            co_return PREMIUM_STATUS::NO_SUBSCRIPTION;
        }
        co_return premium_status_from_string(r.at(0).at("type"));
    }


    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Premium_manager_impl>()); }
} // namespace gb
