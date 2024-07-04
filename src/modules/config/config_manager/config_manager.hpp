//
// Created by ilesik on 7/4/24.
//

#pragma once
#include <shared_mutex>
#include "../config.hpp"

namespace gb {

    class Config_manager: public Config {

        std::vector<Config_ptr> _configs;

        std::shared_mutex _mutex;

    public:
        Config_manager();

        virtual ~Config_manager() = default;

        virtual void run(const Modules& modules);
        virtual void stop();

        virtual std::string get_value(const std::string& name);

        virtual void add_config(const Config_ptr& config);

        virtual void remove_config(const Config_ptr& config);

    };


    extern "C" Module_ptr create();
} // gb


