//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"

namespace gb {

    class Test_module_2: public Module {
    public:
        Test_module_2();;
        virtual ~Test_module_2() = default;

        virtual void innit(const Modules& modules);

        virtual void stop();

        virtual void run();
    };

    extern "C" Module_ptr create();
} // gb


