//
// Created by ilesik on 7/5/24.
//

#include <cstdlib>
#include <iostream>

#include "env_config.hpp"


namespace gb {

    Env_config::Env_config() : Config("env_config",{"config"}){}

    void Env_config::innit(const Modules &modules) {
        this->config_manager = std::static_pointer_cast<Config_manager>(modules.at("config"));
        config_manager->add_config(this);
    }

    void Env_config::stop() {
        config_manager->remove_config(this);
    }

    std::string Env_config::get_value(const std::string &name) {
        const char* env_var_value = std::getenv(name.c_str());
        if (env_var_value == nullptr){
            throw Config_value_not_found_exception("Value "+ name + " was not found in environment variables");
        }
        return {env_var_value};
    }

    std::string Env_config::get_value_or(const std::string &name, const std::string &default_return) {
        const char* env_var_value = std::getenv(name.c_str());
        if (env_var_value == nullptr){
            return default_return;
        }
        return {env_var_value};
    }

    void Env_config::run() {}
} // gb