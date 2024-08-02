//
// Created by ilesik on 6/30/24.
//

#include <iostream>

#include "test_module_1.hpp"

namespace gb {

    Module_ptr create(){
            return std::dynamic_pointer_cast<Module>(std::make_shared<Test_module_1>());
    };

    Test_module_1::Test_module_1() : Module("test_module_1",{"test_module_2","logging","config","image_processing"}){
    }
    void Test_module_1::innit(const Modules &modules) {
        log = std::static_pointer_cast<Logging>(modules.at("logging"));
        config = std::static_pointer_cast<Config>(modules.at("config"));
    }


    void Test_module_1::stop() {
        std::cout << "stop module 1\n";
    }

    void Test_module_1::run() {
        log->warn(config->get_value_or("AAAAAAAAAAAAAAAAAAAAA","b"));
        log->warn(config->get_value("PATH"));
        log->warn(config->get_value("test"));
    }
} // gb