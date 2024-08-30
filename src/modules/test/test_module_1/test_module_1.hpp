//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"
#include "../../config/config.hpp"
#include "../../logging/logging.hpp"

#include <src/modules/achievements_processing/achievements_processing.hpp>
#define _TEST_DER (__FILE__ "123")
namespace gb {

    class Test_module_1: public Module {
    private:
        Logging_ptr  log;
        Config_ptr config;
        Achievements_processing_ptr achievements;
    public:
        Test_module_1();;
        virtual ~Test_module_1() = default;

        virtual void init(const Modules& modules);

        virtual void stop();

        virtual void run();

    };

    extern "C" Module_ptr create();
} // gb


