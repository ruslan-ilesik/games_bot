//
// Created by ilesik on 7/14/24.
//

#include "database_impl.hpp"

namespace gb {
    Database_impl::Database_impl() : Database("database", {"config", "logging"}) {
        _bg_thread = std::thread([this]()->Task<void>{
            while(1){
                std::unique_lock lk (_bg_thread_mutex);
                _cv_bg.wait(lk,[this](){ return !_bg_queue.empty() || !_is_bg_running;});
                if (_bg_queue.empty() && !_is_bg_running){
                    break;
                }
                if(_bg_queue.empty()) {
                    continue;
                }
                auto e = _bg_queue.front();
                _bg_queue.pop();
                lk.unlock();
                co_await execute(e);
                queries_amount--;
                _cv_stop.notify_all();
            }
            co_return;
        });
    }

    void Database_impl::innit(const Modules &modules) {
        this->_log = std::static_pointer_cast<Logging>(modules.at("logging"));
        this->_config = std::static_pointer_cast<Config>(modules.at("config"));
    }

    void Database_impl::run() {
        auto mysql_connections_amount = std::stoi(_config->get_value_or("mysql_connections_amount", "2"));
        for (size_t i = 0; i < mysql_connections_amount; i++) {
            new_connection();
        }
    }

    void Database_impl::stop() {
        _is_bg_running = false;
        _cv_bg.notify_all();
        std::mutex m;
        std::unique_lock lk(m);
        _cv_stop.wait(lk, [this] { return queries_amount == 0; });
        _mysql_list.clear();
        _bg_thread.join();
    }

    void Database_impl::new_connection() {
        std::unique_lock lock(_mutex);
        auto c = std::make_unique<Mysql_connection>(_log, _config, this);
        _mysql_list.push_back(std::move(c));
    }

    Task<Database_return_t> Database_impl::execute(const std::string &sql) {
        queries_amount++;
        auto storage = std::make_shared<Database_return_t>();
        auto storage_mutex = std::make_shared<std::mutex>();
        std::function<Task<Database_return_t>()> tt = [sql, this, storage, storage_mutex]() -> Task<Database_return_t> {
            std::unique_lock l(*storage_mutex);
            if (storage->empty()) {
                MYSQL_RES *confres;
                Mysql_connection* conn;
                bool stop = false;
                while (true) {
                    std::unique_lock lock (_mutex);
                    for (auto &i: _mysql_list) {
                        if (!i->_busy) {
                            conn = i.get();
                            conn->_busy = true;
                            lock.unlock();
                            confres = i->execute_query(sql);
                            stop = true;
                            break;
                        }
                    }
                    if (stop) {
                        break;
                    }
                    lock.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

                // Get the number of columns
                if (confres){
                    int num_fields = mysql_num_fields(confres);
                    // Get the column names
                    MYSQL_FIELD *fields = mysql_fetch_fields(confres);
                    std::vector<std::string> column_names;
                    column_names.reserve(num_fields);
                    for (int i = 0; i < num_fields; ++i) {
                        column_names.push_back(fields[i].name);
                    }
                    // Fetch each row
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(confres))) {
                        unsigned long *lengths = mysql_fetch_lengths(confres);
                        std::map<std::string, std::string> row_map;
                        for (int i = 0; i < num_fields; ++i) {
                            if (row[i]) {
                                row_map[column_names[i]] = std::string(row[i], lengths[i]);
                            } else {
                                row_map[column_names[i]] = "NULL";
                            }
                        }
                        storage->push_back(row_map);
                    }
                    mysql_free_result(confres);
                }
                else{
                    storage->push_back({{}});
                }
                conn->_busy = false;
            }
            co_return *storage;
        };
        {
            std::unique_lock l(_mutex);
            _sql_queue.push_back(tt);
        }
        _cv.notify_all();
        Database_return_t r = co_await tt();
        queries_amount--;
        _cv_stop.notify_all();
        co_return r;
    }

    void Database_impl::background_execute(const std::string &sql) {
        {
            std::unique_lock lk (_bg_thread_mutex);
            _bg_queue.emplace(sql);
            queries_amount++;
        }
        _cv_bg.notify_all();
    }


    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Database_impl>());
    }

    void Mysql_connection::connect() {
        if (_conn == nullptr) {
            _conn = mysql_init(nullptr);
            if (_conn == nullptr) {
                _log->critical("Database ERROR:mysql_init() failed.");
                throw std::runtime_error("Database ERROR:mysql_init() failed.");
            }
        }
        std::string host = _config->get_value("mysql_ip");
        std::string user = _config->get_value("mysql_user");
        std::string password = _config->get_value("mysql_password");
        std::string db_name = _config->get_value("mysql_db_name");
        if (mysql_real_connect(_conn, host.c_str(), user.c_str(), password.c_str(), db_name.c_str(), 0, nullptr, 0) ==
            nullptr) {
            mysql_close(_conn);
            _log->critical("Database ERROR:mysql_real_connect() failed. " + std::string(mysql_error(_conn)));
            throw std::runtime_error("Database ERROR:mysql_real_connect() failed. " + std::string(mysql_error(_conn)));
        }
        _log->info("Database: new connection established");
    }

    Mysql_connection::Mysql_connection(const Logging_ptr &log, const Config_ptr &config, Database_impl *db) {
        _log = log;
        _db = db;
        _config = config;
        _execution_thread = std::thread([this]() -> Task<void> {
            while (1) {
                std::unique_lock lk(_db->_mutex);
                _db->_cv.wait(lk, [this] { return !_is_running || !_db->_sql_queue.empty(); });
                if (!_is_running && _db->_sql_queue.empty()) {
                    break;
                }
                if (_db->_sql_queue.empty()) {
                    continue;
                }
                auto task = _db->_sql_queue.front();
                _db->_sql_queue.pop_front();
                lk.unlock();
                Database_return_t r = co_await task();
            }
            co_return;
        });
        connect();
    }

    Mysql_connection::~Mysql_connection() {
        _is_running = false;
        _db->_cv.notify_all();
        _execution_thread.join();
        mysql_close(_conn);
        _log->info("Database: connection closed");
    }

    MYSQL_RES *Mysql_connection::execute_query(const std::string &query) {
        if (_conn == nullptr) {
            connect();
        }
        int query_state = mysql_query(_conn, query.c_str());
        if (query_state != 0) {
            // Check if the error is related to a lost connection
            if (mysql_errno(_conn) == CR_SERVER_LOST || mysql_errno(_conn) == CR_SERVER_GONE_ERROR) {
                _log->warn("Database: lost connection, trying to reconnect...");
                mysql_close(_conn);
                _conn = nullptr;

                // Attempt to reconnect
                connect();

                // Retry the query
                query_state = mysql_query(_conn, query.c_str());
                if (query_state != 0) {
                    _log->critical("Database ERROR: Failed to execute query after reconnecting: " +
                                   std::string(mysql_error(_conn)));
                    throw std::runtime_error("Database ERROR: Failed to execute query after reconnecting: " +
                                             std::string(mysql_error(_conn)));
                }
            } else {
                _log->critical("Database ERROR: Failed to execute query: " + std::string(mysql_error(_conn)));
                throw std::runtime_error("Database ERROR: Failed to execute query: " + std::string(mysql_error(_conn)));
            }
        }
        return  mysql_use_result(_conn);
    }

} // gb