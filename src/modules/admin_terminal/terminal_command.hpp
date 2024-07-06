//
// Created by ilesik on 7/6/24.
//

#pragma once

namespace gb {

    class Terminal_command {
    private:
        std::string _name;
        std::string _description;
        std::string _help;
    };

    typedef   std::shared_ptr<Terminal_command>  Terminal_command_ptr;

} // gb

