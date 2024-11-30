//
// Created by ilesik on 6/30/24.
//

#include "modules_manager.hpp"

#include <ranges>

namespace gb {
    Modules_manager::Modules_manager(const std::filesystem::path &modules_path) :
        Module("module_manager", {})
#if !defined(__FreeBSD__)
        , _watch(modules_path.string(),
               [=,this](const std::string &path, const filewatch::Event event) {
                   std::unique_lock<std::shared_mutex> lock(this->_mutex);
                   std::cout << "Modules manager: " << path << ' ' << filewatch::event_to_string(event) << '\n';
                   std::string real_path = this->_modules_path.string() + "/" + path;
                   std::vector<std::string> keys;
                   for (const auto &pair: _modules) {
                       if (pair.second.get_module().get() != this) {
                           keys.push_back(pair.first);
                       }
                   }
                   for (auto &k: keys) {
                       auto &v = _modules.at(k);
                       if (v.get_file_path() == real_path) {
                           if (event == filewatch::Event::removed) {
                               auto t = v.get_module()->get_name();
                               do_stop_module(t);
                               return;
                           } else if (event == filewatch::Event::modified) {
                               auto t = v.get_module()->get_name();
                               do_stop_module(t);
                               t = do_load_module(real_path);
                               do_init_module(t);
                               do_run_module(t);
                               return;
                           }
                       }
                   }
                   if ((real_path.ends_with(".so")) &&
                       (event == filewatch::Event::added || (event == filewatch::Event::modified))) {
                       auto t = do_load_module(real_path);
                       do_init_module(t);
                       do_run_module(t);
                   }
               })
#endif
    {
        this->_modules_path = modules_path;
#if defined(__FreeBSD__)
        _monitor_thread = std::thread([=,this]() {
            bool fst = true;
            while (_running) {
                std::this_thread::sleep_for(_scan_interval);

                std::lock_guard<std::mutex> lock(_file_states_mutex);
                for (const auto &entry : std::filesystem::directory_iterator(_modules_path)) {
                    try {
                        auto current_time = std::filesystem::last_write_time(entry);
                        const auto &path = entry.path().string();

                        if (_file_states.find(path) == _file_states.end()) {
                            // File added
                            _file_states[path] = current_time;
                            if (path.ends_with(".so") && !fst) {
                                auto module_name = do_load_module(path);
                                do_init_module(module_name);
                                do_run_module(module_name);
                            }
                        } else if (_file_states[path] != current_time && !fst) {
                            // File modified
                            _file_states[path] = current_time;
                            auto module_name = do_load_module(path);
                            do_stop_module(module_name);
                            do_init_module(module_name);
                            do_run_module(module_name);
                        }
                    } catch (const std::filesystem::filesystem_error &e) {
                        std::cerr << "Filesystem error: " << e.what() << std::endl;
                    }
                }

                // Check for removed files
                for (auto it = _file_states.begin(); it != _file_states.end();) {
                    if (!std::filesystem::exists(it->first)) {
                        auto module_name = it->first;
                        do_stop_module(module_name);
                        it = _file_states.erase(it);
                    } else {
                        ++it;
                    }
                }
                fst = false;
            }
        });
#endif
        std::cout << "Modules_manager starting watching directory: " << modules_path << '\n';
    }

    std::string Modules_manager::load_module(const std::string &path) {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        return this->do_load_module(path);
    }

    std::string Modules_manager::do_load_module(const std::string &path) {

        if (!_allow_modules_load) {
            std::cout << "Modules_manager loading modules is disabled!!!\n";
            return "";
        }
        std::cout << "Modules_manager loading module: " << path << '\n';
        void *libraryHandle = dlopen((path).c_str(), RTLD_NOW);
        if (!libraryHandle) {
            std::cout << "Modules_manager ERROR loading module: " << path << ' ' << dlerror() << std::endl;
            dlclose(libraryHandle);
            return "";
        }
        void *thing = (dlsym(libraryHandle, "create"));
        if (!thing) {
            std::cout << "Modules_manager ERROR loading getting module symbol: " << path << ' ' << dlerror()
                      << std::endl;
            dlclose(libraryHandle);
            return "";
        }
        Module_ptr module = reinterpret_cast<create_func_t>(thing)();

        if (_modules.contains(module->get_name())) {
            std::cout << "Modules_manager ERROR module: " << module->get_name() << " is already loaded" << std::endl;
            module.reset();
            dlclose(libraryHandle);
            return "";
        }

        _modules.insert({module->get_name(), {libraryHandle, module, path, {}}});
        std::cout << "Modules_manager module: " << module->get_name() << " loaded successfully" << std::endl;
        return module->get_name();
    }

    Modules_manager_ptr Modules_manager::create(const std::filesystem::path &modules_path) {
        return std::shared_ptr<Modules_manager>(new Modules_manager(modules_path));
    }
    Modules_manager::~Modules_manager() {
#if defined(__FreeBSD__)
        _running = false;
        if (_monitor_thread.joinable()) {
            _monitor_thread.join();
        }
#endif
    }

    void Modules_manager::init_modules() {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        _running_order.clear();
        std::cout << "Modules_manager start modules init\n";
        size_t iteration = 1;
        auto not_running =
            std::ranges::count_if(std::views::values(_modules), [](Internal_module &v) { return !v.is_initialized(); });
        while (not_running > 0) {
            std::cout << "________________________________________\n";
            std::cout << "Modules_manager module init iteration " << iteration << "\n";
            std::string modules_ran = "";
            size_t cnt = 0;
            for (auto &m: std::views::values(_modules)) {
                if (!m.is_initialized() && m.has_sufficient_dependencies(_modules)) {
                    this->do_init_module(m.get_module()->get_name());
                    cnt++;
                    modules_ran += std::format("\n{}) {}", cnt, m.get_module()->get_name());
                    std::cout << "Modules_manager module " << m.get_module()->get_name() << " is initialized"
                              << std::endl;
                }
            }
            std::cout << "Modules_manager module init iteration " << iteration << " ended\nModules: " << modules_ran
                      << std::endl;
            if (cnt == 0) {

                auto join = [](const std::vector<std::string> &strings, std::string const &separator) {
                    std::ostringstream result;
                    auto begin = strings.begin();
                    if (begin != strings.end())
                        result << *begin++;
                    while (begin != strings.end())
                        result << separator << *begin++;
                    return result.str();
                };
                std::string msg;
                for (auto &m: std::views::values(_modules)) {
                    if (!m.is_initialized()) {
                        msg += std::format("\n{} : dependencies: {}", m.get_module()->get_name(),
                                           join(m.get_module()->get_dependencies(), ", "));
                    }
                }
                throw std::runtime_error("There is not existing dependency or circular dependencies in" + msg);
            }

            not_running = std::ranges::count_if(std::views::values(_modules),
                                                [](Internal_module &v) { return !v.is_initialized(); });
            iteration++;
        }
    }

    Module_ptr Modules_manager::getptr() { return shared_from_this(); }

    void Modules_manager::run() {
        load_modules();
        std::cout << "Modules_manager start modules initial init" << std::endl;
        init_modules();
        std::cout << "____________________________________________" << std::endl;
        std::cout << "Modules_manager running modules" << std::endl;
        run_modules();
        std::cout << "Modules_manager initial start done" << std::endl;
    }

    void Modules_manager::init_module(const std::string &name) {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        this->do_init_module(name);
    }

    void Modules_manager::do_init_module(const std::string &name) {

        if (!_modules.contains(name)) {
            throw std::invalid_argument("Module: " + name + " is not loaded");
        }
        auto &m = _modules.at(name);
        if (m.is_initialized()) {
            return;
        }
        if (!m.has_sufficient_dependencies(_modules)) {
            throw std::runtime_error("Module: " + name + " does not have all dependencies loaded");
        }
        _running_order.push_back(m.get_module());
        std::cout << "Modules_manager running module " << m.get_module()->get_name() << std::endl;
        m.init(_modules);
        for (auto &i: m.get_module()->get_dependencies()) {
            _modules.at(i).add_module_dependent(m.get_module()->get_name());
        }
    }

    void Modules_manager::init(const Modules &modules){};

    void Modules_manager::stop_modules() {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        std::cout << "____________________________________________" << std::endl;
        std::cout << "Modules_manager running all modules stop" << std::endl;

        // Collect all keys (module names) into a vector
        std::vector<std::string> keys;
        for (const auto &pair: _modules) {
            if (pair.second.get_module().get() != this) {
                keys.push_back(pair.first);
            }
        }

        // Iterate over the copied keys and stop each module
        for (const auto &k: keys) {
            do_stop_module(k);
        }
        std::cout << "Modules_manager all modules stopped" << std::endl;
    }

    void Modules_manager::stop_module(const std::string &name) {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        return do_stop_module(name);
    }

    void Modules_manager::do_stop_module(const std::string &name) {
        if (!_modules.contains(name) || !_modules.at(name).is_initialized()) {
            return;
        }
        std::cout << "Modules_manager closing module " << name << std::endl;
        auto &m = _modules.at(name);
        auto e = std::ranges::remove(_running_order, m.get_module());
        _running_order.erase(e.begin(), e.end());
        if (m.get_module().get() == this) {
            std::cout << "Module_manager closing error: Module_manager can not close itself!!!" << std::endl;
            return;
        }
        for (auto &i: m.get_module_dependent()) {
            std::cout << "Modules_manager closing " << name << " module dependency" << std::endl;
            this->do_stop_module(i);
        }

        for (auto &i: m.get_module()->get_dependencies()) {
            if (_modules.contains(i)) {
                _modules.at(i).remove_module_dependent(m.get_module()->get_name());
            }
        }
        m.stop();
        _modules.erase(name);
        if (dlclose(m.get_library_handler()) != 0) {
            throw std::runtime_error("Modules_manager error closing module " + name + " reason: " + dlerror() + "\n");
        }
        std::cout << "Modules_manager module: " << name << " closed successfully" << std::endl;
    }

    void Modules_manager::set_allow_modules_load(bool v) {
        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        this->_allow_modules_load = v;
    }

    bool Modules_manager::is_allowed_modules_load() {
        std::shared_lock<std::shared_mutex> lock(this->_mutex);
        return _allow_modules_load;
    }

    void Modules_manager::stop() {}

    void Modules_manager::do_run_module(const std::string &name) {
        std::cout << "Modules_manager running module " << name << std::endl;
        if (_modules.at(name).get_module().get() != this) {
            this->_modules.at(name).run();
        }
    }

    void Modules_manager::run_modules() {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        for (auto &v: _running_order) {
            do_run_module(v->get_name());
        }
    }

    void Modules_manager::run_module(const std::string &name) {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        this->do_run_module(name);
    }

    std::vector<std::string> Modules_manager::get_module_names() {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        auto kv = std::views::keys(_modules);
        return {kv.begin(), kv.end()};
    }

    void Modules_manager::load_modules() {

        std::unique_lock<std::shared_mutex> lock(this->_mutex);
        _modules.insert({"module_manager", {nullptr, getptr()}});
        _modules.at("module_manager").init(_modules);

        std::cout << "Modules_manager running initial modules loading...\n";
        for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(_modules_path)) {
            if (dirEntry.path().extension() != ".so") {
                continue;
            }
            this->do_load_module(dirEntry.path());
        }
        std::string output = "";
        size_t cnt = 0;
        for (auto &[k, v]: _modules) {
            cnt++;
            output += std::format("\n {}) {}", cnt, k);
        }
        std::cout << "Modules_manager initial modules loading done.\nLoaded modules: " << output << std::endl;
    }

    void *Modules_manager::Internal_module::get_library_handler() const { return _library_handler; }

    Module_ptr Modules_manager::Internal_module::get_module() const { return _module; }

    std::vector<std::string> Modules_manager::Internal_module::get_module_dependent() const {
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
                                                      const std::string &path,
                                                      const std::vector<std::string> &module_dependent) {
        this->_library_handler = library_handler;
        this->_module = module;
        this->_module_dependent = module_dependent;
        this->_file_path = path;
    }

    void Modules_manager::Internal_module::init(const Internal_modules &modules) {
        if (_is_initialized) {
            return;
        }
        Modules deps = {{"module_manager", modules.at("module_manager")._module}};
        for (auto &i: _module->get_dependencies()) {
            deps.insert({i, modules.at(i)._module});
        }
        _module->init(deps);
        _is_initialized = true;
    }

    bool Modules_manager::Internal_module::is_initialized() const { return _is_initialized; }

    bool Modules_manager::Internal_module::has_sufficient_dependencies(const Internal_modules &modules) {
        for (auto &i: _module->get_dependencies()) {
            if ((!modules.contains(i)) || (!modules.at(i).is_initialized())) {
                return false;
            }
        }
        return true;
    }

    void Modules_manager::Internal_module::stop() {
        if (!_is_initialized) {
            return;
        }
        _module->stop();
        _is_initialized = false;
    }

    std::string Modules_manager::Internal_module::get_file_path() { return _file_path; }

    bool Modules_manager::Internal_module::is_running() const { return _is_running; }

    void Modules_manager::Internal_module::run() {
        if (_is_running || !_is_initialized) {
            return;
        }
        _module->run();
        _is_running = true;
    }

} // namespace gb
