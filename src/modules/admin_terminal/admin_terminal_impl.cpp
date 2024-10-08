//
// Created by ilesik on 7/6/24.
//

#include "admin_terminal_impl.hpp"

namespace gb {

    Admin_terminal_impl::Admin_terminal_impl() : Admin_terminal("admin_terminal", {}) {
        if (pipe(_pipe_fd) == -1) {
            throw std::runtime_error("Failed to create pipe");
        }
    }

    void Admin_terminal_impl::stop() {
        _stop_flag = true;

        std::cout << "Admin_terminal_impl waiting thread to finish" << std::endl;

        // Signal stop using pipe
        const char stop_message[] = "STOP";
        if (write(_pipe_fd[1], stop_message, sizeof(stop_message)) == -1) {
            std::cerr << "Failed to write to pipe" << std::endl;
        }

        if (_terminal_thread.joinable()) {
            _terminal_thread.join();
        }

        std::cout << "Admin_terminal_impl stopped" << std::endl;
    }

    void Admin_terminal_impl::init(const Modules &modules) {
        _modules_manager = std::static_pointer_cast<Modules_manager>(modules.at("module_manager"));
    }

    void Admin_terminal_impl::add_command(const Terminal_command_ptr &cmd) {
        std::unique_lock<std::mutex> lock(_mutex);
        std::cout << "Admin_terminal_impl adding new command " << cmd->get_name() << std::endl;
        if (_commands.contains(cmd->get_name())) {
            throw std::runtime_error("Admin_terminal_impl command " + cmd->get_name() + " was already registered");
        }
        _commands.insert({cmd->get_name(), cmd});
    }

    void Admin_terminal_impl::run() {
        _terminal_thread = std::thread([this]() {
            auto split = [](const std::string &s, const std::string &delimiter) {
                size_t pos_start = 0, pos_end, delim_len = delimiter.length();
                std::string token;
                std::vector<std::string> res;

                while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
                    token = s.substr(pos_start, pos_end - pos_start);
                    pos_start = pos_end + delim_len;
                    res.push_back(token);
                }

                res.push_back(s.substr(pos_start));
                return res;
            };
            struct pollfd fds[2];
            memset(fds, 0, sizeof(fds));

            // Watch stdin (fd 0) for input
            fds[0].fd = 0;   // stdin
            fds[0].events = POLLIN;

            // Watch pipe_fd[0] for read, which signals stop
            fds[1].fd = _pipe_fd[0];
            fds[1].events = POLLIN;

            while (!_stop_flag) {
                int ret = poll(fds, 2, -1); // block indefinitely

                if (ret > 0) {
                    if (fds[0].revents & POLLIN) {
                        // Read input from stdin (std::cin)
                        std::string input;
                        std::getline(std::cin, input);
                        auto args = split(input, " ");
                        if (args.empty()) {
                            continue;
                        }
                        std::unique_lock lock(_mutex);
                        if (!_commands.contains(args[0])) {
                            std::cout << "Admin_terminal_impl command " << args[0]
                                      << " not found. Write help to get list of commands\n";
                            continue;
                        }
                        auto command = _commands.at(args[0]);
                        lock.unlock();
                        command->operator()({args.begin() + 1, args.end()});
                    }
                } else if (ret == -1) {
                    std::cerr << "Error in poll" << std::endl;
                    break;
                }
            }
        });

        // Add basic commands
        add_command("help",
                    "Help command to get help about commands.",
                    "Arguments: command name to get help about specific command, with no argument writes list of commands.",
                    [this](const std::vector<std::string> &args) {
                        if (args.empty()) {
                            std::string output = "______LIST OF COMMANDS______\n";
                            size_t cnt = 0;
                            for (auto &[k, v]: _commands) {
                                cnt++;
                                output += std::format("{}) {} - {}\n", cnt, v->get_name(), v->get_description());
                            }
                            std::cout << output;
                        } else {
                            if (!_commands.contains(args[0])) {
                                std::cout << "Help command error: command " << args[0]
                                          << " not found. Write help to get list of available commands" << std::endl;
                                return;
                            }
                            auto &cmd = _commands.at(args[0]);
                            std::cout << cmd->get_name() << " - " << cmd->get_description() << '\n' << cmd->get_help()
                                      << std::endl;
                        }
                    });

        add_command("module_load",
                    "Command to load and run module by specifying relative path to .so file.",
                    "Arguments: relative path to .so file to load and run.",
                    [this](const std::vector<std::string> &args) {
                        if (args.empty()) {
                            std::cout << "module_load command error: no path to module were provided" << std::endl;
                            return;
                        }
                        try {
                            auto m = _modules_manager->load_module(args[0]);
                        }
                        catch (...) {
                            std::cout << "module_load command error: error loading module " << args[0] << std::endl;
                        }
                    });

        add_command("module_load_all",
                    "Command to load and run all modules.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        try {
                            _modules_manager->load_modules();
                        }
                        catch (...) {
                            std::cout << "module_load_all command error: error loading modules" << std::endl;
                        }
                    });

        add_command("module_init_all",
                    "Command to initialize all loaded modules.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        try {
                            _modules_manager->init_modules();
                        }
                        catch (...) {
                            std::cout << "module_init_all command error: error initializing modules" << std::endl;
                        }
                    });

        add_command("module_run_all",
                    "Command to run all loaded modules.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        try {
                            _modules_manager->run_modules();
                        }
                        catch (...) {
                            std::cout << "module_run_all command error: error running modules" << std::endl;
                        }
                    });

        add_command("module_init",
                    "Command to initialize specified loaded module.",
                    "Arguments: module name to initialize.",
                    [this](const std::vector<std::string> &args) {
                        if (args.empty()) {
                            std::cout << "module_init error: No module name provided" << std::endl;
                            return;
                        }
                        try {
                            _modules_manager->init_module(args[0]);
                        }
                        catch (...) {
                            std::cout << "module_init command error: error initializing module " << args[0] << std::endl;
                        }
                    });

        add_command("module_run",
                    "Command to run specified loaded module.",
                    "Arguments: module name to run.",
                    [this](const std::vector<std::string> &args) {
                        if (args.empty()) {
                            std::cout << "module_run error: No module name provided" << std::endl;
                            return;
                        }
                        try {
                            _modules_manager->run_module(args[0]);
                        }
                        catch (...) {
                            std::cout << "module_run command error: error running module " << args[0] << std::endl;
                        }
                    });

        add_command("module_list",
                    "Command to list loaded modules.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        auto m_n = _modules_manager->get_module_names();
                        std::string output = "_____LIST OF MODULES_____\n";
                        for (size_t i = 0; i < m_n.size(); i++){
                            output += std::format("{}) {}\n",i+1,m_n[i]);
                        }
                        std::cout << output;
                    });

        add_command("module_load_init_run_all",
                    "Command to load, initialize, and run all modules.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        try{
                            _modules_manager->load_modules();
                            _modules_manager->init_modules();
                            _modules_manager->run_modules();
                        }
                        catch (...){
                            std::cout << "Command module_load_init_run_all error" << std::endl;
                        }
                    });

        add_command("module_load_disable",
                    "Command to disable module loading.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        _modules_manager->set_allow_modules_load(false);
                        std::cout << "Module load disabled" << std::endl;
                    });

        add_command("module_load_enable",
                    "Command to enable module loading.",
                    "Arguments: no arguments.",
                    [this](const std::vector<std::string> &args) {
                        _modules_manager->set_allow_modules_load(true);
                        std::cout << "Module load enabled" << std::endl;
                    });

        add_command("module_stop",
                    "Command to stop specified module and deallocate it.",
                    "Arguments: module name to stop.",
                    [this](const std::vector<std::string> &args) {
                        if (args.empty()) {
                            std::cout << "module_stop error: No module name provided" << std::endl;
                            return;
                        }
                        if (args[0] == "admin_console") {
                            std::cout
                                << "module_stop command error: You are trying to stop the admin console, which is not possible via itself. Remove the .so file of this module from the modules folder to stop it."
                                << std::endl;
                            return;
                        }
                        try {
                            _modules_manager->stop_module(args[0]);
                        }
                        catch (...) {
                            std::cout << "module_stop command error: error stopping module " << args[0] << std::endl;
                        }
                    });
    }

    void Admin_terminal_impl::remove_command(const std::string &name) {
        std::unique_lock<std::mutex> lock(_mutex);
        _commands.erase(name);
    }

    void Admin_terminal_impl::add_command(std::string name, std::string description, std::string help,
                                          Terminal_command_callback callback) {
        add_command(std::dynamic_pointer_cast<Terminal_command>(std::make_shared<Terminal_command_impl>(name,description,help,callback)));
    }

    Admin_terminal_impl::~Admin_terminal_impl() {
        close(_pipe_fd[0]);
        close(_pipe_fd[1]);
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Admin_terminal_impl>());
    }

} // gb
