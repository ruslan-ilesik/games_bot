//
// Created by ilesik on 6/30/24.
//

#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
namespace gb {

    class Module;
    typedef std::shared_ptr<Module> Module_ptr;
    typedef std::map<std::string,Module_ptr> Modules;
    typedef Module_ptr (*create_func_t)();


    class Not_implemented_exception : public std::logic_error
    {
    public:
        explicit Not_implemented_exception(const std::string& error = "Method is not implemented");
    };

    class Module {
    protected:
        std::string _name;
        std::vector<std::string> _dependencies;

        explicit Module(const std::string& name, const  std::vector<std::string>& _dependencies = {});
    public:

        virtual ~Module() = default;

        std::string get_name();

        std::vector<std::string> get_dependencies();

        //Note that module manager is always passed here;
        virtual void run(const Modules& modules);

        virtual void stop();


    };

    extern "C" Module_ptr create();
}


