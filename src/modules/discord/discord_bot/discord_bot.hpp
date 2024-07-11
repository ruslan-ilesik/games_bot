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


