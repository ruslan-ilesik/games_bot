//
// Created by ilesik on 6/30/24.
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace gb {

    /**
     * @brief Forward declaration of the Module class.
     */
    class Module;

    /**
     * @brief Shared pointer type alias for Module.
     */
    typedef std::shared_ptr<Module> Module_ptr;

    /**
     * @brief Map type alias for mapping module names to Module pointers.
     */
    typedef std::map<std::string, Module_ptr> Modules;

    /**
     * @brief Function pointer type alias for creating Module instances.
     */
    typedef Module_ptr (*create_func_t)();

    /**
     * @brief Exception class for methods that are not implemented.
     */
    class Not_implemented_exception : public std::logic_error
    {
    public:
        /**
         * @brief Constructor for Not_implemented_exception.
         * @param error Error message indicating the method is not implemented.
         */
        explicit Not_implemented_exception(const std::string& error = "Method is not implemented");
    };

    /**
     * @brief Base class for modules.
     */
    class Module {
    protected:
        /**
         * @brief Name of the module.
         */
        std::string _name;

        /**
         * @brief Dependencies of the module: names of the modules this module depends on.
         */
        std::vector<std::string> _dependencies;

        /**
         * @brief Protected constructor for Module.
         * @param name Name of the module.
         * @param dependencies List of dependencies for the module.
         */
        explicit Module(const std::string& name, const std::vector<std::string>& dependencies = {});

    public:
        /**
         * @brief Virtual destructor for Module.
         */
        virtual ~Module() = default;

        /**
         * @brief Get the name of the module.
         * @return Name of the module.
         */
        std::string get_name();

        /**
         * @brief Get the dependencies of the module.
         * @return List of dependencies of the module.
         */
        std::vector<std::string> get_dependencies();

        /**
         * @brief Innit method to be implemented by subclasses.
         * @param modules Map of all modules managed by the application, including the module manager as "module_manager".
         *        Any additional dependencies required by the module should also be included in this map.
         * @note This method must be overridden by subclasses to define module behavior.
         */
        virtual void init(const Modules& modules) = 0;

        /**
         * @brief Stop method to be implemented by subclasses.
         * @note This method must be overridden by subclasses to define module stopping behavior.
         */
        virtual void stop() = 0;

        /**
       * @brief Run method to be implemented by subclasses.
       * @note This method must be overridden by subclasses to define module stopping behavior. Execution order is the same as init.
       */
        virtual void run() = 0;
    };

    /**
     * @brief External C function declaration to create a Module instance.
     * @return Shared pointer to a new Module instance.
     */
    extern "C" Module_ptr create();
}

