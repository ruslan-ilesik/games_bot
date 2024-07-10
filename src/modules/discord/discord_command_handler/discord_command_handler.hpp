//
// Created by ilesik on 7/10/24.
//

#pragma once
#include "../../../module/module.hpp"
#include "../discord/discord_command.hpp"

namespace gb {

    class Discord_command_handler: public Module {
    public:

        Discord_command_handler(const std::string& name, const std::vector<std::string>& dependencies) : Module(name,dependencies){};

        virtual void set_bulk(bool value) = 0;

        virtual bool is_bulk() = 0;

        /**
         * @brief Inherited from module method
         * @param modules A map of module names to module shared pointers.
         */
        virtual void innit(const Modules &modules) = 0;

        /**
         * @brief Stops the Discord_command_handler module.
         */
        virtual void stop() = 0;

        /**
         * @brief Runs the Discord_command_handler module.
         */
        virtual void run() = 0;

        virtual void register_command(const Discord_command_ptr& command) = 0;
    };

    typedef std::shared_ptr<Discord_command_handler> Discord_command_handler_ptr;

    /**
     * @brief Function to create a new instance of the Discord_command_handler module.
     * @return A shared pointer to the newly created Discord_command_handler module.
     */
    extern "C" Module_ptr create();

} // gb

