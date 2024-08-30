//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"
#include "src/modules//database/database.hpp"
#include "src/modules/logging/logging.hpp"

namespace gb {

    class Test_module_2: public Module {
        Database_ptr db;
        Logging_ptr log;
    public:
        Test_module_2();;
        virtual ~Test_module_2() = default;

        virtual void init(const Modules& modules);

        virtual void stop();

        virtual void run();
    };

    extern "C" Module_ptr create();
} // gb


