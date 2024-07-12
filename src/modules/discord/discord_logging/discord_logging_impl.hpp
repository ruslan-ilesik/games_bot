//
// Created by ilesik on 7/9/24.
//

#pragma once

#include "discord_logging.hpp"
#include "../discord_bot/discord_bot.hpp"

namespace gb {

    /**
     * @class Discord_logging_imp
     * @brief Implementation of the Discord_logging module.
     *
     * This class implements the Discord_logging module, which handles logging events from a Discord bot.
     * It initializes by setting up dependencies on the Discord_bot and Logging modules, and attaches a logging
     * handler to the bot's log events.
     */
    class Discord_logging_impl : public Discord_logging {
    private:
        Discord_bot_ptr _discord_bot; ///< Pointer to the Discord bot module.
        Logging_ptr _log; ///< Pointer to the logging module.
        dpp::event_handle _bot_log_handle; ///< Handle for the bot's log event.

    public:
        /**
         * @brief Constructs a Discord_logging_imp object.
         */
        Discord_logging_impl();

        /**
         * @brief Destructor for Discord_logging_imp object.
         */
        virtual ~Discord_logging_impl() = default;

        /**
         * @brief Stops the Discord_logging_imp module.
         *
         * Detaches the logging handler from the bot's log events.
         */
        void stop() override;

        /**
         * @brief Runs the Discord_logging_imp module.
         *
         * This method does not perform any operation in this implementation.
         */
        void run() override;

        /**
         * @brief Initializes the Discord_logging module with the provided modules.
         *
         * Sets up dependencies on the Discord_bot and Logging modules, and attaches a logging handler
         * to the bot's log events.
         *
         * @param modules A map of module names to module shared pointers.
         */
        void innit(const Modules &modules) override;

    };

} // gb
