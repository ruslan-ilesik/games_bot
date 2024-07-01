//
// Created by ilesik on 6/30/24.
//

#include "main.hpp"
#include "./module/modules_manager.hpp"
#include <iostream>
#include <format>
#include <string>

int main() {
    auto m = gb::Modules_manager::create({"./modules"});
    m->run();
    while(true){
        sleep(10);
    }
    return 0;
}
