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

    /**
     * @brief Forward declaration of Modules_manager class.
     */
    class Modules_manager;

    /**
     * @brief Shared pointer type alias for Modules_manager.
     */
    typedef std::shared_ptr<Modules_manager> Modules_manager_ptr;

    /**
     * @brief Represents a manager for handling dynamic modules.
     * Inherits from Module and enables shared ownership using std::enable_shared_from_this.
     */
    class Modules_manager : public Module, public std::enable_shared_from_this<Module> {
    private:
        /**
         * @brief  Forward declaration of class representing an internal module managed by Modules_manager.
         */
        class Internal_module;

        /**
         * @brief Map type alias for mapping module names to Internal_module instances.
         */
        typedef std::map<std::string, Internal_module> Internal_modules;

        /**
         * @brief Represents an internal module loaded dynamically.
         */
        class Internal_module {
        private:

            /**
             * @brief Handler for the loaded dynamic library.
             */
            void *_library_handler;

            /**
             * @brief Pointer to the loaded Module instance.
             */
            Module_ptr _module;

            /**
             * @brief List of module names which are dependent on this module.
             */
            std::vector<std::string> _module_dependent;

            /**
             * @brief File path of the dynamic library.
             */
            std::string _file_path;

            /**
             * @brief Flag indicating if the module is initialized.
             */
            bool _is_initialized = false;

            /**
             * @brief Flag indicating if the module is running.
             */
            bool _is_running = false;

        public:
            /**
             * @brief Constructor for Internal_module.
             * @param library_handler Handler for the loaded dynamic library.
             * @param module Pointer to the loaded Module instance.
             * @param path File path of the dynamic library.
             * @param module_dependent List of modules which depends on this module.
             */
            Internal_module(void *library_handler, const Module_ptr &module, const std::string &path = "",
                            const std::vector<std::string> &module_dependent = {});

            /**
             * @brief Get the handler for the loaded dynamic library.
             * @return Handler for the loaded dynamic library.
             */
            void *get_library_handler() const;

            /**
             * @brief Get the pointer to the loaded Module instance.
             * @return Pointer to the loaded Module instance.
             */
            Module_ptr get_module() const;

            /**
             * @brief Get the list of module names dependent on this module.
             * @return List of module names dependent on this module.
             */
            std::vector<std::string> get_module_dependent() const;

            /**
             * @brief Add a module name to the list of dependent on this module.
             * @param name Module name to add as a dependent on this module.
             * @note it is called by modules_manger when some dependent module gets loaded.
             */
            void add_module_dependent(const std::string &name);

            /**
             * @brief Remove a module name from the list of dependent on this module.
             * @param name Module name to remove from dependent on this module.
             * @note it is called by modules_manger when some dependent module gets closed.
             */
            void remove_module_dependent(const std::string &name);

            /**
             * @brief Check if the module is initialized.
             * @return True if the module is initialized, false otherwise.
             */
            bool is_initialized() const;

            /**
             * @brief Check if the module is running.
             * @return True if the module is running, false otherwise.
             */
            bool is_running() const;

            /**
             * @brief Check if the module has sufficient dependencies fulfilled.
             * @param modules Map of internal modules managed by Modules_manager.
             * @return True if the module has sufficient dependencies, false otherwise.
             */
            bool has_sufficient_dependencies(const Internal_modules &modules);

            /**
             * @brief Innit method to innit the module's functionality.
             * @param modules Map of internal modules managed by Modules_manager.
             * @note It copies all of the modules defined as dependencies to new map and pass them to module + modules_manager
             */
            void innit(const Internal_modules &modules);

            /**
             * @brief Run method to execute the module's functionality.
             */
            void run();

            /**
             * @brief Get the file path of the dynamic library.
             * @return File path of the dynamic library.
             */
            std::string get_file_path();

            /**
             * @brief Stop method to stop the module.
             */
            void stop();
        };

        /**
         * @brief Mutex for thread-safe access to internal data structures.
         */
        std::shared_mutex _mutex;

        /**
         * @brief File watcher for monitoring module file changes.
         */
        filewatch::FileWatch <std::string> _watch;

        /**
         * @brief Map of loaded internal modules.
        */
        std::map<std::string, Internal_module> _modules;

        /**
          * @brief Path to the directory containing module files.
         */
        std::filesystem::path _modules_path;

        /**
         * @brief Flag indicating whether module loading is allowed.
         * @note Does not impact modules stopping.
        */
        bool _allow_modules_load = true;

        /**
         * @brief List of modules in orde they should be ran.
         * @note innit_modules clear it.
         */
        std::vector<Module_ptr> _running_order{};

        /**
         * @brief Private constructor for Modules_manager.
         * @param modules_path Path to the directory containing module files.
         */
        explicit Modules_manager(const std::filesystem::path &modules_path);

    public:
        /**
         * @brief Factory method to create an instance of Modules_manager.
         * @param modules_path Path to the directory containing module files. Default is current directory.
         * @return Shared pointer to the created Modules_manager instance.
         */
        static Modules_manager_ptr create(const std::filesystem::path &modules_path = {"./"});

        /**
         * @brief Virtual destructor for Modules_manager.
         */
        virtual ~Modules_manager() = default;


        /**
        * @brief Innit_modules method to innit all loaded modules.
        */
        virtual void innit_modules();

        /**
         * @brief Innit_module method to execute a innit module by name.
         * @param name Name of the module to innit.
         */
        virtual void innit_module(const std::string &name);

        /**
         * @brief Stop method to stop all running modules and deallocate them.
         */
        virtual void stop_modules();

        /**
         * @brief Run method to run all loaded methods.
        */
        virtual void run_modules();

        /**
         * @brief Run method to be overridden by subclasses.
         * @param modules Map of all modules managed by the application, including the module manager as "module_manager".
         *        Any additional dependencies required by the module should also be included in this map.
         * @note This method must be overridden by subclasses to define module behavior.
         */
        virtual void innit(const Modules &modules);

        /**
         * @brief Stop method to be overridden by subclasses.
         * @note This method must be overridden by subclasses to define module behavior.
         */
        virtual void stop();

        /**
         * @brief Run method to be overridden by subclasses.
         * @note This method must be overridden by subclasses to define module behavior.
         */
        virtual void run();

        /**
        * @brief Method to run a module by name.
        * @param name Name of the module to run.
        */
        virtual void run_module(const std::string& name);

        /**
         * @brief Stop method to stop a specific module by name.
         * @param name Name of the module to stop and deallocate.
         * @note It will also stop all modules dependent on current module.
         */
        virtual void stop_module(const std::string &name);

        /**
        * @brief Load all module dynamically from the default path.
        */
        virtual void load_modules();

        /**
         * @brief Load a module dynamically from the specified file path.
         * @param path path of the module to load.
         * @return Name of the loaded module.
         */
        virtual std::string load_module(const std::string &path);

        /**
         * @brief Set whether module loading is allowed.
         * @param v True to allow module loading, false otherwise.
         */
        virtual void set_allow_modules_load(bool v);

        /**
         * @brief Check if module loading is allowed.
         * @return True if module loading is allowed, false otherwise.
         */
        virtual bool is_allowed_modules_load();

        /**
         * @brief Get list of all loaded module names.
         * @return std::vector<std::string> list of module names.
         */
        virtual std::vector<std::string> get_module_names();

    protected:


    private:
        /**
         * @brief Get a shared pointer to this Modules_manager instance.
         * @return Shared pointer to this Modules_manager instance.
         */
        Module_ptr getptr();

        /**
         * @brief Internal method to stop a module by name.
         * @param name Name of the module to stop.
         * @note It will also stop all modules dependent on current module.
         */
        void do_stop_module(const std::string &name);

        /**
         * @brief Internal method to innit a module by name.
         * @param name Name of the module to innit.
         */
        void do_innit_module(const std::string &name);

        /**
         * @brief Internal method to run a module by name.
         * @param name Name of the module to innit.
         */
        void do_run_module(const std::string &name);

        /**
         * @brief Internal method to load a module dynamically from the specified file path.
         * @param file path of the module to load.
         * @return Name of the loaded module.
         */
        std::string do_load_module(const std::string &path);
    };

}
