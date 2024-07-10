//
// Created by ilesik on 7/9/24.
//

#include "discord_logging.hpp"
#include "../discord_bot/discord_bot.hpp"

namespace gb {
    class Discord_logging_imp : public Discord_logging{

    private:
        /**
         * @brief A shared pointer to the Discord_bot module.
         */
        Discord_bot_ptr _discord_bot;

        /**
         * @brief A shared pointer to the Logging module.
         */
        Logging_ptr _log;

        /**
         * @brief Handle for the bot's log event.
         */
        dpp::event_handle _bot_log_handle;
    public:

        /**
         * @brief Constructs a Discord_logging_imp object.
         */
        Discord_logging_imp() : Discord_logging("discord_logging", {"logging", "discord_bot"}) {}

        /**
         * @brief Destructor for Discord_logging_imp object.
         */
        virtual ~Discord_logging_imp() = default;

        /**
         * @brief Stops the Discord_logging_imp module.
         * This method detaches the logging handler from the bot's log events.
         */
        void stop() override{
            _discord_bot->get_bot()->on_log.detach(_bot_log_handle);
        }

        /**
         * @brief Runs the Discord_logging_imp module.
         */
        void run() override{

        }

        /**
         * @brief Initializes the Discord_logging module with the provided modules.
         *
         * This method sets up the dependencies on the Discord_bot and Logging modules,
         * and attaches a logging handler to the bot's log events.
         *
         * @param modules A map of module names to module shared pointers.
         */
        void innit(const Modules &modules) override{
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

            _discord_bot->add_pre_requirement([this,log_fn]() {
                _bot_log_handle = _discord_bot->get_bot()->on_log(log_fn);
            });
        }
    };

    /**
     * @brief Function to create a new instance of the Discord_logging_imp module.
     * @return A shared pointer to the newly created Discord_logging_imp module.
     */
    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_logging_imp>());
    }
} // gb