//
// Created by ilesik on 7/31/24.
//

#include "discord_tic_tac_toe_command_impl.hpp"

namespace gb {
    Discord_tic_tac_toe_command_impl::Discord_tic_tac_toe_command_impl() : Discord_tic_tac_toe_command("discord_game_tic_tac_toe",{"image_processing","discord_button_click_handler","discord_command_handler","discord"}) {
        lobby_title = "Tic Tac Toe";
        lobby_description = "Simple Tic Tac Toe game everyone know (I believe)";
        lobby_image_url = "https://i.ibb.co/wM7sbC9/tic-tac-toe.webp";
    }

    Module_ptr create() {
        return std::dynamic_pointer_cast<Module>(std::make_shared<Discord_tic_tac_toe_command_impl>());
    }
} // gb