//
// Created by ilesik on 7/14/24.
//

#pragma once
#include <coroutine>

#include "../../module/module.hpp"
#include "src/utils/coro/coro.hpp"




namespace gb{

    typedef std::vector<std::map<std::string,std::string>> Database_return_t;

    class Database: public Module{
    public:
        Database(const std::string& name, const std::vector<std::string>& dep) : Module(name, dep) {};

        virtual Task<Database_return_t> execute(const std::string& sql) = 0;
    };


    typedef std::shared_ptr<Database> Database_ptr;
} //gb
