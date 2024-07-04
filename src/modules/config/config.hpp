//
// Created by ilesik on 7/4/24.
//

#pragma once
#include "../../module/module.hpp"
namespace gb{

    class Config_value_not_found_exception : public std::runtime_error
    {
    public:

        explicit Config_value_not_found_exception(const std::string& error = "value not found"): std::runtime_error(error){};
    };

    class Config: public Module{
    protected:
        Config(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};
    public:
        virtual ~Config() = default;

        virtual std::string get_value(const std::string& name) = 0;

    };

    typedef std::shared_ptr<Config> Config_ptr;

    extern "C" Module_ptr create();
} //gb
