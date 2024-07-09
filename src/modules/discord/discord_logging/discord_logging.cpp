//
// Created by ilesik on 7/9/24.
//

#include "discord_logging.hpp"

namespace gb {
    Discord_logging::Discord_logging() : Module("discord_logging", {"logging", "discord_bot"}) {}

    void Discord_logging::stop() {
        _discord_bot->get_bot()->on_log.detach(_bot_log_handle);
    }

    void Discord_logging::run() {

    }

    void Discord_logging::innit(const Modules &modules) {
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
        if (_discord_bot->get_bot() == nullptr){
            _discord_bot->pre_requirements.emplace_back([this,log_fn]() {

            });
        }
        else{
            _bot_log_handle = _discord_bot->get_bot()->on_log(log_fn);
        };

    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_logging>());
    }
} // gb