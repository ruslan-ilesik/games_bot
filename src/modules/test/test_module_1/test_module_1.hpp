//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"

namespace gb {

    class Test_module_1: public Module {
    public:
        Test_module_1();;
        virtual ~Test_module_1() = default;

        virtual void run(const Modules& modules);

    };

    extern "C" Module_ptr create();
} // gb


