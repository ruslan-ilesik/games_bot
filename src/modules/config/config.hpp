//
// Created by ilesik on 7/4/24.
//

#pragma once
#include "../../module/module.hpp"


namespace gb {

    /**
     * @brief The Config class is an abstract base class that inherits from Module.
     * It provides an interface for configuration management in the application.
     */
    class Config : public Module {
    protected:
        /**
         * @brief Protected constructor for the Config class.
         * @param name The name of the configuration module.
         * @param dependencies A list of dependencies for this configuration module.
         */
        Config(const std::string& name, const std::vector<std::string>& dependencies)
                : Module(name, dependencies) {}

    public:
        /**
         * @brief Virtual destructor for the Config class.
         */
        virtual ~Config() = default;

        /**
         * @brief Pure virtual method to get a configuration value by name.
         * @param name The name of the configuration parameter to retrieve.
         * @return The value of the configuration parameter.
         * @note This method must be implemented by derived classes.
         */
        virtual std::string get_value(const std::string& name) = 0;

        /**
         * @brief Pure virtual method to get a configuration value by name, with a default return value.
         * @param name The name of the configuration parameter to retrieve.
         * @param default_return The default value to return if the configuration parameter is not found.
         * @return The value of the configuration parameter if found, otherwise the default return value.
         * @note This method must be implemented by derived classes.
         */
        virtual std::string get_value_or(const std::string& name, const std::string& default_return) = 0;
    };

    /**
     * @brief Type alias for a shared pointer to a Config instance.
     */
    typedef std::shared_ptr<Config> Config_ptr;

    /**
     * @brief External C function declaration to create a Config instance.
     * @return Shared pointer to a new Module instance.
     * @note This function is intended for dynamic module loading.
     */
    extern "C" Module_ptr create();
} // namespace gb