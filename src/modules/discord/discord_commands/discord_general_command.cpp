//
// Created by ilesik on 8/30/24.
//

#include "discord_general_command.hpp"

namespace gb {
    Discord_general_command::Discord_general_command(const std::string &name,
                                                     const std::vector<std::string> &dependencies) :
        Module(name, dependencies) {}

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
} // gb