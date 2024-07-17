//
// Created by ilesik on 7/14/24.
//

#pragma once

#include "./database.hpp"  // Include base class definition
#include "src/modules/logging/logging.hpp"
#include "src/modules/config/config.hpp"

#include <iostream>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <list>
#include <mysql/mysql.h>

namespace gb {

    /*
     * @brief Definition of Database_impl class.
     */
    class Database_impl;

    /**
     * @brief Represents a MySQL database connection.
     *
     * Manages a connection to a MySQL database, executes queries, and handles reconnecting
     * in case of connection loss.
     */
    class Mysql_connection {
    private:
        MYSQL *_conn = nullptr;         ///< MySQL connection object
        Logging_ptr _log;               ///< Pointer to the logging module
        Config_ptr _config;             ///< Pointer to the configuration module
        Database_impl *_db;             ///< Pointer to the parent database implementation
        bool _is_running = true;        ///< Flag indicating if the execution thread should run.
        std::thread _execution_thread;  ///< Thread for executing database tasks asynchronously

        /**
         * @brief Establishes a connection to the MySQL database.
         *
         * Initializes the MySQL connection using configuration parameters.
         */
        void connect();

    public:
        std::atomic_bool _busy = false;  ///< Atomic flag indicating if the connection is busy

        /**
         * @brief Constructor for Mysql_connection.
         *
         * Initializes the connection and starts the execution thread.
         *
         * @param log Pointer to the logging module
         * @param config Pointer to the configuration module
         * @param db Pointer to the parent Database_impl instance
         */
        Mysql_connection(const Logging_ptr &log, const Config_ptr &config, Database_impl *db);

        // Deleted copy constructor and assignment operator
        Mysql_connection(const Mysql_connection &) = delete;
        Mysql_connection &operator=(const Mysql_connection &) = delete;

        /**
         * @brief Destructor for Mysql_connection.
         *
         * Stops the execution thread and closes the MySQL connection.
         */
        ~Mysql_connection();

        /**
         * @brief Executes a MySQL query.
         *
         * Executes the provided SQL query and handles reconnection if necessary.
         *
         * @param query The SQL query to execute
         * @return Pointer to the MySQL result object
         */
        MYSQL_RES *execute_query(const std::string &query);

    };

    /**
     * @brief Represents a queue of database tasks.
     *
     * Stores functions that return a Task representing a database operation.
     */
    typedef std::list<std::function<Task<Database_return_t>()>> Database_queue;

    /**
     * @brief Concrete implementation of the Database interface.
     *
     * Manages multiple MySQL connections, executes database queries asynchronously,
     * and handles background execution of queries.
     */
    class Database_impl : public Database {
    private:
        Logging_ptr _log;                       ///< Pointer to the logging module
        Config_ptr _config;                     ///< Pointer to the configuration module
        std::vector<std::unique_ptr<Mysql_connection>> _mysql_list{};  ///< List of MySQL connections
        std::atomic_size_t queries_amount = 0;  ///< Atomic counter for active queries
        std::condition_variable _cv_stop;       ///< Condition variable for stopping background tasks
        std::thread _bg_thread;                 ///< Thread for background task execution
        std::queue<std::string> _bg_queue;      ///< Queue of background SQL queries
        bool _is_bg_running = true;              ///< Flag indicating if background tasks are running
        std::condition_variable _cv_bg;         ///< Condition variable for background task execution
        std::mutex _bg_thread_mutex;            ///< Mutex for _bg_queue acces synchronization

        /**
         * @brief Creates a new MySQL connection and adds it to _mysql_list.
         */
        void new_connection();

    public:
        std::mutex _mutex;               ///< Mutex for _sql_queue acess synchronization
        std::condition_variable _cv;    ///< Condition variable for task execution
        Database_queue _sql_queue;       ///< Queue of database tasks

        /**
         * @brief Constructor for Database_impl.
         *
         * Initializes the database module, starts the background execution thread.
         */
        Database_impl();

        /**
         * @brief Initializes the Database_impl instance with required modules.
         *
         * @param modules Map of module names to their corresponding instances
         */
        void innit(const Modules &modules) override;

        /**
         * @brief Starts the database module and establishes MySQL connections.
         */
        void run() override;

        /**
         * @brief Stops the background execution thread and closes all MySQL connections.
         */
        void stop() override;

        /**
         * @brief Executes a database query asynchronously.
         *
         * @param sql The SQL query to execute
         * @return A Task that, when awaited, returns the result of the query
         *         as a Database_return_t (a vector of rows).
         */
        Task<Database_return_t> execute(const std::string &sql) override;

        /**
         * @brief Executes a database query in the background.
         *
         * @param sql The SQL query to execute
         */
        void background_execute(const std::string &sql) override;

    };

    /**
     * @brief Factory function to create an instance of Database_impl.
     *
     * @return A shared pointer to a newly created Database_impl instance
     */
    extern "C" Module_ptr create();

} // namespace gb
