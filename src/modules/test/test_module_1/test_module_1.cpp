//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_1.hpp"

namespace gb {

    gb::Module_ptr create(){
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_1>());
    };

    Test_module_1::Test_module_1() : Module("test_module_1",{"test_module_2"}){
    }

    void Test_module_1::run(const Modules &modules) {
        std::cout << "run module 1\n";
        //std::cout << _TEST_DER << '\n';
    }

    void Test_module_1::stop() {
        std::cout << "stop module 1\n";
    }
} // gb