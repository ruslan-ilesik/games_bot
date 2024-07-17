//
// Created by ilesik on 7/14/24.
//

#pragma once
#include <coroutine>
#include <vector>
#include <map>
#include <memory>

#include "../../module/module.hpp"
#include "src/utils/coro/coro.hpp"

namespace gb {

    /**
     * @brief Type representing the result of a database query.
     *
     * This type is a vector of maps, where each map represents a row
     * from the query result. Each entry in the map consists of a column
     * name (as a string) and its corresponding value (also as a string).
     */
    typedef std::vector<std::map<std::string, std::string>> Database_return_t;

    /**
     * @brief Abstract base class for database modules.
     *
     * This class serves as a blueprint for any database implementation
     * and provides pure virtual functions that must be overridden in
     * derived classes. It facilitates asynchronous database operations
     * using coroutines.
     */
    class Database : public Module {
    public:
        /**
         * @brief Constructor for the Database class.
         *
         * @param name The name of the database module.
         * @param dep A vector of dependencies required by this module.
         */
        Database(const std::string& name, const std::vector<std::string>& dep)
            : Module(name, dep) {}

        /**
         * @brief Executes a SQL query asynchronously and returns the result.
         *
         * @param sql The SQL query to execute.
         * @return A Task that, when awaited, returns the result of the query
         *         as a Database_return_t (a vector of rows).
         */
        virtual Task<Database_return_t> execute(const std::string& sql) = 0;

        /**
         * @brief Executes a SQL query in the background without waiting for a result.
         *
         * @param sql The SQL query to execute.
         */
        virtual void background_execute(const std::string& sql) = 0;

    };

    /**
     * @brief Shared pointer type for Database objects.
     */
    typedef std::shared_ptr<Database> Database_ptr;
} // namespace gb
