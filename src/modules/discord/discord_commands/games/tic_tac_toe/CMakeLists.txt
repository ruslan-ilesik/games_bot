add_library(discord_game_tic_tac_toe SHARED
    ./discord_tic_tac_toe_command_impl.cpp
    ../discord_game_command.cpp
    ../../../discord_games/discord_game.cpp
    ../../../discord_games/tic_tac_toe/discord_tic_tac_toe_game.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_tic_tac_toe PRIVATE dpp)

target_include_directories(discord_game_tic_tac_toe PRIVATE ${DPP_INCLUDE_DIR})