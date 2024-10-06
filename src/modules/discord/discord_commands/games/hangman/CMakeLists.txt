add_library(discord_game_hangman SHARED
        ./discord_hangman_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../discord_games/hangman/discord_hangman_game.cpp
        ../../../../../module/module.cpp
        ../../../../../games/hangman/hangman.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_hangman PRIVATE dpp)

target_include_directories(discord_game_hangman PRIVATE ${DPP_INCLUDE_DIR})