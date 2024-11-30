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

    class Webserver_impl : public Webserver {
        std::thread _drogon_thread;
        std::thread _background_worker;
        std::condition_variable _cv;                ///< Condition variable to manage background worker sleep and stop behavior.
        std::mutex _mutex;                          ///< Mutex to synchronize access to shared data between threads.
        bool _stop = false;

        Prepared_statement _delete_cookie_stmt;
        Prepared_statement _cookie_exists_stmt;
    public:
        Discord_statistics_collector_ptr discord_stats;
        Config_ptr config;
        Discord_command_handler_ptr commands_handler;
        Logging_ptr log;
        Premium_manager_ptr premium_manager;
        Discord_achievements_processing_ptr achievements_manager;

        std::atomic_uint64_t current_jwt_id;

        std::vector<std::function<void()>> on_stop;

        Database_ptr db;

        Webserver_impl();

        ~Webserver_impl() = default;

        void stop() override;
        void run() override;
        void init(const Modules &modules) override;

        Task<void>  delete_cookie(uint64_t id);
        Task<bool>  check_if_cookie_exists(uint64_t id);
    };

    extern "C" Module_ptr create();

} // namespace gb
