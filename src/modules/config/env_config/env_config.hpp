//
// Created by ilesik on 7/5/24.
//

#pragma once

#include "../config.hpp"
#include "../config_manager/config_manager.hpp"

namespace gb {

    /**
     * @brief The Env_config class provides configuration values from environment variables.
     * It inherits from the Config class and integrates with a Config_manager for additional configuration management.
     */
    class Env_config : public Config {
    private:
        /**
         * @brief Shared pointer to a Config_manager instance.
         */
        Config_manager_ptr config_manager;

    public:
        /**
         * @brief Constructor for Env_config.
         * Initializes a new instance of Env_config.
         */
        Env_config();

        /**
         * @brief Method to initialize the Env_config instance.
         * @param modules Map of all modules managed by the application, including the module manager.
         * @note This method sets the config_manager by retrieving it from the provided modules.
         */
        virtual void innit(const Modules& modules);

        /**
         * @brief Method to stop the Env_config instance.
         * @note This method performs any necessary cleanup or shutdown procedures for the Env_config instance.
         */
        virtual void stop();

        /**
         * @brief Method to run the Env_config instance.
         * @note This method defines the runtime behavior of the Env_config instance.
         */
        virtual void run();

        /**
         * @brief Method to get a configuration value by name from the environment variables.
         * @param name The name of the configuration parameter to retrieve.
         * @return The value of the configuration parameter.
         * @throws std::runtime_error if the configuration parameter is not found.
         */
        virtual std::string get_value(const std::string& name);

        /**
         * @brief Method to get a configuration value by name from the environment variables, with a default return value.
         * @param name The name of the configuration parameter to retrieve.
         * @param default_return The default value to return if the configuration parameter is not found.
         * @return The value of the configuration parameter if found, otherwise the default return value.
         */
        virtual std::string get_value_or(const std::string& name, const std::string& default_return);
    };

    /**
     * @brief External C function declaration to create an Env_config instance.
     * @return Shared pointer to a new Module instance.
     * @note This function is intended for dynamic module loading.
     */
    extern "C" Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Env_config>());
    }

} // namespace gb