//
// Created by ilesik on 10/27/24.
//

#pragma once

#include <drogon/drogon.h>
#include <jwt/jwt.hpp>

#include <src/modules/discord/discord_statistics_collector/discord_statistics_collector.hpp>
#include "./webserver.hpp"

#include <src/modules/config/config.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord_achievements_processing/discord_achievements_processing.hpp>
#include <src/modules/discord/discord_command_handler/discord_command_handler.hpp>
#include <src/modules/discord/premium_manager/premium_manager.hpp>
#include <src/modules/logging/logging.hpp>

namespace gb {

    /**
     * @class Webserver_impl
     * @brief Implementation of the Webserver interface.
     *
     * The Webserver_impl class manages the main web server logic, including initializing,
     * running, and stopping the server. It handles background tasks, API routes, and cookie management.
     */
    class Webserver_impl : public Webserver {
        std::thread _drogon_thread; ///< Thread for running the Drogon web server.
        std::thread _background_worker; ///< Thread for executing periodic background tasks.
        std::condition_variable _cv; ///< Condition variable to manage background worker sleep and stop behavior.
        std::mutex _mutex; ///< Mutex to synchronize access to shared data between threads.
        bool _stop = false; ///< Flag indicating whether the server should stop.

        Prepared_statement _delete_cookie_stmt; ///< Prepared statement for deleting cookies from the database.
        Prepared_statement _cookie_exists_stmt; ///< Prepared statement for checking cookie existence in the database.

    public:
        Discord_statistics_collector_ptr discord_stats; ///< Pointer to the Discord statistics collector module.
        Config_ptr config; ///< Pointer to the configuration module.
        Discord_command_handler_ptr commands_handler; ///< Pointer to the Discord command handler module.
        Logging_ptr log; ///< Pointer to the logging module.
        Premium_manager_ptr premium_manager; ///< Pointer to the premium manager module.
        Discord_achievements_processing_ptr achievements_manager; ///< Pointer to the achievements processing module.

        std::atomic_uint64_t current_jwt_id; ///< Atomic counter for generating unique JWT identifiers.

        std::vector<std::function<void()>> on_stop; ///< List of callback functions to execute on server stop.

        Database_ptr db; ///< Pointer to the database module.

        /**
         * @brief Constructor for Webserver_impl.
         * Initializes the server with required modules and sets up its internal state.
         */
        Webserver_impl();

        /**
         * @brief Destructor for Webserver_impl.
         * Cleans up resources used by the server.
         */
        ~Webserver_impl() = default;

        /**
         * @brief Stops the server and background worker.
         * Cleans up resources, notifies threads to stop, and performs shutdown operations.
         */
        void stop() override;

        /**
         * @brief Starts the server and background tasks.
         * Initializes prepared statements, background worker, and routes, then starts the Drogon server.
         */
        void run() override;

        /**
         * @brief Initializes the server with the provided modules.
         *
         * @param modules A map containing references to required modules.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Deletes a cookie from the database.
         *
         * @param id The unique identifier of the cookie to delete.
         * @return A Task<void> representing the asynchronous operation.
         */
        Task<void> delete_cookie(uint64_t id);

        /**
         * @brief Checks if a cookie exists in the database.
         *
         * @param id The unique identifier of the cookie to check.
         * @return A Task<bool> indicating whether the cookie exists.
         */
        Task<bool> check_if_cookie_exists(uint64_t id);
    };

    /**
     * @brief Factory function to create a new Webserver_impl instance.
     *
     * @return A shared pointer to the created Webserver_impl instance.
     */
    extern "C" Module_ptr create();

} // namespace gb
