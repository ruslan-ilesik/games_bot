//
// Created by ilesik on 6/30/24.
//

#include "modules_manager.hpp"

namespace gb {
    Modules_manager::Modules_manager(const std::filesystem::path &modules_path) : Module("modules_manager",{}),
            _watch(modules_path.string(),
                   [this](const std::string &path, const filewatch::Event event) {
                       std::scoped_lock<std::shared_mutex> m(this->_mutex);
                       std::cout << "Modules manager: " << path << ' ' << filewatch::event_to_string(event) << '\n';
                   }) {
        std::cout << "Modules_manager starting watching directory: " << modules_path << '\n';
    }

    void Modules_manager::load_module(const std::string &path) {
        std::cout << "Modules_manager loading module: " << path << '\n';
        void *libraryHandle = dlopen((path).c_str(), RTLD_LAZY);
        if (!libraryHandle) {
            std::cout << "Modules_manager ERROR loading module: " << path << ' ' << dlerror() << '\n';
            dlclose(libraryHandle);
            return;
        }
        void *thing = (dlsym(libraryHandle, "innit"));
        if (!thing) {
            std::cout << "Modules_manager ERROR loading getting module symbol: " << path << ' ' << dlerror() << '\n';
            dlclose(libraryHandle);
            return;
        }
        Module_ptr module = reinterpret_cast<create_func_t>(thing)();

        if (_modules.contains(module->get_name())) {
            std::cout << "Modules_manager ERROR module: " << module->get_name() << " is already loaded" << '\n';
            module.reset();
            dlclose(libraryHandle);
            return;
        }

        _modules.insert({module->get_name(),{libraryHandle,module,{}}});
    }

    std::shared_ptr<Modules_manager> Modules_manager::create(const std::filesystem::path &modules_path) {
        return std::shared_ptr<Modules_manager>(new Modules_manager(modules_path));
    }

    void *Modules_manager::Internal_module::get_library_handler() {
        return _library_handler;
    }

    Module_ptr Modules_manager::Internal_module::get_module() {
        return _module;
    }

    std::vector<std::string> Modules_manager::Internal_module::get_module_dependent() {
        return _module_dependent;
    }

    void Modules_manager::Internal_module::add_module_dependent(const std::string &name) {
        this->_module_dependent.push_back(name);
    }

    void Modules_manager::Internal_module::remove_module_dependent(const std::string &name) {
        auto e = std::ranges::remove(_module_dependent, name);
        _module_dependent.erase(e.begin(), e.end());
    }

    Modules_manager::Internal_module::Internal_module(void *library_handler, const Module_ptr &module,
                                                      const std::vector<std::string> &module_dependent) {
        this->_library_handler = library_handler;
        this->_module = module;
        this->_module_dependent = module_dependent;
    }

    void Modules_manager::Internal_module::run(const Modules &modules) {
        if (is_run){
            return;
        }
        is_run = true;
    }

}// gb