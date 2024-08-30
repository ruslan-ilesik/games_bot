//
// Created by ilesik on 7/12/24.
//

#pragma once

#include "./discord_command_ping.hpp"
#include "../../../discord_command_handler/discord_command_handler.hpp"
#include "../../../discord_bot/discord_bot.hpp"

namespace gb {

    /**
     * @class Discord_command_ping_impl
     * @brief Implementation of the ping command for Discord.
     *
     * This class implements a ping command for Discord, which checks the latency between
     * the bot and various components, and provides a response with the latency information.
     */
    class Discord_command_ping_impl : public Discord_command_ping {
    private:
        /**
         * @brief Shared pointer to the Discord command handler.
         */
        Discord_command_handler_ptr _command_handler;

        /**
         * @brief Shared pointer to the Discord bot.
         */
        Discord_bot_ptr _discord_bot;

    public:
        /**
         * @brief Constructor for Discord_command_ping_impl.
         */
        Discord_command_ping_impl();

        /**
         * @brief Runs the ping command.
         *
         * This method is intended to start any necessary operations for the ping command.
         */
        void run() override;

        /**
         * @brief Initializes the ping command.
         *
         * This method initializes the ping command by setting up the necessary modules and
         * registering the command with the Discord bot.
         *
         * @param modules A reference to the collection of modules required by this command.
         */
        void init(const Modules &modules) override;
    };

    /**
     * @brief Function to create a new instance of the Discord_command_ping_impl module.
     * @return A shared pointer to the newly created Discord_command_ping_impl module.
     */
    extern "C" Module_ptr create();

} // namespace gb