//
// Created by ilesik on 7/9/24.
//

#include "discord_bot.hpp"
#include "../../config/config.hpp"


namespace gb {

    class Discord_bot_impl : public Discord_bot {
    private:
        Config_ptr config;
    public:
        Discord_bot_impl() : Discord_bot("discord_bot", {"config"}) {};

        ~Discord_bot_impl() {
            // memory clean up to not have memory leak, originally created in innit method
            delete bot;

        }

        virtual void innit(const Modules &modules) {
            config = std::static_pointer_cast<Config>(modules.at("config"));
        }

        virtual void run() {
            if (bot != nullptr) {
                throw std::runtime_error("Bot variable is not nullptr, memory leak possible");
            }
            bot = new dpp::cluster(config->get_value("discord_bot_token"));
            bot->start(dpp::st_return);
        }

        virtual void stop() {
            if (bot == nullptr) {
                throw std::runtime_error("Bot is nullptr, no way to stop it");
            }
            bot->shutdown();
        }

    };

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_bot_impl>());
    }

} // gb