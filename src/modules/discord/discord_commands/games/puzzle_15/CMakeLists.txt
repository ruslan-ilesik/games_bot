add_library(discord_game_puzzle_15 SHARED
        discord_puzzle_15_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        discord_puzzle_15_command_impl.hpp
        discord_puzzle_15_command.hpp
        ../../../discord_games/puzzle_15/discord_puzzle_15_game.cpp
        ../../../../../games/puzzle_15/puzzle_15.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_puzzle_15 PRIVATE dpp)

target_include_directories(discord_game_puzzle_15 PRIVATE ${DPP_INCLUDE_DIR})