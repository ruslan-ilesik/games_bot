//
// Created by ilesik on 7/9/24.
//

#include "discord_logging_impl.hpp"


namespace gb {

    Discord_logging_impl::Discord_logging_impl()
        : Discord_logging("discord_logging", {"logging", "discord_bot"}) {}

    void Discord_logging_impl::stop() {
        _discord_bot->get_bot()->on_log.detach(_bot_log_handle);
    }

    void Discord_logging_impl::run() {
        // No operation needed in run() for this implementation
    }

    void Discord_logging_impl::init(const Modules &modules) {
        this->_discord_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_log = std::static_pointer_cast<Logging>(modules.at("logging"));

        auto log_fn = [this](const dpp::log_t &event) {
            std::string message = "Discord bot: " + event.message;
            switch (event.severity) {
                case dpp::ll_trace:
                    _log->trace(message);
                    break;
                case dpp::ll_debug:
                    _log->debug(message);
                    break;
                case dpp::ll_info:
                    _log->info(message);
                    break;
                case dpp::ll_warning:
                    _log->warn(message);
                    break;
                case dpp::ll_error:
                    _log->error(message);
                    break;
                case dpp::ll_critical:
                default:
                    _log->critical(message);
                    break;
            }
        };

        _discord_bot->add_pre_requirement([this, log_fn]() {
            _bot_log_handle = _discord_bot->get_bot()->on_log(log_fn);
        });
    }

    /**
     * @brief Function to create a new instance of the Discord_logging_imp module.
     * @return A shared pointer to the newly created Discord_logging_imp module.
     */
    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_logging_impl>());
    }

} // gb
