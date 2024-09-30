add_library(discord_game_2048 SHARED
    ../discord_game_command.cpp
    ../../discord_general_command.cpp
    ../../../discord_games/discord_game.cpp
    ../../../../../module/module.cpp
    ./discord_2048_command_impl.cpp
    ../../../discord_games/2048/discord_game_2048.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_2048 PRIVATE dpp)

target_include_directories(discord_game_2048 PRIVATE ${DPP_INCLUDE_DIR})