//
// Created by ilesik on 7/3/24.
//

#include "logging_impl.hpp"

namespace gb {

    Logging_impl::Logging_impl() : Logging("logging", {}) {}

    void Logging_impl::innit(const Modules& modules) {
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

    void Logging_impl::run() {
        // No specific action needed for run in this implementation.
    }

    void Logging_impl::info(const std::string& message) {
        _log->info(message);
    }

    void Logging_impl::trace(const std::string& message) {
        _log->trace(message);
    }

    void Logging_impl::debug(const std::string& message) {
        _log->debug(message);
    }

    void Logging_impl::warn(const std::string& message) {
        _log->warn(message);
    }

    void Logging_impl::error(const std::string& message) {
        _log->error(message);
    }

    void Logging_impl::critical(const std::string& message) {
        _log->critical(message);
    }

    void Logging_impl::stop() {
        // No specific stop implementation required for spdlog.
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Logging_impl>());
    }

} // namespace gb
