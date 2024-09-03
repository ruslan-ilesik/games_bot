//
// Created by ilesik on 8/30/24.
//

#include "discord_general_command.hpp"

namespace gb {
    Discord_general_command::Discord_general_command(const std::string &name,
                                                     const std::vector<std::string> &dependencies) :
        Module(name, dependencies) {
        for (auto &i: std::vector<std::string>({"discord_bot","discord_command_handler","discord"})) {
            if (std::find(_dependencies.begin(), _dependencies.end(), i) == _dependencies.end()) {
                _dependencies.push_back(i);
            }
        }
    }

    void Discord_general_command::command_start() { ++_cnt; }

    void Discord_general_command::command_end() {
        --_cnt;
        _cv.notify_all();
    }

    void Discord_general_command::stop() {
        std::mutex m;
        std::unique_lock lk(m);
        _cv.wait(lk, [this]() { return _cnt == 0; });
    }
    void Discord_general_command::init(const Modules &modules) {
        this->_bot = std::static_pointer_cast<Discord_bot>(modules.at("discord_bot"));
        this->_command_handler = std::static_pointer_cast<Discord_command_handler>(
            modules.at("discord_command_handler"));
        this-> _discord = std::static_pointer_cast<Discord>(modules.at("discord"));
    }
} // gb