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
    const char* env_var_value = getenv("PATH");
    std::cout << "AAAAAAAA: "<< env_var_value << '\n';
    m->run();
    while(true){
        sleep(1);
        //m->stop_modules();
    }
    return 0;
}
