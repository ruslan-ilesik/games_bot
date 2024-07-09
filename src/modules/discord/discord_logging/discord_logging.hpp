//
// Created by ilesik on 7/9/24.
//

#pragma once

#include <dpp/dpp.h>
#include "../../../module/module.hpp"
#include "../discord_bot/discord_bot.hpp"
#include "../../logging/logging.hpp"

namespace gb {

    /**
     * @class Discord_logging
     * @brief A class representing a module for logging Discord bot events.
     *
     * This class inherits from the Module class and integrates logging functionality
     * with a Discord bot using the DPP library.
     */
    class Discord_logging : public Module {
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
         * @brief Constructs a Discord_logging object.
         */
        Discord_logging();

        /**
         * @brief Destructor for Discord_logging object.
         */
        virtual ~Discord_logging() = default;

        /**
         * @brief Initializes the Discord_logging module with the provided modules.
         *
         * This method sets up the dependencies on the Discord_bot and Logging modules,
         * and attaches a logging handler to the bot's log events.
         *
         * @param modules A map of module names to module shared pointers.
         */
        virtual void innit(const Modules &modules);

        /**
         * @brief Stops the Discord_logging module.
         * This method detaches the logging handler from the bot's log events.
         */
        virtual void stop();

        /**
         * @brief Runs the Discord_logging module.
         */
        virtual void run();
    };

    /**
     * @typedef Discord_logging_ptr
     * @brief A shared pointer type for Discord_logging.
     */
    typedef std::shared_ptr<Discord_logging> Discord_logging_ptr;

    /**
     * @brief Function to create a new instance of the Discord_logging module.
     * @return A shared pointer to the newly created Discord_logging module.
     */
    extern "C" Module_ptr create();

} // gb
