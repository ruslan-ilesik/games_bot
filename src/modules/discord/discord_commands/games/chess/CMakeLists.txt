add_library(discord_game_chess SHARED
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ./discord_chess_command_impl.cpp
        #../../../discord_games/chess/discord_chess_game.cpp
        #../../../../../games/chess/chess/chess.cpp #do not include
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_chess PRIVATE dpp)

target_include_directories(discord_game_chess PRIVATE ${DPP_INCLUDE_DIR})