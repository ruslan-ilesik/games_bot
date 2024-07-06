//
// Created by ilesik on 7/6/24.
//

#pragma once
#include <nlohmann/json.hpp>
#include "../config.hpp"
#include "../config_manager/config_manager.hpp"

namespace gb {

    /**
     * @brief The Json_config class provides configuration values from a JSON file.
     * It inherits from the Config class and integrates with a Config_manager for additional configuration management.
     */
    class Json_config: public Config {
    private:
        /**
         * @brief JSON object to store configuration values.
         */
        nlohmann::json _json;

        /**
         * @brief Shared pointer to a Config_manager instance.
         */
        Config_manager_ptr _config_manager;

    public:
        /**
         * @brief Constructor for Json_config.
         * Initializes a new instance of Json_config.
         */
        Json_config();

        /**
         * @brief Virtual destructor for Json_config.
         */
        virtual ~Json_config() = default;

        /**
         * @brief Method to run the Json_config instance.
         * @note This method defines the runtime behavior of the Json_config instance.
         */
        virtual void run();

        /**
         * @brief Method to initialize the Json_config instance.
         * @param modules Map of all modules managed by the application, including the module manager.
         * @note This method sets the config_manager by retrieving it from the provided modules and registers itself in config_manager also loads the JSON configuration.
         */
        virtual void innit(const Modules& modules);

        /**
         * @brief Method to stop the Json_config instance.
         * @note This method performs stop of json config by removing itself from config_manager.
         */
        virtual void stop();

        /**
         * @brief Method to get a configuration value by name from the JSON file.
         * @param name The name of the configuration parameter to retrieve.
         * @return The value of the configuration parameter.
         * @throws std::runtime_error if the configuration parameter is not found.
         */
        virtual std::string get_value(const std::string& name);

        /**
         * @brief Method to get a configuration value by name from the JSON file, with a default return value.
         * @param name The name of the configuration parameter to retrieve.
         * @param default_return The default value to return if the configuration parameter is not found.
         * @return The value of the configuration parameter if found, otherwise the default return value.
         */
        virtual std::string get_value_or(const std::string& name, const std::string& default_return);
    };

    /**
     * @brief External C function declaration to create a Json_config instance.
     * @return Shared pointer to a new Module instance.
     * @note This function is intended for dynamic module loading.
     */
    extern "C" Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Json_config>());
    }

} // namespace gb