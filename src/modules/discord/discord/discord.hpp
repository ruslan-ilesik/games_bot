//
// Created by ilesik on 7/10/24.
//

#pragma once
#include "./discord_command.hpp"
#include "../../../module/module.hpp"

namespace gb {

    //This is basically interface class for constructing all discord types custom wrapped to reduce compilation time of other modules
    class Discord: public Module {
    public:
        Discord(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};

        /**
        * @brief Destructor for the Discord class.
        * Virtual destructor to allow proper cleanup of derived class objects.
        */
        ~Discord() override = default;

        /**
         * @brief Inherited from module method
         * @param modules A map of module names to module shared pointers.
         */
         void innit(const Modules &modules) override  = 0;

        /**
         * @brief Stops the Discord module.
         */
        void stop() override = 0;

        /**
         * @brief Runs the Discord module.
         */
        void run() override  = 0;

        virtual Discord_command_ptr create_discord_command(const dpp::slashcommand &command, const slash_command_handler_t &handler,
                                                       const Command_data &command_data) = 0;

    };

    /**
    * @typedef Discord_ptr
    * @brief A shared pointer type for Discord.
    */
    typedef std::shared_ptr<Discord> Discord_ptr;

    /**
     * @brief Creates a new instance of the Discord module.
     * @return A shared pointer to the newly created Discord module.
     */
    extern "C" Module_ptr create();
} // gb
