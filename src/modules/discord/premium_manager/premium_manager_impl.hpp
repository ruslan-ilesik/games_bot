//
// Created by ilesik on 10/2/24.
//

#pragma once
#include <src/modules/database/database.hpp>
#include "./premium_manager.hpp"
#include <thread>
#include <condition_variable>
#include <mutex>

namespace gb {

    /**
     * Implementation of the Premium_manager interface.
     *
     * This class handles the premium subscription management, including
     * retrieving premium statuses, managing background tasks, and interacting
     * with the database to update subscription information.
     */
    class Premium_manager_impl : public Premium_manager {
        Database_ptr _db;                           ///< Pointer to the database module used for managing premium subscription data.
        std::thread _background_worker;             ///< Background worker thread responsible for running periodic tasks.
        std::condition_variable _cv;                ///< Condition variable to manage background worker sleep and stop behavior.
        std::mutex _mutex;                          ///< Mutex to synchronize access to shared data between threads.
        bool _stop = false;                         ///< Flag to signal the background worker to stop.

        Prepared_statement _remove_all_expired_subscriptions_stmt; ///< Prepared statement for removing expired premium subscriptions.
        Prepared_statement _get_user_premium_status_stmt;          ///< Prepared statement for retrieving a user's premium status.

    public:
        /**
         * Default constructor for Premium_manager_impl.
         * Initializes the premium manager implementation with required settings.
         */
        Premium_manager_impl();

        /**
         * Virtual destructor.
         * Ensures proper cleanup of resources.
         */
        virtual ~Premium_manager_impl() = default;

        /**
         * Initializes the Premium_manager_impl with the required module dependencies.
         *
         * @param modules The collection of modules that this manager depends on, such as the database.
         */
        void init(const Modules &modules) override;

        /**
         * Starts the background worker that periodically checks and updates premium subscriptions.
         */
        void run() override;

        /**
         * Stops the background worker and ensures the cleanup of resources.
         * Signals the worker thread to stop and waits for its completion.
         */
        void stop() override;

        /**
        *
        * @return A url to buy subscription.
        **/
        std::string get_premium_buy_url() override;

        /**
         * Retrieves the premium status of a specific user.
         *
         * @param user_id The Discord user ID (snowflake) of the user whose premium status is being queried.
         * @return A Task that resolves to the user's current PREMIUM_STATUS.
         */
        Task<PREMIUM_STATUS> get_users_premium_status(const dpp::snowflake& user_id) override;

    };

    /**
     * Factory function to create an instance of Premium_manager_impl.
     *
     * @return A shared pointer to a newly created Premium_manager_impl module.
     */
    extern "C" Module_ptr create();

} // namespace gb
