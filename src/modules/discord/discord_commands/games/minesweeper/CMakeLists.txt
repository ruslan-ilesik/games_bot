add_library(discord_game_minesweeper SHARED
        ./discord_minesweeper_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ../../../discord_games/minesweeper/discord_minesweeper_game.cpp
        ../../../../../games/minesweeper/minesweeper.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_minesweeper PRIVATE dpp)

target_include_directories(discord_game_minesweeper PRIVATE ${DPP_INCLUDE_DIR})