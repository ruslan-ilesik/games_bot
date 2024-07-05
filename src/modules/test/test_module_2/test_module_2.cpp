//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_2.hpp"
#include "../test_module_1/test_module_1.hpp"

namespace gb {

    gb::Module_ptr create(){
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_2>());
    };

    Test_module_2::Test_module_2() : Module("test_module_2",{}){
    }

    void Test_module_2::innit(const Modules &modules) {
        //std::cout << "run module 2\n";
        std::cout << _TEST_DER << '\n';
    }

    void Test_module_2::stop() {
        std::cout << "stop module 2\n";
    }

    void Test_module_2::run() {}
} // gb