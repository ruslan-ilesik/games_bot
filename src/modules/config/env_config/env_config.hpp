//
// Created by ilesik on 7/5/24.
//

#pragma once
#include "../config.hpp"
#include "../config_manager/config_manager.hpp"

namespace gb {

    class Env_config: public Config{
    private:
        Config_manager_ptr config_manager;
    public:
        Env_config();

        virtual void innit(const Modules& modules);

        virtual void stop();

        virtual void run();

        virtual std::string get_value(const std::string& name);

        virtual std::string get_value_or(const std::string& name, const std::string& default_return);
    };

    extern "C" Module_ptr create(){
        return std::dynamic_pointer_cast<Module>(std::make_shared<Env_config>());
    }
} // gb


