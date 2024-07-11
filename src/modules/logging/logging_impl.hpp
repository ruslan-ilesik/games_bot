//
// Created by ilesik on 7/3/24.
//

#pragma once

#include "logging.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace gb {

    /**
     * @class Logging_impl
     * @brief Implementation of the Logging module using spdlog.
     *
     * This class provides logging functionalities by utilizing the spdlog library.
     */
    class Logging_impl : public Logging {
    private:
        /// Shared pointer to the asynchronous logger.
        std::shared_ptr<spdlog::async_logger> _log;

    public:
        /**
         * @brief Constructor for the Logging_impl class.
         *
         * Initializes the Logging_impl with the name "logging" and an empty list of dependencies.
         */
        Logging_impl();

        /**
         * @brief Destructor for the Logging_impl class.
         */
        virtual ~Logging_impl() = default;

        /**
         * @brief Initializes the Logging_impl module with the provided modules.
         *
         * This method initializes spdlog settings, including thread pool, log sinks (stdout and rotating file),
         * log pattern, and log level.
         *
         * @param modules A reference to a map of modules.
         */
        virtual void innit(const Modules& modules) override;

        /**
         * @brief Placeholder for run method.
         *
         * This method does not perform any specific actions in this implementation.
         */
        virtual void run() override;

        /**
         * @brief Logs an informational message.
         *
         * @param message The message to log.
         */
        virtual void info(const std::string& message) override;

        /**
         * @brief Logs a trace message.
         *
         * @param message The message to log.
         */
        virtual void trace(const std::string& message) override;

        /**
         * @brief Logs a debug message.
         *
         * @param message The message to log.
         */
        virtual void debug(const std::string& message) override;

        /**
         * @brief Logs a warning message.
         *
         * @param message The message to log.
         */
        virtual void warn(const std::string& message) override;

        /**
         * @brief Logs an error message.
         *
         * @param message The message to log.
         */
        virtual void error(const std::string& message) override;

        /**
         * @brief Logs a critical message.
         *
         * @param message The message to log.
         */
        virtual void critical(const std::string& message) override;

        /**
         * @brief Placeholder for stop method.
         *
         * This method does not require specific implementation for spdlog.
         */
        virtual void stop() override;
    };

    /**
     * @brief Creates a new Logging_impl module.
     *
     * @return A shared pointer to the created Logging_impl module.
     */
    Module_ptr create();

} // namespace gb
