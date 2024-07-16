//
// Created by ilesik on 7/14/24.
//

#pragma once

#include "./database.hpp"
#include "src/modules/logging/logging.hpp"
#include "src/modules/config/config.hpp"

#include <iostream>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <list>
#include <mysql/mysql.h>

namespace gb {

    class Database_impl;

    class Mysql_connection {
    private:
        MYSQL *_conn = nullptr;
        Logging_ptr _log;
        Config_ptr _config;
        Database_impl *_db;
        bool _is_running = true;
        std::thread _execution_thread;
        std::mutex _mutex;

        void connect();

    public:
        std::atomic_bool _busy = false;

        Mysql_connection(const Logging_ptr &log, const Config_ptr &config, Database_impl *db);

        // Delete copy constructor
        Mysql_connection(const Mysql_connection &) = delete;

        // Delete copy assignment operator
        Mysql_connection &operator=(const Mysql_connection &) = delete;

        ~Mysql_connection();

        MYSQL_RES *execute_query(const std::string &query);

    };

    class Database_impl;

    typedef std::list<std::function<Task<Database_return_t>()>> Database_queue;

    class Database_impl : public Database {
    private:
        Logging_ptr _log;
        Config_ptr _config;
        std::vector<std::unique_ptr<Mysql_connection>> _mysql_list{};
        size_t queries_amount = 0;
        // used for proper stop;
        std::condition_variable _cv_stop;

        void new_connection();

    public:
        std::mutex _mutex;
        std::condition_variable _cv;
        Database_queue _sql_queue;

        Database_impl();

        void stop() override;

        void run() override;

        void innit(const Modules &modules) override;

        Task<Database_return_t> execute(const std::string &sql) override;;


    };


    extern "C" Module_ptr create();
} // gb


