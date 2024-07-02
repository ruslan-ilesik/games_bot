//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_2.hpp"

namespace gb {

    gb::Module_ptr create(){
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_1>());
    };

    Test_module_1::Test_module_1() : Module("test_module_2",{}){
    }

    void Test_module_1::run(const Modules &modules) {
        std::cout << "run module 2\n";
    }
} // gb