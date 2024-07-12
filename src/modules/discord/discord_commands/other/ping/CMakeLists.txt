add_library(discord_command_ping SHARED
    ./discord_command_ping_impl.cpp
    ../../../../../module/module.cpp
)

# Find the dpp package
find_package(dpp REQUIRED)

# Link dpp to the logging library
target_link_libraries(discord_command_ping PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_command_ping PRIVATE ${DPP_INCLUDE_DIR})