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
    class Modules_manager;

    typedef std::shared_ptr<Modules_manager> Modules_manager_ptr;

    class Modules_manager : public Module, public std::enable_shared_from_this<Module> {
    private:
        class Internal_module;

        typedef std::map<std::string, Internal_module> Internal_modules;

        class Internal_module {
        private:
            void *_library_handler;
            Module_ptr _module;
            std::vector<std::string> _module_dependent;
            bool _is_running = false;
        public:
            Internal_module(void *library_handler, const Module_ptr &module,
                            const std::vector<std::string> &module_dependent = {});

            void *get_library_handler() const;

            Module_ptr get_module() const;

            std::vector<std::string> get_module_dependent() const;

            void add_module_dependent(const std::string &name);

            void remove_module_dependent(const std::string &name);

            bool is_running() const;

            bool has_sufficient_dependencies(const Internal_modules &modules);

            void run(const Internal_modules &modules);

            void stop();
        };

        std::shared_mutex _mutex;
        filewatch::FileWatch<std::string> _watch;
        std::map<std::string, Internal_module> _modules;
        std::filesystem::path _modules_path;
        bool _allow_modules_load = true;

        Module_ptr getptr();

        void do_stop_module(const std::string &name);

        void do_run_module(const std::string &name);

        void do_load_module(const std::string &name);

    protected:
        explicit Modules_manager(const std::filesystem::path &modules_path);

    public:
        static Modules_manager_ptr create(const std::filesystem::path &modules_path = {"./"});

        //do not use yourself, only for internal use
        virtual void run(const Modules &modules);

        void load_module(const std::string &name);

        //RUNS ALL THE MODULES;
        void run_modules();

        void run_module(const std::string &name);

        // STOPS ALL THE MODULES
        void stop_modules();

        void stop_module(const std::string &name);

        void run();

        void set_allow_modules_load(bool v);

        bool is_allowed_modules_load();

    };
}

