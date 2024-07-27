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
     * @brief Type representing a prepared statement identifier.
     *
     * This type is an alias for size_t and is used to uniquely identify
     * prepared statements within the database module.
     */
    typedef size_t Prepared_statement;

    /**
     * @brief Abstract base class for prepared statement parameters.
     *
     * This class defines the interface for adding parameters to a prepared
     * statement. Derived classes must implement the add_param method to
     * handle specific parameter types and conversions.
     */
    class Prepared_statement_params {
    public:
        /**
         * @brief Adds a parameter to the prepared statement.
         *
         * @param param The parameter to add, represented as a string.
         */
        virtual void add_param(const std::string &param) = 0;

        /**
         * @brief Virtual destructor for Prepared_statement_params.
         */
        virtual ~Prepared_statement_params() = default;
    };

    /**
     * @brief Abstract base class for database modules.
     *
     * This class serves as a blueprint for any database implementation
     * and provides pure virtual functions that must be overridden in
     * derived classes. It facilitates asynchronous database operations
     * using coroutines.
     */
    class Database : public Module {
    protected:
        /**
         * @brief Creates an object to hold prepared statement parameters.
         *
         * Derived classes must implement this function to provide an
         * appropriate implementation of Prepared_statement_params.
         *
         * @return A unique_ptr to a Prepared_statement_params object.
         */
        virtual std::unique_ptr<Prepared_statement_params> get_params_object() = 0;

        /**
         * @brief Executes a prepared statement asynchronously.
         *
         * Derived classes must implement this function to handle the
         * execution of prepared statements using coroutines.
         *
         * @param p The identifier of the prepared statement.
         * @param params The parameters to bind to the prepared statement.
         * @return A Task that, when awaited, returns the result of the query
         *         as a Database_return_t.
         */
        virtual Task<Database_return_t>
        _execute_prepared_statement(Prepared_statement p, std::unique_ptr<Prepared_statement_params> params) = 0;

        /**
         * @brief Executes a prepared statement in the background.
         *
         * Derived classes must implement this function to handle the
         * execution of prepared statements without waiting for the result.
         *
         * @param p The identifier of the prepared statement.
         * @param params The parameters to bind to the prepared statement.
         */
        virtual void _background_execute_prepared_statement(Prepared_statement p, std::unique_ptr<Prepared_statement_params> params) = 0;

    public:
        /**
         * @brief Constructor for the Database class.
         *
         * @param name The name of the database module.
         * @param dep A vector of dependencies required by this module.
         */
        Database(const std::string &name, const std::vector<std::string> &dep)
            : Module(name, dep) {}

        /**
         * @brief Executes a SQL query asynchronously and returns the result.
         *
         * @param sql The SQL query to execute.
         * @return A Task that, when awaited, returns the result of the query
         *         as a Database_return_t (a vector of rows).
         */
        virtual Task<Database_return_t> execute(const std::string &sql) = 0;

        /**
         * @brief Executes a SQL query in the background without waiting for a result.
         *
         * @param sql The SQL query to execute.
         */
        virtual void background_execute(const std::string &sql) = 0;

        /**
         * @brief Creates a prepared statement from the given SQL query.
         *
         * Derived classes must implement this function to handle the creation
         * of prepared statements.
         *
         * @param sql The SQL query to prepare.
         * @return A Prepared_statement identifier for the prepared statement.
         */
        virtual Prepared_statement create_prepared_statement(const std::string &sql) = 0;

        /**
         * @brief Removes a prepared statement.
         *
         * Derived classes must implement this function to handle the removal
         * of prepared statements.
         *
         * @param st The identifier of the prepared statement to remove.
         */
        virtual void remove_prepared_statement(Prepared_statement st) = 0;

        // Utility function to convert various types to strings
        /**
         * @brief Converts a value to a string.
         *
         * This template function handles the conversion of various types
         * to strings, except for std::string.
         *
         * @param value The value to convert.
         * @return The string representation of the value.
         */
        template<typename T>
        std::enable_if_t<!std::is_same_v<T, std::string>, std::string> to_string(const T &value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        /**
         * @brief Overload for converting std::string to string.
         *
         * @param value The string value to convert.
         * @return The string itself.
         */
        std::string to_string(const std::string &value) {
            return value;
        }

        /**
         * @brief Overload for converting const char* to string.
         *
         * @param value The C-style string to convert.
         * @return A std::string object containing the value.
         */
        std::string to_string(const char *value) {
            return std::string(value);
        }

        /**
         * @brief Executes a prepared statement asynchronously with parameters.
         *
         * This function uses variadic templates to accept any number of
         * parameters of any type, converts them to strings, and binds them
         * to the prepared statement.
         *
         * @param pt The identifier of the prepared statement.
         * @param args The parameters to bind to the prepared statement.
         * @return A Task that, when awaited, returns the result of the query
         *         as a Database_return_t.
         */
        template<typename... Args>
        Task<Database_return_t> execute_prepared_statement(Prepared_statement pt, Args &&... args) {
            auto params = this->get_params_object();
            auto add_param = [&params, this](const auto &arg) {
                params->add_param(to_string(arg));
            };
            // Expand the variadic template and apply add_param to each argument
            (add_param(std::forward<Args>(args)), ...);
            Database_return_t result = co_await _execute_prepared_statement(pt, std::move(params));
            co_return {result};
        }

        /**
         * @brief Executes a prepared statement in the background with parameters.
         *
         * This function uses variadic templates to accept any number of
         * parameters of any type, converts them to strings, and binds them
         * to the prepared statement.
         *
         * @param pt The identifier of the prepared statement.
         * @param args The parameters to bind to the prepared statement.
         */
        template<typename... Args>
        void background_execute_prepared_statement(Prepared_statement pt, Args &&... args){
            auto params = this->get_params_object();
            auto add_param = [&params, this](const auto &arg) {
                params->add_param(to_string(arg));
            };
            // Expand the variadic template and apply add_param to each argument
            (add_param(std::forward<Args>(args)), ...);
            _background_execute_prepared_statement(pt, std::move(params));
        }
    };

    /**
     * @brief Shared pointer type for Database objects.
     */
    typedef std::shared_ptr<Database> Database_ptr;
} // namespace gb
