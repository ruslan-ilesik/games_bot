//
// Created by ilesik on 7/9/24.
//

#pragma once

#include <functional>
#include <mutex>
#include <vector>
#include "../../config/config.hpp"
#include "discord_bot.hpp"

#include <src/modules/database/database.hpp>

namespace gb {

    /**
     * @class Discord_bot_impl
     * @brief Implementation of the Discord_bot class.
     * This class implements the Discord_bot module, providing methods to
     * initialize, run, and stop the Discord bot using the DPP library.
     */
    class Discord_bot_impl : public Discord_bot {
    private:
        /**
         * @brief A shared pointer to the Config module.
         */
        Config_ptr _config;

        /**
         * @brief  A shared pointer to the Database module.
         */
        Database_ptr _db;

        /**
         * @brief Mutex for synchronization of operations.
         */
        std::mutex _mutex;

        /**
         * @brief Database backup dpp timer which runs once per day.
         */
        dpp::timer _db_backup_timer;

        /**
         * @brief A pointer to the object representing the bot.
         */
        std::unique_ptr<Discord_cluster> _bot;

        /**
         * @brief A list of functions to be called after module init but before bot
         * start. Cleared and ignored after start.
         */
        std::vector<std::function<void()>> _pre_requirements;

        /**
         * @brief Function to preprocess message before it will be sent to discord.
         * @param message Message to preprocess, such as by default add to all embeds
         * timestamp.
         * @return New edited message.
         */
        dpp::message message_preprocessing(dpp::message message);

    public:
        /**
         * @brief Constructs a Discord_bot_impl object.
         */
        Discord_bot_impl();

        /**
         * @brief Destructor for the Discord_bot_impl class.
         */
        ~Discord_bot_impl() override;

        /**
         * @brief Initializes the Discord_bot_impl with the provided modules.
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Starts the Discord bot.
         * Throws an exception if the bot is already initialized, indicating a
         * potential memory leak.
         */
        void run() override;

        /**
         * @brief Stops the Discord bot.
         * Throws an exception if the bot is not initialized, indicating that there is
         * no bot to stop.
         */
        void stop() override;

        /**
         * @brief Getter for bot.
         * @return dpp::cluster* bot pointer.
         */
        Discord_cluster *get_bot() override;

        /**
         * @brief Adds a pre-requirement function to be executed before the bot is
         * initialized or execute immediately if bot initialized.
         */
        void add_pre_requirement(const std::function<void()> &func) override;

        /**
         * @brief Sends a reply to a slash command event in Discord.
         *
         * This function is used to send a response message to a slash command event.
         * It allows you to provide a custom message and optionally handle the
         * completion of the command with a callback function.
         *
         * @param event The slash command event that triggered the reply.
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses 'dpp::utility::log_error' to log any
         * errors that occur during the execution of the command. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void reply(const dpp::slashcommand_t &event, const dpp::message &message,
                   const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Sends a reply to a select click event in Discord.
         *
         * This function is used to send a response message to a select click event.
         * It allows you to provide a custom message and optionally handle the
         * completion of the command with a callback function.
         *
         * @param event The select click event that triggered the reply.
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the command. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void reply(const dpp::select_click_t &event, const dpp::message &message,
                   const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Sends a reply to a button click event in Discord.
         *
         * This function is used to send a response message to a button click event.
         * It allows you to provide a custom message and optionally handle the
         * completion of the command with a callback function.
         *
         * @param event The button click event that triggered the reply.
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the command. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void reply(const dpp::button_click_t &event, const dpp::message &message,
                   const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Sends new message as a reply to a button click event in Discord.
         *
         * This function is used to send new message response message to a button
         * click event. It allows you to provide a custom message and optionally
         * handle the completion of the command with a callback function.
         *
         * @param event The button click event that triggered the reply.
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the command. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void reply_new(const dpp::button_click_t &event, const dpp::message &message,
                       const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;


        /**
         * @brief Edits given message.
         *
         * This function is used to edit given message
         * It allows you to provide a custom message and optionally
         * handle the completion of the command with a callback function.
         *
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the function. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void message_edit(const dpp::message &message,
                          const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Sends given message.
         *
         * This function is used to send given message to channel id specified in it.
         * It allows you to provide a custom message and optionally
         * handle the completion of the command with a callback function.
         *
         * @param message The message to be sent.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the function. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        void message_create(const dpp::message &message,
                            const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Edits the original response of a slash command interaction.
         *
         * This function modifies the original response of a slash command interaction.
         * You can provide a new message to replace the previous response and optionally
         * supply a callback to handle the completion of the request.
         *
         * @param event The slash command interaction containing details of the command.
         * @param m The new message content to replace the original response.
         * @param callback Optional. A callback function to handle the completion of the request.
         * The default is `dpp::utility::log_error`. You can provide a custom callback
         * for additional processing or error handling.
         */
        void event_edit_original_response(
            const dpp::slashcommand_t &event, const dpp::message &m,
            const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Edits the original response of a button click interaction.
         *
         * This function modifies the original response of a button click interaction.
         * A new message can be supplied to replace the existing response. A callback
         * function can also be provided to handle the completion of the request.
         *
         * @param event The button click interaction containing details of the event.
         * @param m The new message content to replace the original response.
         * @param callback Optional. A callback function to handle the completion of the request.
         * The default is `dpp::utility::log_error`. You can provide a custom callback
         * for additional processing or error handling.
         */
        void event_edit_original_response(
            const dpp::button_click_t &event, const dpp::message &m,
            const dpp::command_completion_event_t &callback = dpp::utility::log_error()) override;

        /**
         * @brief Sends a direct message to a specified user asynchronously.
         *
         * This coroutine function is used to send a direct message to a specified user
         * (identified by their user ID). It handles the message processing internally
         * before dispatching the message.
         *
         * @param user_id The Discord snowflake (unique identifier) of the user to whom
         * the direct message will be sent.
         * @param message The message object to be sent.
         *
         * @return dpp::task<dpp::confirmation_callback_t> This asynchronous task returns
         * a confirmation callback when the message has been sent. The confirmation
         * callback contains information about the success or failure of the message delivery.
         */
        dpp::task<dpp::confirmation_callback_t> co_direct_message_create(const dpp::snowflake &user_id,
                                                                         dpp::message &message) override;
    };

    /**
     * @brief Function to create a new instance of the Discord_bot_impl module.
     * @return A shared pointer to the newly created Discord_bot_impl module.
     */
    Module_ptr create();

} // namespace gb
