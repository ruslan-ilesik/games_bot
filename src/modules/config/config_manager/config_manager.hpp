//
// Created by ilesik on 7/4/24.
//

#pragma once
#include <shared_mutex>
#include "../config.hpp"

namespace gb {

    class Config_manager: public Config {

        std::vector<Config*> _configs;

        std::shared_mutex _mutex;

    public:
        Config_manager();

        virtual ~Config_manager() = default;

        virtual void innit(const Modules& modules);
        virtual void stop();

        virtual void run();

        virtual std::string get_value(const std::string& name);

        virtual std::string get_value_or(const std::string& name, const std::string& default_return);

        virtual void add_config(Config* config);

        virtual void remove_config(Config* config);

    };

    typedef std::shared_ptr<Config_manager> Config_manager_ptr;


    extern "C" Module_ptr create();
} // gb


