//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"
#include "../../logging/logging.hpp"
#include "../../config/config.hpp"
#define _TEST_DER (__FILE__ "123")
namespace gb {

    class Test_module_1: public Module {
    private:
        Logging_ptr  log;
        Config_ptr config;
    public:
        Test_module_1();;
        virtual ~Test_module_1() = default;

        virtual void innit(const Modules& modules);

        virtual void stop();

        virtual void run();

    };

    extern "C" Module_ptr create();
} // gb


