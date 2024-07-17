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

    Test_module_2::Test_module_2() : Module("test_module_2",{"database","logging"}){
    }

    void Test_module_2::innit(const Modules &modules) {
        //std::cout << "run module 2\n";
        std::cout << _TEST_DER << '\n';
        this->db = std::static_pointer_cast<Database>(modules.at("database"));
        this->log = std::static_pointer_cast<Logging>(modules.at("logging"));
    }

    void Test_module_2::stop() {
        std::cout << "stop module 2\n";
    }

    void Test_module_2::run() {
        auto cc = [this](size_t i)-> Task<void>{
            db->background_execute(std::format("Select  {};",i));
            //log->info("THIS WORKS");
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));

            /*for (const auto &row : r) {
                for (const auto &col : row) {
                    std::cout << col.first << ": " << col.second << " | ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;*/
            co_return;
        };
        auto myid = std::this_thread::get_id();
        std::stringstream ss;
        ss << myid;
        std::string mystring = ss.str();

        log->warn("MAIN THREAD: "+mystring);
        std::vector<std::thread> threads;

        size_t requests_per_thread = 10;
        for (int i = 0; i < 10; ++i) {
            int start = i * requests_per_thread;
            threads.emplace_back([start,requests_per_thread,cc] {
                for (int i = start; i < start + requests_per_thread; ++i) {
                    cc(i);
                }
            });
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }

    }
} // gb