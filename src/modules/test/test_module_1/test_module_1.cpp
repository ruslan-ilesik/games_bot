//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_1.hpp"

namespace gb {

    gb::Module_ptr create(){
            std::cout << "loaded test module\n";
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_1>());
    };

    Test_module_1::Test_module_1() : Module("test_module_1",{}){
    }
} // gb