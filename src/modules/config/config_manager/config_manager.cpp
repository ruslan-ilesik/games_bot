//
// Created by ilesik on 7/4/24.
//

#include <format>

#include "config_manager.hpp"

namespace gb {


    Config_manager::Config_manager() : Config("config_manager",{}){}

    void Config_manager::stop() {}

    std::string Config_manager::get_value(const std::string &name) {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        for (const auto& config : _configs){
            try{
                return config->get_value(name);
            }
            catch (const Config_value_not_found_exception&){

            }
        }
        throw Config_value_not_found_exception(std::format("Value {} not found in provided configs",name));
    }

    void Config_manager::add_config(const Config_ptr &config) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        this->_configs.push_back(config);
    }

    void Config_manager::remove_config(const Config_ptr &config) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        auto e = std::ranges::remove(_configs, config);
        _configs.erase(e.begin(), e.end());
    }

    void Config_manager::run(const Modules &modules) {

    };


    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Config_manager>());
    }
} // gb