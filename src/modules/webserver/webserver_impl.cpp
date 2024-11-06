//
// Created by ilesik on 10/27/24.
//

#include "webserver_impl.hpp"

#include "api/commands_page.hpp"
#include "api/index_page.hpp"


namespace gb {
    Webserver_impl::Webserver_impl() : Webserver("webserver", {"discord_statistics_collector", "database", "config","discord_command_handler"}) {}
    void Webserver_impl::stop() {
        drogon::app().quit();
        for (auto &i: on_stop) {
            i();
        }
        _drogon_thread.join();
    }
    void Webserver_impl::run() {
        index_page_api(this);
        commands_page_api(this);
        _drogon_thread = std::thread(
            [this]() { drogon::app().addListener("0.0.0.0", std::stoi(config->get_value("webserver_port"))).run(); });
    }
    void Webserver_impl::init(const Modules &modules) {
        discord_stats =
            std::static_pointer_cast<Discord_statistics_collector>(modules.at("discord_statistics_collector"));
        db = std::static_pointer_cast<Database>(modules.at("database"));
        config = std::static_pointer_cast<Config>(modules.at("config"));
        commands_handler = std::static_pointer_cast<Discord_command_handler>(modules.at("discord_command_handler"));
    }

    Module_ptr create() { return std::dynamic_pointer_cast<Module>(std::make_shared<Webserver_impl>()); }
} // namespace gb
