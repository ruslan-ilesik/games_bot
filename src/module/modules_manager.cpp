//
// Created by ilesik on 6/30/24.
//

#include "modules_manager.hpp"

#include <ranges>

namespace gb {
    Modules_manager::Modules_manager(const std::filesystem::path &modules_path) :
            Module("modules_manager", {}),
            _watch(modules_path.string(),
                   [this](const std::string &path, const filewatch::Event event) {
                       std::scoped_lock<std::shared_mutex> m(this->_mutex);
                       std::cout << "Modules manager: " << path << ' ' << filewatch::event_to_string(event) << '\n';
                   }) {

        this->_modules_path = modules_path;
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
        void *thing = (dlsym(libraryHandle, "create"));
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

        _modules.insert({module->get_name(), {libraryHandle, module, {}}});
        std::cout << "Modules_manager module: " << module->get_name() << " loaded successfully\n";
    }

    Modules_manager_ptr Modules_manager::create(const std::filesystem::path &modules_path) {
        return std::shared_ptr<Modules_manager>(new Modules_manager(modules_path));
    }

    void Modules_manager::run_modules() {
        std::cout << "Modules_manager start modules run\n";
        size_t iteration = 1;
        auto not_running = std::ranges::count_if(std::views::values(_modules),[](Internal_module& v){return !v.is_running();});
        while ( not_running > 0){
            bool was_new_loaded = false;
            std::cout << "Modules_manager module run iteration "<< iteration << "\n";
            for (auto& m : std::views::values(_modules)){
                if (!m.is_running() && m.has_sufficient_dependencies(_modules)){
                    std::cout << "Modules_manager running module " << m.get_module()->get_name() << '\n';
                    m.run(_modules);
                    was_new_loaded = true;
                    std::cout << "Modules_manager module " << m.get_module()->get_name() << " is running\n";
                }
            }
            if (!was_new_loaded){

                auto join = [](const std::vector<std::string>& strings, std::string const& separator)
                {
                    std::ostringstream result;
                    auto begin = strings.begin();
                    if (begin != strings.end())
                        result << *begin++;
                    while (begin != strings.end())
                        result << separator << *begin++;
                    return result.str();
                };
                std::string msg;
                for (auto& m : std::views::values(_modules)){
                    if (!m.is_running()){
                        msg += std::format("\n{} : dependencies: {}",m.get_module()->get_name(),join(m.get_module()->get_dependencies(),", "));
                    }
                }
                throw std::runtime_error("There is not existing dependency or circular dependencies in"+ msg);
            }

            not_running = std::ranges::count_if(std::views::values(_modules),[](Internal_module& v){return !v.is_running();});
            iteration++;
        }


    }

    Module_ptr Modules_manager::getptr() {
        return shared_from_this();
    }

    void Modules_manager::run() {
        _modules.insert({"module_manager",{nullptr,getptr()}});
        std::cout << "Modules_manager running initial modules loading...\n";
        for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(_modules_path)) {
            if (dirEntry.path().extension() != ".so") { continue; }
            this->load_module(dirEntry.path());
        }
        std::string output = "";
        size_t cnt = 0;
        for (auto &[k, v]: _modules) {
            cnt++;
            output += std::format("\n {}) {}", cnt, k);
        }
        std::cout << "Modules_manager initial modules loading done.\nLoaded modules: " << output << '\n';
        std::cout << "Modules_manager start modules initial run\n";
        run_modules();
    }

    void Modules_manager::run(const Modules &modules) {}

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

    void Modules_manager::Internal_module::run(const Internal_modules &modules) {
        if (_is_running) {
            return;
        }
        Modules deps = {{"module_manager",modules.at("module_manager")._module}};
        for (auto& i: _module->get_dependencies()){
            deps.insert({i,modules.at(i)._module});
        }
        _module->run(deps);
        _is_running = true;

    }

    bool Modules_manager::Internal_module::is_running() const { return _is_running; }

    bool Modules_manager::Internal_module::has_sufficient_dependencies(const Internal_modules &modules) {
        for (auto &i: _module->get_dependencies()) {
            if (!modules.contains(i) || !modules.at(i).is_running()) {
                return false;
            }
        }
        return true;
    }

}// gb