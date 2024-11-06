//
// Created by ilesik on 10/27/24.
//

#pragma once

#include <drogon/drogon.h>
#include <src/modules/discord/discord_statistics_collector/discord_statistics_collector.hpp>
#include "./webserver.hpp"

#include <src/modules/config/config.hpp>
#include <src/modules/database/database.hpp>
#include <src/modules/discord/discord_command_handler/discord_command_handler.hpp>

namespace gb {

    class Webserver_impl : public Webserver {
        std::thread _drogon_thread;
    public:
        Discord_statistics_collector_ptr discord_stats;
        Config_ptr config;
        Discord_command_handler_ptr commands_handler;

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
