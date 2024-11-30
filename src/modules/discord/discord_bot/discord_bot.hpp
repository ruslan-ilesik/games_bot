//
// Created by ilesik on 7/9/24.
//

#pragma once

#include <dpp/dpp.h>
#include "../../../module/module.hpp"
#include "./discord_cluster.hpp"

namespace gb {

    /**
     * @class Discord_bot
     * @brief A class representing a Discord bot module.
     * This class inherits from the Module class and encapsulates the functionality
     * for interacting with the Discord API through the DPP library.
     */
    class Discord_bot : public Module {
    public:
        /**
         * @brief Constructs a Discord_bot object.
         * @param name The name of the module.
         * @param dependencies A vector of strings representing the dependencies of
         * the module.
         */
        Discord_bot(const std::string &name, const std::vector<std::string> &dependencies) :
            Module(name, dependencies){};

        /**
         * @brief Destructor for the Discord_bot class.
         * Virtual destructor to allow proper cleanup of derived class objects.
         */
        virtual ~Discord_bot() = default;

        /**
         * @brief Getter for bot.
         * @return Discord_cluster* bot pointer.
         */
        virtual Discord_cluster *get_bot() = 0;

        /**
         * @brief Adds a pre-requirement function to be executed before the bot is
         * initialized or execute immediately if bot initialized.
         */
        virtual void add_pre_requirement(const std::function<void()> &func) = 0;

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
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the command. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        virtual void reply(const dpp::slashcommand_t &event, const dpp::message &message,
                           const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

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
        virtual void reply(const dpp::select_click_t &event, const dpp::message &message,
                           const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

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
        virtual void reply(const dpp::button_click_t &event, const dpp::message &message,
                           const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

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
         * errors that occur during the execution of the function. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        virtual void reply_new(const dpp::button_click_t &event, const dpp::message &message,
                               const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

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
        virtual void message_edit(const dpp::message &message,
                                  const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

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
        virtual void message_create(const dpp::message &message,
                                    const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

        /**
         * @brief Edits the original response of a slash command event.
         *
         * This function is used to edit the original response to a slash command event.
         * It allows you to modify the original message sent as a response to the user
         * after a slash command interaction. You can also handle the completion of the
         * command with a callback function.
         *
         * @param event The slash command event containing interaction details.
         * @param m The message to be used for editing the original response.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the function. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        virtual void
        event_edit_original_response(const dpp::slashcommand_t &event, const dpp::message &m,
                                     const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;

        /**
         * @brief Edits the original response of a button click event.
         *
         * This function is used to edit the original response to a button click event.
         * It allows you to modify the message originally sent in response to a button
         * interaction. You can also handle the completion of the command with a
         * callback function.
         *
         * @param event The button click event containing interaction details.
         * @param m The message to be used for editing the original response.
         * @param callback Optional. The callback function to handle the completion of
         * the command. By default, it uses `dpp::utility::log_error` to log any
         * errors that occur during the execution of the function. You can provide a
         * custom callback function to handle the completion event as needed.
         */
        virtual void
        event_edit_original_response(const dpp::button_click_t &event, const dpp::message &m,
                                     const dpp::command_completion_event_t &callback = dpp::utility::log_error()) = 0;


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
        virtual dpp::task<dpp::confirmation_callback_t> co_direct_message_create(const dpp::snowflake &user_id,
                                                                                 dpp::message &message) = 0;
    };

    /**
     * @typedef Discord_bot_ptr
     * @brief A shared pointer type for Discord_bot.
     */
    typedef std::shared_ptr<Discord_bot> Discord_bot_ptr;

    /**
     * @brief Creates a new instance of the Discord_bot module.
     * @return A shared pointer to the newly created Discord_bot module.
     */
    extern "C" Module_ptr create();
} // namespace gb
