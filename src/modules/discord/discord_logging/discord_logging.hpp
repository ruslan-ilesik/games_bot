//
// Created by ilesik on 7/9/24.
//

#pragma once

#include <dpp/dpp.h>
#include "../../../module/module.hpp"
#include "../../logging/logging.hpp"

namespace gb {

    /**
     * @class Discord_logging
     * @brief A class representing a module for logging Discord bot events.
     *
     * This class inherits from the Module class and integrates logging functionality
     * with a Discord bot using the DPP library.
     */
    class Discord_logging : public Module {
    private:

    public:
        /**
         * @brief Constructs a Discord_logging object.
         */
        Discord_logging(const std::string& name, const std::vector<std::string>& dependencies) : Module(name,dependencies){};

        /**
         * @brief Destructor for Discord_logging object.
         */
        virtual ~Discord_logging() = default;

        /**
         * @brief Inherited from module method
         * @param modules A map of module names to module shared pointers.
         */
        virtual void innit(const Modules &modules) = 0;

        /**
         * @brief Stops the Discord_logging module.
         */
        virtual void stop() = 0;

        /**
         * @brief Runs the Discord_logging module.
         */
        virtual void run() = 0;
    };

    /**
     * @typedef Discord_logging_ptr
     * @brief A shared pointer type for Discord_logging.
     */
    typedef std::shared_ptr<Discord_logging> Discord_logging_ptr;

    /**
     * @brief Function to create a new instance of the Discord_logging module.
     * @return A shared pointer to the newly created Discord_logging module.
     */
    extern "C" Module_ptr create();

} // gb
