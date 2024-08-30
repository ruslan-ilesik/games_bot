//
// Created by ilesik on 7/14/24.
//

#include "database_impl.hpp"

namespace gb {
    Database_impl::Database_impl() : Database("database", {"config", "logging", "admin_terminal"}) {
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
        _bg_stmt_thread = std::thread([this]()->Task<void>{
            while(1){
                std::unique_lock lk (_bg_thread_stmt_mutex);
                _cv_bg_stmt.wait(lk,[this](){ return !_bg_stmt_queue.empty() || !_is_bg_running;});
                if (_bg_stmt_queue.empty() && !_is_bg_running){
                    break;
                }
                if(_bg_stmt_queue.empty()) {
                    continue;
                }
                auto e = std::move(_bg_stmt_queue.front());
                _bg_stmt_queue.pop();
                lk.unlock();
                co_await _execute_prepared_statement(e.first,std::move(e.second));
                queries_amount--;
                _cv_stop.notify_all();
            }
            co_return;
        });
    }

    void Database_impl::init(const Modules &modules) {
        this->_log = std::static_pointer_cast<Logging>(modules.at("logging"));
        this->_config = std::static_pointer_cast<Config>(modules.at("config"));
        this->_admin_terminal = std::static_pointer_cast<Admin_terminal>(modules.at("admin_terminal"));

        _admin_terminal->add_command(
            "database_add_connection",
            "Adds new database connection to pool",
            "Arguments: no arguments.",
            [this](const std::vector<std::string>& arguments){
                try{
                    new_connection();
                }
                catch(...){
                    std::cout << "database_add_connection command error: Error adding connection" << std::endl;
                }
            }
        );

        _admin_terminal->add_command(
            "database_get_connections",
            "Prints all connection from pool and their status",
            "Arguments: no arguments.",
            [this](const std::vector<std::string>& arguments){
                try{
                    std::unique_lock lock(_mutex);
                    std::string r = "Connections list:\ntotal amount: " + std::to_string(_mysql_list.size());
                    size_t cnt = 0;
                    for (auto& i : _mysql_list){
                        r += std::format("\n{}) is busy: {}",cnt, i->_busy.load());
                        cnt++;
                    }
                    std::cout << r << std::endl;
                }
                catch(...){
                    std::cout << "database_remove_connection command error: Error printing connections" << std::endl;
                }
            }
        );

        _admin_terminal->add_command(
            "database_request_queue_size",
            "Shows length of request queue (including currently running)",
            "Arguments: no arguments.",
            [this](const std::vector<std::string>& arguments){
                try{
                    std::unique_lock lock(_bg_thread_mutex);
                    std::cout << "Database queries in queue: " << (queries_amount - _bg_queue.size()) << std::endl;
                }
                catch(...){
                    std::cout << "database_remove_connection command error: Error getting request queue size" << std::endl;
                }
            }
        );
    }

    void Database_impl::run() {
        auto mysql_connections_amount = std::stoi(_config->get_value_or("mysql_connections_amount", "2"));
        for (size_t i = 0; i < mysql_connections_amount; i++) {
            new_connection();
        }
    }

    void Database_impl::stop() {
        _admin_terminal->remove_command("database_get_connections");
        _admin_terminal->remove_command("database_add_connection");
        _admin_terminal->remove_command("database_request_queue_size");
        _is_bg_running = false;
        _cv_bg.notify_all();
        _cv_bg_stmt.notify_all();
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
                std::unique_lock<std::mutex> r;
                while (true) {
                    std::unique_lock lock (_mutex);
                    for (auto &i: _mysql_list) {
                        if (!i->_busy) {
                            r = std::unique_lock(i->_mutex);
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

    std::map<Prepared_statement, std::string> Database_impl::get_prepared_statements() {
        std::shared_lock lk (_prepared_statements_mutex);
        return _prepared_statements;
    }

    std::string Database_impl::get_prepared_statement(const Prepared_statement &st) {
        std::shared_lock lk (_prepared_statements_mutex);
        return _prepared_statements.at(st);
    }

    void Database_impl::remove_prepared_statement(Prepared_statement st) {
        {
            std::unique_lock lk2(_mutex);
            std::unique_lock lk(_prepared_statements_mutex);
            auto tt = [this, st]() -> Task<Database_return_t> {
                for (auto &i: _mysql_list) {
                    i->remove_statement(st);
                }
                _prepared_statements.erase(st);
                co_return {};
            };
            _sql_queue.emplace_back(tt);
        }
        _cv.notify_all();
    }

    Prepared_statement Database_impl::create_prepared_statement(const std::string &sql) {
        std::unique_lock lk2 (_mutex);
        std::unique_lock lk(_prepared_statements_mutex);

        while (_prepared_statements.contains(_prepared_statements_index)) {
            if (_prepared_statements_index == std::numeric_limits<Prepared_statement>::max()) {
                _prepared_statements_index = std::numeric_limits<Prepared_statement>::min();
            } else {
                _prepared_statements_index++;
            }
        }
        _prepared_statements[_prepared_statements_index] = sql;
        auto ind = _prepared_statements_index;
        lk.unlock();
        for (auto &i: _mysql_list) {
            i->prepare_statement(_prepared_statements_index);
        }
        return ind;
    }

    std::unique_ptr<Prepared_statement_params> Database_impl::get_params_object() {
        return std::move(std::make_unique<Mysql_prepared_statement_params>());
    }

    Task<Database_return_t>
    Database_impl::_execute_prepared_statement(Prepared_statement st, std::unique_ptr<Prepared_statement_params> params) {
        queries_amount++;
        auto storage = std::make_shared<Database_return_t>();
        auto storage_mutex = std::make_shared<std::mutex>();

        auto tt = [st, &params, this, storage, storage_mutex]() -> Task<Database_return_t> {
            typedef unsigned char my_bool;

            std::unique_lock l(*storage_mutex);
            if (storage->empty()) {
                Mysql_connection* conn = nullptr;
                bool stop = false;
                std::unique_lock<std::mutex> r;
                while (!stop) {
                    std::unique_lock lock(_mutex);
                    for (auto &i: _mysql_list) {
                        if (!i->_busy) {
                            r = std::unique_lock(i->_mutex);
                            conn = i.get();
                            conn->_busy = true;
                            lock.unlock();
                            try {
                                auto temp = i->execute_prepared_statement(st, std::move(params));
                                storage->insert(storage->end(),temp.begin(),temp.end());
                                if (storage->empty()){
                                    storage->push_back({{}});
                                }
                                stop = true;
                            } catch (const std::runtime_error& e) {
                                conn->_busy = false;
                                throw;
                            }
                            break;
                        }
                    }
                    if (!stop) {
                        lock.unlock();
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                }
                conn->_busy = false;
            }
            co_return *storage;
        };
        {
            std::unique_lock l(_mutex);
            _sql_queue.emplace_back(tt);
        }
        _cv.notify_all();

        Database_return_t r = co_await tt();
        queries_amount--;
        _cv_stop.notify_all();
        co_return r;
    }

    void Database_impl::_background_execute_prepared_statement(Prepared_statement p,
                                                               std::unique_ptr<Prepared_statement_params> params) {
        {
            std::unique_lock lk (_bg_thread_stmt_mutex);
            _bg_stmt_queue.emplace(p,std::move(params));
            queries_amount++;
        }

        _cv_bg_stmt.notify_all();
    }


    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Database_impl>());
    }

    void Mysql_connection::connect()
    {
        _prepared_statements.clear();
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
        auto temp = _db->get_prepared_statements();
        for (auto& [k,v] : temp){
            _prepare_statement(k);
        }
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
                                   std::string(mysql_error(_conn)) + " query: "+ query);
                    throw std::runtime_error("Database ERROR: Failed to execute query after reconnecting: " +
                                             std::string(mysql_error(_conn)) + " query: "+ query);
                }
            } else {
                _log->critical("Database ERROR: Failed to execute query: " + std::string(mysql_error(_conn)) + " query: "+ query);
                throw std::runtime_error("Database ERROR: Failed to execute query: " + std::string(mysql_error(_conn)) + " query: "+ query);
            }
        }
        return  mysql_use_result(_conn);
    }

    void Mysql_connection::_prepare_statement(const Prepared_statement &st) {
        std::string statement = _db->get_prepared_statement(st);
        MYSQL_STMT* stm = mysql_stmt_init(_conn);
        if (stm == nullptr){
            connect();
            return;
        }
        if (mysql_stmt_prepare(stm, statement.c_str(), statement.size()) != 0){
            std::string error_message = mysql_stmt_error(stm);
            _log->critical("Database ERROR: Failed to create prepared statement: " + statement + ". MySQL error: " + error_message);
            throw std::runtime_error("Database ERROR: Failed to create prepared statement: " + statement + ". MySQL error: " + error_message);
        }
        _prepared_statements[st] = stm;
    }

    void Mysql_connection::prepare_statement(const Prepared_statement &st) {
        std::unique_lock lk(_mutex);
        _prepare_statement(st);
    }

    void Mysql_connection::remove_statement(const Prepared_statement &st) {
        std::unique_lock lk (_mutex);
        MYSQL_STMT* stm = _prepared_statements.at(st);
        mysql_stmt_close(stm);
        _prepared_statements.erase(st);
    }

   Database_return_t Mysql_connection::execute_prepared_statement(const Prepared_statement &st, std::unique_ptr<Prepared_statement_params> params) {
    int max_retries = 2;
    int attempts = 0;

    while (attempts < max_retries) {
        try {
            if (_conn == nullptr) {
                connect();
            }
            auto args = dynamic_cast<Mysql_prepared_statement_params*>(params.get())->get_binds();

            if (!_prepared_statements.contains(st)) {
                _log->critical("Database Error: Unknown prepared statement");
                throw std::runtime_error("Database Error: Unknown prepared statement");
            }
            auto* stmt = _prepared_statements[st];
            if (!args.empty()) {
                if (mysql_stmt_bind_param(stmt, &args[0])) {
                    _log->critical("Database Error: mysql_stmt_bind_param failed");
                    throw std::runtime_error("Database Error: mysql_stmt_bind_param failed");
                }
            }

            if (mysql_stmt_execute(stmt)) {
                _log->critical("Database Error: mysql_stmt_execute(), failed. Error: " + std::string(mysql_stmt_error(stmt)));
                throw std::runtime_error("Database Error: mysql_stmt_execute(), failed. Error: " + std::string(mysql_stmt_error(stmt)));
            }

            Database_return_t storage{};

            do {
                MYSQL_RES* confres = mysql_stmt_result_metadata(stmt);
                if (!confres) {
                    continue;
                }

                unsigned long num_fields = mysql_stmt_field_count(stmt);
                MYSQL_FIELD* fields = mysql_fetch_fields(confres);
                auto bindings = std::make_unique<MYSQL_BIND[]>(num_fields);
                auto string_buffers = std::make_unique<std::unique_ptr<char[]>[]>(num_fields);
                auto lengths = std::make_unique<unsigned long[]>(num_fields);
                auto is_null = std::make_unique<bool[]>(num_fields);

                std::memset(bindings.get(), 0, sizeof(MYSQL_BIND) * num_fields);
                std::memset(is_null.get(), 0, sizeof(bool) * num_fields);
                std::memset(lengths.get(), 0, sizeof(unsigned long) * num_fields);

                for (size_t i = 0; i < num_fields; i++) {
                    fields[i].length = std::min(fields[i].length, 2048UL);
                    string_buffers[i] = std::make_unique<char[]>(fields[i].length);
                    std::memset(string_buffers[i].get(), 0, fields[i].length);
                    bindings[i].buffer_type = MYSQL_TYPE_VAR_STRING;
                    bindings[i].buffer = string_buffers[i].get();
                    bindings[i].buffer_length = fields[i].length;
                    bindings[i].is_null = &is_null[i];
                    bindings[i].length = &lengths[i];
                }

                int result = mysql_stmt_bind_result(stmt, bindings.get());
                if (result) {
                    _log->critical("Database ERROR: mysql_stmt_bind_result failed: " + std::string(mysql_stmt_error(stmt)));
                    throw std::runtime_error("Database ERROR: mysql_stmt_bind_result failed: " + std::string(mysql_stmt_error(stmt)));
                }

                while (true) {
                    int fetch_result = mysql_stmt_fetch(stmt);
                    if (fetch_result == MYSQL_NO_DATA) {
                        break;
                    } else if (fetch_result != 0) {
                        _log->critical("Database ERROR: " + std::string(mysql_stmt_error(stmt)));
                        throw std::runtime_error("Database ERROR: " + std::string(mysql_stmt_error(stmt)));
                    }

                    if (mysql_num_fields(confres)) {
                        long s_field_count = 0;
                        std::map<std::string, std::string> row;
                        while (s_field_count < mysql_num_fields(confres)) {
                            std::string a = (fields[s_field_count].name ? fields[s_field_count].name : "");
                            std::string b = is_null[s_field_count] ? "" : std::string(string_buffers[s_field_count].get(), lengths[s_field_count]);
                            row[a] = b;
                            s_field_count++;
                        }
                        storage.push_back(row);
                    }
                }

                mysql_free_result(confres);

                // Proceed to the next result set if available
            } while (mysql_stmt_next_result(stmt) == 0);

            return storage;

        } catch (const std::runtime_error& e) {
            // Check if the error is due to connection lost
            if (std::string(e.what()).find("Lost connection") != std::string::npos) {
                _log->warn("Connection lost, attempting to reconnect...");
                connect(); // Attempt to reconnect
                attempts++;
            } else {
                throw; // Rethrow the exception if it's not related to connection lost
            }
        }
    }
    throw std::runtime_error("Database Error: Max retries reached. Unable to execute statement.");
}


    void Mysql_prepared_statement_params::add_param(const std::string &param) {
        MYSQL_BIND bind{};
        _string_data.push_back(param); // Store the string in the container
        auto& stored_string = _string_data.back();
        bind.buffer_type = MYSQL_TYPE_VAR_STRING;
        bind.buffer =  const_cast<char*>(stored_string.c_str());
        bind.is_null = 0;
        bind.is_null_value = false;
        bind.buffer_length = stored_string.length();
        _binds.push_back(bind);
        //std::cout<<std::string((char*)_binds.back().buffer,(_binds.back().buffer_length)) << std::endl;
    }

    std::vector<MYSQL_BIND> Mysql_prepared_statement_params::get_binds() {
        return _binds;
    }
} // gb