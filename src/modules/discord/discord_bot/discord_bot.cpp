//
// Created by ilesik on 7/9/24.
//

#include "discord_bot.hpp"
#include "../../config/config.hpp"

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
        Config_ptr config;

    public:
        /**
         * @brief Constructs a Discord_bot_impl object.
         */
        Discord_bot_impl() : Discord_bot("discord_bot", {"config"}) {};

        /**
         * @brief Destructor for the Discord_bot_impl class.
         */
        ~Discord_bot_impl() {
            // Memory cleanup to prevent memory leak, originally created in innit method.
            delete bot;
        }

        /**
         * @brief Initializes the Discord_bot_impl with the provided modules.
         * @param modules A map of module names to module pointers.
         */
        virtual void innit(const Modules &modules) override {
            config = std::static_pointer_cast<Config>(modules.at("config"));
        }

        /**
         * @brief Starts the Discord bot.
         * Throws an exception if the bot is already initialized, indicating a potential memory leak.
         */
        virtual void run() override {
            if (bot != nullptr) {
                throw std::runtime_error("Bot variable is not nullptr, memory leak possible");
            }
            bot = new dpp::cluster(config->get_value("discord_bot_token"));
            bot->start(dpp::st_return);
        }

        /**
         * @brief Stops the Discord bot.
         * Throws an exception if the bot is not initialized, indicating that there is no bot to stop.
         */
        virtual void stop() override {
            if (bot == nullptr) {
                throw std::runtime_error("Bot is nullptr, no way to stop it");
            }
            bot->shutdown();
        }
    };

    /**
     * @brief Function to create a new instance of the Discord_bot_impl module.
     * @return A shared pointer to the newly created Discord_bot_impl module.
     */
    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_bot_impl>());
    }

} // gb
