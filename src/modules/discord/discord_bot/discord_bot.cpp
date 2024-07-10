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
         * @brief A list of function which should called after module innit but before bot start. cleared and ignored after start
         */
        std::vector<std::function<void()>> _pre_requirements;

    public:
        /**
         * @brief Constructs a Discord_bot_impl object.
         */
        Discord_bot_impl() : Discord_bot("discord_bot", {"config"}) {};

        /**
         * @brief Destructor for the Discord_bot_impl class.
         */
        ~Discord_bot_impl() override {
            // Memory cleanup to prevent memory leak, originally created in innit method.
            delete _bot;
        }

        /**
         * @brief Initializes the Discord_bot_impl with the provided modules.
         * @param modules A map of module names to module pointers.
         */
        void innit(const Modules &modules) override {
            _config = std::static_pointer_cast<Config>(modules.at("config"));
        }

        /**
         * @brief Starts the Discord bot.
         * Throws an exception if the bot is already initialized, indicating a potential memory leak.
         */
        void run() override{
            if (_bot != nullptr) {
                throw std::runtime_error("Bot variable is not nullptr, memory leak possible");
            }
            _bot = new Discord_cluster(_config->get_value("discord_bot_token"));

            //run all pre requirements.
            {
                std::unique_lock<std::mutex> lock(_mutex);
                for (auto& i : _pre_requirements){
                    i();
                }
            }
            _bot->start(dpp::st_return);
        }

        /**
         * @brief Stops the Discord bot.
         * Throws an exception if the bot is not initialized, indicating that there is no bot to stop.
         */
        void stop() override{
            if (_bot == nullptr) {
                throw std::runtime_error("Bot is nullptr, no way to stop it");
            }
            _bot->shutdown();
        }

        /**
         * @brief Getter for bot.
         * @return dpp::cluster* bot pointer.
         */
        Discord_cluster* get_bot() override{
            return _bot;
        }

        /**
         * @brief Adds a pre-requirement function to be executed before the bot is initialized or execute immediately if bot initialized.
         */
        void add_pre_requirement(const std::function<void()> &func) override{
            std::unique_lock<std::mutex> lock(_mutex);
            if (_bot == nullptr) {
                _pre_requirements.push_back(func);
            } else {
                func();
            }
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
