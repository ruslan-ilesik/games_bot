//
// Created by ilesik on 6/30/24.
//

#include "main.hpp"
#include "./module/modules_manager.hpp"
#include <iostream>
#include <format>
#include <string>



#define _TEST_DER__FILE__ (__FILE__"123")
int main() {
    auto m = gb::Modules_manager::create({"./modules"});
    m->run();
    while(true){
        sleep(10);
        //m->stop_module("admin_terminal");
        //m->stop_modules();
    }
    return 0;
}
