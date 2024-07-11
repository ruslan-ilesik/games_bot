//
// Created by ilesik on 7/9/24.
//

#pragma once

#include "discord_bot.hpp"
#include "../../config/config.hpp"
#include <mutex>
#include <vector>
#include <functional>

namespace gb {

    /**
     * @class Discord_bot_impl
     * @brief Implementation of the Discord_bot class.
     * This class implements the Discord_bot module, providing methods to initialize,
     * run, and stop the Discord bot using the DPP library.
     */
    class Discord_bot_impl : public Discord_bot {
    private:
        /**
         * @brief A shared pointer to the Config module.
         */
        Config_ptr _config;

        /**
         * @brief Mutex for synchronization of operations.
         */
        std::mutex _mutex;

        /**
         * @brief A pointer to the object representing the bot.
         */
        Discord_cluster *_bot = nullptr;

        /**
         * @brief A list of functions to be called after module init but before bot start. Cleared and ignored after start.
         */
        std::vector<std::function<void()>> _pre_requirements;

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
        void innit(const Modules &modules) override;

        /**
         * @brief Starts the Discord bot.
         * Throws an exception if the bot is already initialized, indicating a potential memory leak.
         */
        void run() override;

        /**
         * @brief Stops the Discord bot.
         * Throws an exception if the bot is not initialized, indicating that there is no bot to stop.
         */
        void stop() override;

        /**
         * @brief Getter for bot.
         * @return dpp::cluster* bot pointer.
         */
        Discord_cluster* get_bot() override;

        /**
         * @brief Adds a pre-requirement function to be executed before the bot is initialized or execute immediately if bot initialized.
         */
        void add_pre_requirement(const std::function<void()> &func) override;
    };

    /**
     * @brief Function to create a new instance of the Discord_bot_impl module.
     * @return A shared pointer to the newly created Discord_bot_impl module.
     */
    Module_ptr create();

} // gb