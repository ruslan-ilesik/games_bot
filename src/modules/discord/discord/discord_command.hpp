//
// Created by ilesik on 7/10/24.
//

#pragma once

#include <dpp/dpp.h>
#include<functional>
#include <string>

namespace gb {

    typedef std::function<void(const dpp::slashcommand_t &)> slash_command_handler_t;

    struct Command_data {
        std::string help_text;//categories one by one nested in each other. for example. {"game","single player"};

        std::vector<std::string> category;

    };

    class Discord_command {

    public:
        virtual ~Discord_command() = default;

        // Getter for _command
        virtual dpp::slashcommand get_command() const = 0;

        // Getter for _handler
        virtual slash_command_handler_t get_handler() const = 0;

        // Getter for _command_data
        virtual Command_data get_command_data() const = 0;

        virtual void operator()(const dpp::slashcommand_t & event) = 0;

        virtual std::string get_name() const = 0;
    };

    typedef std::shared_ptr<Discord_command> Discord_command_ptr;

} // gb

