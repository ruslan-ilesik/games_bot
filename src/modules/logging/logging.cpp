//
// Created by ilesik on 7/3/24.
//

#include "logging.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>


namespace gb {

    /**
     * @class Spdlog_logger
     * @brief Implementation of the Logging module using spdlog.
     *
     * This class provides logging functionalities by utilizing the spdlog library.
     */
    class Spdlog_logger : public Logging {
    private:
        /// Shared pointer to the asynchronous logger.
        std::shared_ptr<spdlog::async_logger> _log;

    public:
        /**
         * @brief Constructor for the Spdlog_logger class.
         */
        Spdlog_logger() : Logging("logging", {}) {}

        /**
         * @brief Innit the logging module and initializes spdlog settings.
         *
         * @param modules A reference to a map of modules.
         */
        virtual void innit(const Modules& modules) override {
            spdlog::init_thread_pool(8192, 2);
            const std::string log_name = "bot_log.log";
            static std::vector<spdlog::sink_ptr> sinks;
            static auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            static auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_name, 1024 * 1024 * 5, 10);
            sinks.push_back(stdout_sink);
            sinks.push_back(rotating);
            _log = std::make_shared<spdlog::async_logger>("logs", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            spdlog::register_logger(_log);
            _log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
            _log->set_level(spdlog::level::level_enum::debug);
        }

        virtual void run() override{
        }

        /**
         * @brief Logs an informational message.
         *
         * @param message The message to log.
         */
        virtual void info(const std::string& message) override {
            _log->info(message);
        }

        /**
         * @brief Logs a trace message.
         *
         * @param message The message to log.
         */
        virtual void trace(const std::string& message) override {
            _log->trace(message);
        }

        /**
         * @brief Logs a debug message.
         *
         * @param message The message to log.
         */
        virtual void debug(const std::string& message) override {
            _log->debug(message);
        }

        /**
         * @brief Logs a warning message.
         *
         * @param message The message to log.
         */
        virtual void warn(const std::string& message) override {
            _log->warn(message);
        }

        /**
         * @brief Logs an error message.
         *
         * @param message The message to log.
         */
        virtual void error(const std::string& message) override {
            _log->error(message);
        }

        /**
         * @brief Logs a critical message.
         *
         * @param message The message to log.
         */
        virtual void critical(const std::string& message) override {
            _log->critical(message);
        }

        /**
         * @brief Stops the logging module.
         */
        virtual void stop() override {
            // No specific stop implementation required for spdlog.
        }
    };

    /**
     * @brief Creates a new Spdlog_logger module.
     *
     * @return A shared pointer to the created Spdlog_logger module.
     */
    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Spdlog_logger>());
    }

} // namespace gb
