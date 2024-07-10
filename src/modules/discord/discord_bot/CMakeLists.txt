add_library(discord_bot SHARED
        ./discord_bot.cpp
        ./discord_cluster.cpp
        ../../../module/module.cpp
)

# Find the dpp package
find_package(dpp REQUIRED)

# Link dpp to the logging library
target_link_libraries(discord_bot PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_bot PRIVATE ${DPP_INCLUDE_DIR})