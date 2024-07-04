//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_1.hpp"
#include "../../logging/logging.hpp"
namespace gb {

    Module_ptr create(){
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_1>());
    };

    Test_module_1::Test_module_1() : Module("test_module_1",{"test_module_2","logging"}){
    }
    void Test_module_1::run(const Modules &modules) {
        auto log = std::static_pointer_cast<Logging>(modules.at("logging"));
        log->warn("WORKS!");
    }


    void Test_module_1::stop() {
        std::cout << "stop module 1\n";
    }
} // gb