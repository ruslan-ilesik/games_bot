//
// Created by ilesik on 7/9/24.
//

#pragma  once
#include <dpp/dpp.h>
#include "../../../module/module.hpp"


namespace gb {

    class Discord_bot: public Module{
    public:
        dpp::cluster* bot = nullptr;

        Discord_bot(const std::string& name, const std::vector<std::string>& dependencies): Module(name,dependencies){};

        virtual ~Discord_bot() = default;
    };

    typedef std::shared_ptr<Discord_bot> Discord_bot_ptr;

    extern "C" Module_ptr create();
} // gb


