//
// Created by ilesik on 7/14/24.
//

#pragma once

#include "./database.hpp" // Include base class definition
#include "src/modules/admin_terminal/admin_terminal.hpp"
#include "src/modules/config/config.hpp"
#include "src/modules/logging/logging.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <mysql/mysql.h>
#include <shared_mutex>
#include <thread>

namespace gb {

    /*
     * @brief Definition of Database_impl class.
     */
    class Database_impl;

    /**
     * @class Mysql_prepared_statement_params
     * @brief This class implements the Prepared_statement_params interface for MySQL prepared statements.
     *
     * The Mysql_prepared_statement_params class is responsible for managing the parameters
     * used in MySQL prepared statements. It stores the parameters and provides methods to
     * add new parameters and retrieve the stored parameters in a format suitable for MySQL
     * operations.
     */
    class Mysql_prepared_statement_params : public Prepared_statement_params {
        std::vector<MYSQL_BIND> _binds; ///< Container to hold the MySQL bind structures.
        std::list<std::string> _string_data; ///< Container to hold the string data for the parameters.
    public:
        /**
         * @brief Destructor for Mysql_prepared_statement_params.
         *
         * The destructor is defined as default since no special cleanup is required
         * for this class.
         */
        virtual ~Mysql_prepared_statement_params() = default;

        /**
         * @brief Adds a parameter to the prepared statement.
         *
         * This method adds a parameter to the MySQL prepared statement by storing
         * the parameter string and creating a corresponding MYSQL_BIND structure.
         *
         * @param param The parameter to add, represented as a string.
         */
        void add_param(const std::string &param) override;

        /**
         * @brief Retrieves the stored parameters as a vector of MYSQL_BIND structures.
         *
         * This method returns the parameters stored in the class in a format that
         * can be used with MySQL prepared statements.
         *
         * @return A vector of MYSQL_BIND structures representing the stored parameters.
         */
        std::vector<MYSQL_BIND> get_binds();
    };

    /**
     * @class Mysql_connection
     * @brief Represents a MySQL database connection.
     *
     * Manages a connection to a MySQL database, executes queries, and handles reconnecting
     * in case of connection loss.
     */
    class Mysql_connection {
    private:
        MYSQL *_conn = nullptr; ///< MySQL connection object
        Logging_ptr _log; ///< Pointer to the logging module
        Config_ptr _config; ///< Pointer to the configuration module
        Database_impl *_db; ///< Pointer to the parent database implementation
        bool _is_running = true; ///< Flag indicating if the execution thread should run.
        std::thread _execution_thread; ///< Thread for executing database tasks asynchronously

        /**
         * @brief Establishes a connection to the MySQL database.
         *
         * Initializes the MySQL connection using configuration parameters.
         */
        void connect();

        /**
         * @brief Prepares a MySQL statement.
         *
         * Initializes and prepares a MySQL statement for execution.
         *
         * @param st The prepared statement to be initialized and prepared.
         */
        void _prepare_statement(const Prepared_statement &st);

    public:
        std::mutex _mutex; ///< Mutex to protect shared resources
        std::atomic_bool _busy = false; ///< Atomic flag indicating if the connection is busy
        std::map<Prepared_statement, MYSQL_STMT *> _prepared_statements; ///< Map of prepared statements

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

        /**
         * @brief Executes a prepared statement.
         *
         * Executes the provided prepared statement with the given parameters.
         *
         * @param st The prepared statement to execute
         * @param params The parameters for the prepared statement
         * @return The result of the query as a Database_return_t
         */
        Database_return_t execute_prepared_statement(const Prepared_statement &st,
                                                     std::unique_ptr<Prepared_statement_params> params);

        /**
         * @brief Prepares a MySQL statement.
         *
         * Initializes and prepares a MySQL statement for execution.
         *
         * @param st The prepared statement to be initialized and prepared.
         */
        void prepare_statement(const Prepared_statement &st);

        /**
         * @brief Removes a prepared statement.
         *
         * Closes and removes the prepared statement from the map.
         *
         * @param st The prepared statement to be removed.
         */
        void remove_statement(const Prepared_statement &st);
    };

    /**
     * @brief Represents a queue of database tasks.
     *
     * Stores functions that return a Task representing a database operation.
     */
    typedef std::list<std::function<Task<Database_return_t>()>> Database_queue;


    /**
     * @class Database_impl
     * @brief Implements the Database interface to manage MySQL connections and execute queries.
     *
     * Manages multiple MySQL connections, executes SQL queries, and handles prepared statements.
     * Provides asynchronous execution of queries and supports background task execution.
     */
    class Database_impl : public Database {
    private:
        Logging_ptr _log; ///< Pointer to the logging module
        Config_ptr _config; ///< Pointer to the configuration module
        Admin_terminal_ptr _admin_terminal; ///< Pointer to the admin terminal module
        std::vector<std::unique_ptr<Mysql_connection>> _mysql_list{}; ///< List of MySQL connections
        std::atomic_size_t queries_amount = 0; ///< Atomic counter for active queries
        std::condition_variable _cv_stop; ///< Condition variable for stopping background tasks
        std::thread _bg_thread; ///< Thread for background task execution
        std::queue<std::string> _bg_queue; ///< Queue of background SQL queries
        std::thread _bg_stmt_thread; ///< Thread for background prepared statements execution
        std::queue<std::pair<Prepared_statement, std::unique_ptr<Prepared_statement_params>>>
            _bg_stmt_queue; ///< Queue of background prepared statements queries
        bool _is_bg_running = true; ///< Flag indicating if background tasks are running
        std::condition_variable _cv_bg; ///< Condition variable for background task execution
        std::mutex _bg_thread_mutex; ///< Mutex for _bg_queue access synchronization
        std::condition_variable _cv_bg_stmt; ///< Condition variable for background prepared statements execution
        std::mutex _bg_thread_stmt_mutex; ///< Mutex for _bg_stmt_queue access synchronization
        std::map<Prepared_statement, std::string> _prepared_statements; ///< Map of prepared statements
        std::shared_mutex _prepared_statements_mutex; ///< Mutex for _prepared_statements access synchronization
        size_t _prepared_statements_index = 0; ///< Index for generating prepared statement identifiers

        /**
         * @brief Creates a new MySQL connection and adds it to _mysql_list.
         */
        void new_connection();

    protected:
        /**
         * @brief Creates a new Prepared_statement_params object.
         *
         * @return A unique pointer to the newly created Prepared_statement_params object.
         */
        std::unique_ptr<Prepared_statement_params> get_params_object() override;

        /**
         * @brief Executes a prepared statement asynchronously.
         *
         * @param st The prepared statement to execute
         * @param params The parameters for the prepared statement
         * @return A Task that, when awaited, returns the result of the query as a Database_return_t.
         */
        Task<Database_return_t> _execute_prepared_statement(Prepared_statement st,
                                                            std::unique_ptr<Prepared_statement_params> params) override;

        /**
         * @brief Executes a prepared statement in the background.
         *
         * @param p The prepared statement to execute
         * @param params The parameters for the prepared statement
         */
        void _background_execute_prepared_statement(Prepared_statement p,
                                                    std::unique_ptr<Prepared_statement_params> params) override;

    public:
        std::mutex _mutex; ///< Mutex for _sql_queue access synchronization
        std::condition_variable _cv; ///< Condition variable for task execution
        Database_queue _sql_queue; ///< Queue of database tasks

        /**
         * @brief Constructor for Database_impl.
         *
         * Initializes the database module and starts the background execution thread.
         */
        Database_impl();

        /**
         * @breif Define destructor.
         */
        virtual ~Database_impl() = default;

        /**
         * @brief Initializes the Database_impl instance with required modules.
         *
         * @param modules Map of module names to their corresponding instances
         */
        void init(const Modules &modules) override;

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
         * @return A Task that, when awaited, returns the result of the query as a Database_return_t.
         */
        Task<Database_return_t> execute(const std::string &sql) override;

        /**
         * @brief Executes a database query in the background.
         *
         * @param sql The SQL query to execute
         */
        void background_execute(const std::string &sql) override;

        /**
         * @brief Creates a prepared statement. Bloks database until it is done in every connection.
         *
         * @param sql The SQL query for the prepared statement
         * @return The identifier for the prepared statement
         */
        Prepared_statement create_prepared_statement(const std::string &sql) override;

        /**
         * @brief Removes a prepared statement. Bloks database until it is done in every connection.
         *
         * @param st The identifier of the prepared statement to remove
         */
        void remove_prepared_statement(Prepared_statement st) override;

        /**
         * @brief Gets all prepared statements.
         *
         * @return A map of all prepared statements and their corresponding SQL queries
         */
        std::map<Prepared_statement, std::string> get_prepared_statements();

        /**
         * @brief Gets the SQL query for a prepared statement.
         *
         * @param st The identifier of the prepared statement
         * @return The SQL query of the prepared statement
         */
        std::string get_prepared_statement(const Prepared_statement &st);
    };

    /**
     * @brief Factory function to create an instance of Database_impl.
     *
     * @return A shared pointer to a newly created Database_impl instance
     */
    extern "C" Module_ptr create();

} // namespace gb
