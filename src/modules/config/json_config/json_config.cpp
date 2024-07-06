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
        this->config_manager = std::static_pointer_cast<Config_manager>(modules.at("config"));
        config_manager->add_config(this);
        std::ifstream ifs("./data/config.json");
        json = nlohmann::json::parse(ifs);
    }

    Json_config::Json_config() : Config("json_config",{"config"}){}

    void Json_config::run() {

    }

    void Json_config::stop() {
        config_manager->remove_config(this);
    }

    std::string Json_config::get_value(const std::string &name) {
        if (!json.contains(name)){
            throw  std::runtime_error("Value " + name + " not found in json config");
        }
        auto value = json.at(name);
        return to_string(value);
    }

    std::string Json_config::get_value_or(const std::string &name, const std::string &default_return) {
        try{
            return  this->get_value(name);
        }
        catch (std::runtime_error&){

        }
        return default_return;
    }
} // gb