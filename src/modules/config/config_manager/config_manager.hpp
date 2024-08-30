//
// Created by ilesik on 7/4/24.
//

#pragma once
#include <shared_mutex>
#include "../config.hpp"

namespace gb {

    /**
     * @brief The Config_manager class is a concrete class that manages multiple Config instances.
     * It provides methods to initialize, run, and stop configurations, as well as retrieve configuration values.
     */
    class Config_manager : public Config {
    private:
        /**
         * @brief Vector of pointers to Config instances managed by Config_manager.
         */
        std::vector<Config*> _configs;

        /**
         * @brief Shared mutex for thread-safe access to the _configs vector.
         */
        std::shared_mutex _mutex;

    public:
        /**
         * @brief Constructor for Config_manager.
         * Initializes a new instance of Config_manager.
         */
        Config_manager();

        /**
         * @brief Virtual destructor for Config_manager.
         */
        virtual ~Config_manager() = default;

        /**
         * @brief Method to initialize the Config_manager and its configurations.
         * @param modules Map of all modules managed by the application, including the module manager.
         */
        virtual void init(const Modules& modules);

        /**
         * @brief Method to stop the Config_manager and its configurations.
         */
        virtual void stop();

        /**
         * @brief Method to run the Config_manager and its configurations.
         */
        virtual void run();

        /**
         * @brief Method to get a configuration value by name.
         * @param name The name of the configuration parameter to retrieve.
         * @return The value of the configuration parameter.
         * @note Searches through all managed Config instances to find the value and returns first one found.
         * @throws std::runtime_error if the configuration parameter is not found.
         */
        virtual std::string get_value(const std::string& name);

        /**
         * @brief Method to get a configuration value by name, with a default return value.
         * @param name The name of the configuration parameter to retrieve.
         * @param default_return The default value to return if the configuration parameter is not found.
         * @return The value of the configuration parameter if found, otherwise the default return value.
         * @note Searches through all managed Config instances to find the value and returns first one found.
         */
        virtual std::string get_value_or(const std::string& name, const std::string& default_return);

        /**
         * @brief Method to add a Config instance to the manager.
         * @param config Pointer to the Config instance to add.
         */
        virtual void add_config(Config* config);

        /**
         * @brief Method to remove a Config instance from the manager.
         * @param config Pointer to the Config instance to remove.
         */
        virtual void remove_config(Config* config);
    };

    /**
     * @brief Type alias for a shared pointer to a Config_manager instance.
     */
    typedef std::shared_ptr<Config_manager> Config_manager_ptr;

    /**
     * @brief External C function declaration to create a Config_manager instance.
     * @return Shared pointer to a new Module instance.
     * @note This function is intended for dynamic module loading.
     */
    extern "C" Module_ptr create();
} // namespace gb


