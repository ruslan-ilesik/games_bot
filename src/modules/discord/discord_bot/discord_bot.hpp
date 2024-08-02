//
// Created by ilesik on 7/9/24.
//

#pragma once

#include <dpp/dpp.h>
#include "./discord_cluster.hpp"
#include "../../../module/module.hpp"

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
         * @param dependencies A vector of strings representing the dependencies of the module.
         */
        Discord_bot(const std::string &name, const std::vector<std::string> &dependencies)
            : Module(name, dependencies) {};

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
         * @brief Adds a pre-requirement function to be executed before the bot is initialized or execute immediately if bot initialized.
         */
        virtual void add_pre_requirement(const std::function<void()> &func) = 0;

        /**
         * @brief Sends a reply to a slash command event in Discord.
         *
         * This function is used to send a response message to a slash command event. It allows you to provide a custom
         * message and optionally handle the completion of the command with a callback function.
         *
         * @param event The slash command event that triggered the reply.
         * @param message The message to be sent as a reply.
         * @param callback Optional. The callback function to handle the completion of the command. By default, it uses
         *                 `dpp::utility::log_error` to log any errors that occur during the execution of the command.
         *                 You can provide a custom callback function to handle the completion event as needed.
         */
        virtual void reply(const dpp::slashcommand_t& event, const dpp::message& message, const dpp::command_completion_event_t& callback = dpp::utility::log_error()) = 0;

        /**
        * @brief Sends a reply to a select click event in Discord.
        *
        * This function is used to send a response message to a select click event. It allows you to provide a custom
        * message and optionally handle the completion of the command with a callback function.
        *
        * @param event The select click event that triggered the reply.
        * @param message The message to be sent as a reply.
        * @param callback Optional. The callback function to handle the completion of the command. By default, it uses
        *                 `dpp::utility::log_error` to log any errors that occur during the execution of the command.
        *                 You can provide a custom callback function to handle the completion event as needed.
        */
        virtual void reply(const dpp::select_click_t& event, const dpp::message& message, const dpp::command_completion_event_t& callback = dpp::utility::log_error()) = 0;


        /**
        * @brief Sends a reply to a button click event in Discord.
        *
        * This function is used to send a response message to a button click event. It allows you to provide a custom
        * message and optionally handle the completion of the command with a callback function.
        *
        * @param event The button click event that triggered the reply.
        * @param message The message to be sent as a reply.
        * @param callback Optional. The callback function to handle the completion of the command. By default, it uses
        *                 `dpp::utility::log_error` to log any errors that occur during the execution of the command.
        *                 You can provide a custom callback function to handle the completion event as needed.
        */
        virtual void reply(const dpp::button_click_t& event, const dpp::message& message, const dpp::command_completion_event_t& callback = dpp::utility::log_error()) = 0;

        /**
        * @brief Sends new message as a reply to a button click event in Discord.
        *
        * This function is used to send new message response message to a button click event. It allows you to provide a custom
        * message and optionally handle the completion of the command with a callback function.
        *
        * @param event The button click event that triggered the reply.
        * @param message The message to be sent as a reply.
        * @param callback Optional. The callback function to handle the completion of the command. By default, it uses
        *                 `dpp::utility::log_error` to log any errors that occur during the execution of the command.
        *                 You can provide a custom callback function to handle the completion event as needed.
        */
        virtual void reply_new(const dpp::button_click_t& event, const dpp::message& message, const dpp::command_completion_event_t& callback = dpp::utility::log_error()) = 0;
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
} // gb


