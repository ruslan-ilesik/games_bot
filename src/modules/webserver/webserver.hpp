//
// Created by ilesik on 10/27/24.
//

#pragma once
#include <src/module/module.hpp>

namespace gb {

    /**
     * @class Webserver
     * @brief Base class for implementing a web server module.
     *
     * The Webserver class provides a foundation for creating modules with web server functionality,
     * managing dependencies, and integrating into the module system.
     */
    class Webserver : public Module {
    public:
        /**
         * @brief Constructs a Webserver instance with a name and its dependencies.
         *
         * @param name The name of the web server module.
         * @param dependencies A list of module names this server depends on.
         */
        Webserver(const std::string &name, const std::vector<std::string> &dependencies) : Module(name, dependencies) {}
    };

} // namespace gb
