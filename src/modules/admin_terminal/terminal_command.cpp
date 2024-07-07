//
// Created by ilesik on 7/6/24.
//

#include "./terminal_command.hpp"

namespace gb{
    std::string Terminal_command::get_name() const{
        return _name;
    }

    std::string Terminal_command::get_description() const {return _description;}

    std::string Terminal_command::get_help() const {return _help;}

    void Terminal_command::operator()(const std::vector<std::string> &arguments) {
        _callback(arguments);
    }

    Terminal_command::Terminal_command(std::string name, std::string description, std::string help,
                                       Terminal_command_callback callback) {
        this->_name = name;
        this->_description = description;
        this->_callback = callback;
        this->_help = help;
    }

} //gb