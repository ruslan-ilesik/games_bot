add_library(discord_games_manager SHARED
    ./discord_games_manager_impl.cpp
    ../../../../module/module.cpp
    ../discord_game.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_games_manager PRIVATE dpp)

target_include_directories(discord_games_manager PRIVATE ${DPP_INCLUDE_DIR})