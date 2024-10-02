//
// Created by ilesik on 10/2/24.
//

#include "premium_manager_impl.hpp"

namespace gb {
    Premium_manager_impl::Premium_manager_impl() : Premium_manager("premium_manager", {"database"}) {}

    void Premium_manager_impl::init(const Modules &modules) {
        _db = std::static_pointer_cast<Database>(modules.at("database"));
        _remove_all_expired_subscriptions = _db->create_prepared_statement(
            "UPDATE `premium` SET status='ENDED' where `end_time` IS NOT NULL AND `end_time` < UTC_TIMESTAMP()");
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
                _db->background_execute_prepared_statement(_remove_all_expired_subscriptions);
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
        _db->remove_prepared_statement(_remove_all_expired_subscriptions);
    }

    Task<PREMIUM_STATUS> Premium_manager_impl::get_users_premium_status(const dpp::snowflake &user_id) {

        co_return PREMIUM_STATUS::NO_SUBSCRIPTION;
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Premium_manager_impl>()); }
} // namespace gb
