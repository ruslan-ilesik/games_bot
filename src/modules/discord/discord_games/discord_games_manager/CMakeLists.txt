add_library(discord_games_manager SHARED
    ./discord_games_manager_impl.cpp
    ../../../../module/module.cpp
    ../discord_game.cpp
)

# Find the dpp package
find_package(dpp REQUIRED CONFIG)

# Link dpp to the logging library
target_link_libraries(discord_games_manager PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_games_manager PRIVATE ${DPP_INCLUDE_DIR})