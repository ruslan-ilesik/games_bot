//
// Created by ilesik on 9/18/24.
//

#pragma once

#include "./discord_rubiks_cube_command.hpp"

namespace gb {

    /**
     * @class Discord_rubiks_cube_command_impl
     * @brief Implements the Rubik's Cube game command for the Discord bot.
     *
     * This class handles the setup, execution, and teardown of the Rubik's Cube game command
     * in a Discord environment. It inherits from the Discord_rubiks_cube_command base class
     * and overrides the necessary methods such as `run`, `init`, and `stop`.
     */
    class Discord_rubiks_cube_command_impl : public Discord_rubiks_cube_command {

    protected:
        /**
         * Overrides command callback with proper handler.
         * @param event slashcommand event forwarded to handler.
         * @return nothing
         */
        dpp::task<void> _command_callback(const dpp::slashcommand_t &event) override;

    public:
        /**
         * @brief Constructs a Discord_rubiks_cube_command_impl object.
         *
         * Initializes the Rubik's Cube command with the required dependencies.
         */
        Discord_rubiks_cube_command_impl();

        /**
        * @breif Define destructor.
        */
        virtual ~Discord_rubiks_cube_command_impl() = default;

        /**
         * @brief Empty function to implement abstract class.
         *
         */
        void run() override;

        /**
         * @brief Initializes the command with the necessary modules.
         *
         * This method sets up the command handler, required modules, and other resources
         * necessary for the Rubik's Cube game to run. It also registers the game command
         * with the Discord bot.
         *
         * @param modules A map of module names to module pointers.
         */
        void init(const Modules &modules) override;

        /**
         * @brief Stops the command and performs cleanup.
         *
         * This method removes the Rubik's Cube game command from the command handler
         * and performs necessary cleanup tasks such as deallocating resources.
         */
        void stop() override;
    };

    /**
     * @brief Factory function to create a Discord_rubiks_cube_command_impl module.
     *
     * @return A shared pointer to the created module.
     */
    extern "C" Module_ptr create();

} // namespace gb
