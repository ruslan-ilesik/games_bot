//
// Created by ilesik on 7/6/24.
//


#include <fstream>
#include "json_config.hpp"

namespace gb {

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Json_config>());
    }

    void Json_config::innit(const Modules &modules) {
        this->_config_manager = std::static_pointer_cast<Config_manager>(modules.at("config"));
        _config_manager->add_config(this);
        reload_from_file();
        this->_admin_terminal = std::static_pointer_cast<Admin_terminal>(modules.at("admin_terminal"));
        _admin_terminal->add_command({"json_config_reload",
                                      "Command to reload json config from file",
                                      "Arguments: no arguments",
                                      [this](const std::vector<std::string> &args) {
                                          try {
                                              reload_from_file();
                                              std::cout << "json config was reloaded" << std::endl;
                                          }
                                          catch (...) {
                                              std::cout << "json_config_reload command error: Was unable to reload file"
                                                        << std::endl;
                                          }
                                      }
                                     });

    }

    Json_config::Json_config() : Config("json_config", {"config", "admin_terminal"}) {}

    void Json_config::run() {

    }

    void Json_config::stop() {
        _config_manager->remove_config(this);
        _admin_terminal->remove_command("json_config_reload");
    }

    std::string Json_config::get_value(const std::string &name) {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return do_get_value(name);
    }

    std::string Json_config::get_value_or(const std::string &name, const std::string &default_return) {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        try {
            return this->do_get_value(name);
        }
        catch (std::runtime_error &) {

        }
        return default_return;
    }

    void Json_config::reload_from_file() {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        std::ifstream ifs("./data/config.json");
        _json = nlohmann::json::parse(ifs);
    }

    std::string Json_config::do_get_value(const std::string &name) {
        if (!_json.contains(name)) {
            throw std::runtime_error("Value " + name + " not found in json config");
        }
        auto value = _json.at(name);
        return to_string(value);
    }
} // gb