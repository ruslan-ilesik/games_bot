//
// Created by ilesik on 6/30/24.
//

#pragma once

#include <filesystem>
#include <shared_mutex>

#include <dlfcn.h>

#include "./module.hpp"
#include "../utils/filewatch/FileWatch.hpp"

namespace gb {
    //Note that module manager is always passed to modules;
    class Modules_manager : public Module, public std::enable_shared_from_this<Module> {
    private:

        class Internal_module {
        private:
            void *_library_handler;
            Module_ptr _module;
            std::vector<std::string> _module_dependent;
            bool is_run = false;
        public:
            Internal_module(void *library_handler, const Module_ptr &module,
                            const std::vector<std::string> &module_dependent = {});

            void *get_library_handler();

            Module_ptr get_module();

            std::vector<std::string> get_module_dependent();

            void add_module_dependent(const std::string &name);

            void remove_module_dependent(const std::string &name);

            void run(const Modules& modules);
        };

        std::shared_mutex _mutex;
        filewatch::FileWatch<std::string> _watch;
        std::map<std::string, Internal_module> _modules;

        explicit Modules_manager(const std::filesystem::path &modules_path);

        void load_module(const std::string &name);


    public:
        static std::shared_ptr<Modules_manager> create(const std::filesystem::path &modules_path = {"./"});

    };
}

