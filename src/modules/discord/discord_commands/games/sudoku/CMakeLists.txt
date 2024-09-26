add_library(discord_game_sudoku SHARED
        ./discord_sudoku_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ../../../discord_games/sudoku/discord_sudoku_game.cpp
        discord_sudoku_command_impl.hpp
        ../../../discord_games/sudoku/discord_sudoku_game.hpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_sudoku PRIVATE dpp)

target_include_directories(discord_game_sudoku PRIVATE ${DPP_INCLUDE_DIR})