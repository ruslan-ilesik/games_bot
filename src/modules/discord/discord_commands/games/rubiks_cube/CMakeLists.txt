add_library(discord_game_rubiks_cube SHARED
        ./discord_rubiks_cube_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ../../../discord_games/rubiks_cube/discord_rubiks_cube_game.cpp
        ../../../../../games/rubiks_cube/rubiks_cube.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_rubiks_cube PRIVATE dpp)

target_include_directories(discord_game_rubiks_cube PRIVATE ${DPP_INCLUDE_DIR})