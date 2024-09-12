add_library(discord_game_battleships SHARED
    ../discord_game_command.cpp
    ../../discord_general_command.cpp
    ../../../discord_games/discord_game.cpp
    ../../../../../module/module.cpp
    discord_battleships_command_impl.cpp
    discord_battleships_command.hpp
    ../../../../../games/battleships/battleships.hpp
    ../../../../../games/battleships/battleships.cpp
    ../../../discord_games/battleships/discord_battleships_game.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_game_battleships PRIVATE dpp)

target_include_directories(discord_game_battleships PRIVATE ${DPP_INCLUDE_DIR})