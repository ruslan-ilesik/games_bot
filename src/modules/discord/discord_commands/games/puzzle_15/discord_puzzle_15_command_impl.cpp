//
// Created by ilesik on 10/14/24.
//

#include "discord_puzzle_15_command_impl.hpp"

namespace gb {
    Discord_puzzle_15_command_impl::Discord_puzzle_15_command_impl() :
        Discord_puzzle_15_command("discord_puzzle_15_command", {"premium_manager"}) {
        lobby_title = "puzzle 15";
        lobby_description =
            "The 15 Puzzle (also called Gem Puzzle, Boss Puzzle, Game of Fifteen, Mystic Square and more) is a sliding "
            "puzzle. It has 15 square tiles numbered 1 to 15 in a frame that is 4 tile positions high and 4 tile "
            "positions wide, with one unoccupied position. Tiles in the same row or column of the open position can be "
            "moved by sliding them horizontally or vertically, respectively. The goal of the puzzle is to place the "
            "tiles in numerical order (from left to right, top to bottom).";

        lobby_image_url = "https://i.ibb.co/MBqBBM1/15-puzzle-magical-svg.png";
    }

    void Discord_puzzle_15_command_impl::init(const Modules &modules) {
        Discord_puzzle_15_command::init(modules);
        _premium = std::static_pointer_cast<Premium_manager>(modules.at("premium_manager"));

    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_puzzle_15_command_impl>());
    };
} // namespace gb
