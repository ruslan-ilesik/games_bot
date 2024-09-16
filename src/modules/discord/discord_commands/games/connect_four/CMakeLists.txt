add_library(discord_game_connect_four SHARED
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ./discord_connect_four_command_impl.cpp
        ../../../discord_games/connect_four/discord_connect_four_game.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_connect_four PRIVATE dpp)

target_include_directories(discord_game_connect_four PRIVATE ${DPP_INCLUDE_DIR})