//
// Created by ilesik on 10/27/24.
//

#include "webserver_impl.hpp"

#include "api/commands_page.hpp"
#include "api/dashboard_page.hpp"
#include "api/discord_login.hpp"
#include "api/index_page.hpp"
#include "api/patreon.hpp"
#include "api/premium.hpp"


namespace gb {
    Webserver_impl::Webserver_impl() :
        Webserver("webserver", {"discord_statistics_collector", "database", "config", "discord_command_handler",
                                "logging", "premium_manager", "discord_achievements_processing"}) {}


    void Webserver_impl::stop() {
        {
            std::unique_lock lk(_mutex);
            _stop = true;
            _cv.notify_all();
        }
        drogon::app().quit();
        for (auto &i: on_stop) {
            i();
        }
        _drogon_thread.join();
        _background_worker.join();
        db->remove_prepared_statement(_delete_cookie_stmt);
        db->remove_prepared_statement(_cookie_exists_stmt);
    }
    void Webserver_impl::run() {
        _delete_cookie_stmt = db->create_prepared_statement("delete from website_cookies where `id` = ?;");
        _cookie_exists_stmt = db->create_prepared_statement("select * from website_cookies where `id` = ?;");
        Database_return_t r = sync_wait(db->execute("SELECT MAX(id)AS max_id FROM website_cookies"));
        if (r.empty()) {
            current_jwt_id = 0;
        } else {
            uint64_t id;
            std::istringstream iss(r.at(0).at("max_id"));
            iss >> id;
            current_jwt_id = id + 1;
        }
        _background_worker = std::thread{[this]() {
            Prepared_statement remove_old_jwt =
                db->create_prepared_statement("delete from website_cookies where expire_time < UTC_TIMESTAMP();");
            while (1) {
                std::unique_lock lk(_mutex);
                // Wait for either _stop == true or a 24 hours timeout
                bool result = _cv.wait_for(lk, std::chrono::days(1), [this]() { return _stop; });

                if (result) {
                    break;
                }
                sync_wait(db->execute_prepared_statement(remove_old_jwt));
            }
            db->remove_prepared_statement(remove_old_jwt);
            return;
        }};

        index_page_api(this);
        commands_page_api(this);
        discord_login_api(this);
        patreon_api(this);
        premium_api(this);
        dashboard_page_api(this);

        drogon::app().setDocumentRoot("./website");

        drogon::app().registerHandlerViaRegex(
            ".*", // Match any path using a proper regex
            [this](const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
                std::string path = req->path();

                // Handle paths without extensions (e.g., /commands -> /commands.html)
                if (path.find('.') == std::string::npos && path != "/") {
                    path += ".html";
                } else if (path == "/") {
                    path = "/index.html";
                }

                // Construct the file path relative to the website directory
                auto filePath = "./website" + path;

                if (std::filesystem::exists(filePath)) {
                    // Serve the file if it exists
                    auto resp = drogon::HttpResponse::newFileResponse(filePath);
                    callback(resp);
                } else {
                    // File not found, return 404
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k404NotFound);
                    resp->setBody("404 Not Found");
                    callback(resp);
                }
            });

        _drogon_thread = std::thread(
            [this]() { drogon::app().addListener("0.0.0.0", std::stoi(config->get_value("webserver_port"))).run(); });
    }
    void Webserver_impl::init(const Modules &modules) {
        discord_stats =
            std::static_pointer_cast<Discord_statistics_collector>(modules.at("discord_statistics_collector"));
        db = std::static_pointer_cast<Database>(modules.at("database"));
        config = std::static_pointer_cast<Config>(modules.at("config"));
        commands_handler = std::static_pointer_cast<Discord_command_handler>(modules.at("discord_command_handler"));
        log = std::static_pointer_cast<Logging>(modules.at("logging"));
        premium_manager = std::static_pointer_cast<Premium_manager>(modules.at("premium_manager"));
        achievements_manager =
            std::static_pointer_cast<Discord_achievements_processing>(modules.at("discord_achievements_processing"));
    }

    Task<void> Webserver_impl::delete_cookie(uint64_t id) {
        co_await db->execute_prepared_statement(_delete_cookie_stmt, id);
        co_return;
    }

    Task<bool> Webserver_impl::check_if_cookie_exists(uint64_t id) {
        Database_return_t r = co_await db->execute_prepared_statement(_cookie_exists_stmt, id);
        co_return !r.empty();
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Webserver_impl>()); }
} // namespace gb
