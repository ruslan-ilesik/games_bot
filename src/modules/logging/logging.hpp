//
// Created by ilesik on 7/3/24.
//

#pragma once
#include <any>
#include "../../module/module.hpp"

namespace gb {

    class Logging;

    /**
     * @typedef Logging_ptr
     * @brief A shared pointer to a Logging object.
     */
    typedef std::shared_ptr<Logging> Logging_ptr;

    /**
     * @class Logging
     * @brief An abstract base class for logging functionality.
     *
     * This class defines the interface for logging messages at various severity levels.
     * It inherits from the Module class.
     */
    class Logging: public Module {
    public:
        /**
         * @brief Constructor for the Logging class.
         *
         * @param name The name of the logging module.
         * @param dep A vector of dependencies for the logging module.
         */
        Logging(const std::string& name, const std::vector<std::string>& dep) : Module(name, dep) {};

        /// Virtual destructor.
        virtual ~Logging() = default;

        /**
         * @brief Logs an informational message.
         *
         * @param message The message to log.
         */
        virtual void info(const std::string& message) = 0;

        /**
         * @brief Logs a debug message.
         *
         * @param message The message to log.
         */
        virtual void debug(const std::string& message) = 0;

        /**
         * @brief Logs a trace message.
         *
         * @param message The message to log.
         */
        virtual void trace(const std::string& message) = 0;

        /**
         * @brief Logs a warning message.
         *
         * @param message The message to log.
         */
        virtual void warn(const std::string& message) = 0;

        /**
         * @brief Logs an error message.
         *
         * @param message The message to log.
         */
        virtual void error(const std::string& message) = 0;

        /**
         * @brief Logs a critical message.
         *
         * @param message The message to log.
         */
        virtual void critical(const std::string& message) = 0;

        /**
         * @brief Runs the logging module.
         *
         * This function is called to start the logging module. It can be overridden
         * by derived classes to provide specific functionality.
         *
         * @param modules A reference to a map of modules.
         */
        virtual void init(const Modules& modules) {};

        /**
         * @brief Stops the logging module.
         *
         * This function is called to stop the logging module. It can be overridden
         * by derived classes to provide specific functionality.
         */
        virtual void stop() {};
    };

    /**
     * @brief Creates a new logging module.
     *
     * This function is used to create a new instance of a logging module.
     *
     * @return A shared pointer to the created logging module.
     */
    extern "C" Module_ptr create();

} // namespace gb
