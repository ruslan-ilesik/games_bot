//
// Created by ilesik on 10/27/24.
//

#pragma once

#include <drogon/drogon.h>
#include <src/modules/discord/discord_statistics_collector/discord_statistics_collector.hpp>
#include "./webserver.hpp"

#include <src/modules/config/config.hpp>
#include <src/modules/database/database.hpp>

namespace gb {

    class Webserver_impl : public Webserver {
        std::thread _drogon_thread;
    public:
        Discord_statistics_collector_ptr discord_stats;
        Config_ptr config;
        std::vector<std::function<void()>> on_stop;

        Database_ptr db;

        Webserver_impl();

        ~Webserver_impl() = default;

        void stop() override;
        void run() override;
        void init(const Modules &modules) override;
    };

    extern "C" Module_ptr create();

} // namespace gb
