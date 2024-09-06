add_library(discord_game_dominoes SHARED
        ./discord_dominoes_command_impl.cpp
        ../discord_game_command.cpp
        ../../discord_general_command.cpp
        ../../../discord_games/discord_game.cpp
        ../../../../../module/module.cpp
        ../../../discord_games/dominoes/discord_dominoes_game.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_dominoes PRIVATE dpp)

target_include_directories(discord_game_dominoes PRIVATE ${DPP_INCLUDE_DIR})