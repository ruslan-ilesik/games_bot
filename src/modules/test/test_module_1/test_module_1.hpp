//
// Created by ilesik on 6/30/24.
//

#pragma once
#include "../../../module/module.hpp"
#define _TEST_DER (__FILE__ "123")
namespace gb {

    class Test_module_1: public Module {
    public:
        Test_module_1();;
        virtual ~Test_module_1() = default;

        virtual void run(const Modules& modules);

        virtual void stop();

    };

    extern "C" Module_ptr create();
} // gb


