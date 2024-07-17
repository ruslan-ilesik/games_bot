//
// Created by ilesik on 7/4/24.
//

#include <format>
#include <iostream>
#include "config_manager.hpp"

namespace gb {


    Config_manager::Config_manager() : Config("config",{}){}

    void Config_manager::stop() {}

    std::string Config_manager::get_value(const std::string &name) {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        for (const auto& config : _configs){
            try{
                return config->get_value(name);
            }
            catch (const std::runtime_error&){
                continue;
            }
        }
        throw std::runtime_error(std::format("Value {} not found in provided configs",name));
    }

    void Config_manager::add_config(Config* config) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        std::cout << "Config manager new config: " << config->get_name() << std::endl;
        this->_configs.push_back(config);
    }

    void Config_manager::remove_config(Config* config) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        std::cout << "Config manager remove config: " << config->get_name() << std::endl;
        auto e = std::ranges::remove(_configs,config);
        _configs.erase(e.begin(), e.end());
    }

    void Config_manager::innit(const Modules &modules) {

    }

    std::string Config_manager::get_value_or(const std::string &name, const std::string &default_return) {
        std::shared_lock<std::shared_mutex> lock (_mutex);
        try{
            return this->get_value(name);
        }
        catch (const  std::runtime_error&){
        }
        return default_return;
    }

    void Config_manager::run() {};


    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Config_manager>());
    }
} // gb