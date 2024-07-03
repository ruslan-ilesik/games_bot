//
// Created by ilesik on 6/30/24.
//

#include "main.hpp"
#include "./module/modules_manager.hpp"
#include <iostream>
#include <format>
#include <string>
#define _TEST_DER __FILE__"123"
int main() {
    std::cout << _TEST_DER << '\n';
    auto m = gb::Modules_manager::create({"./modules"});
    m->run();
    while(true){
        sleep(1);
        m->stop_modules();
    }
    return 0;
}
