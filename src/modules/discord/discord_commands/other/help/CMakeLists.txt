add_library(discord_command_help SHARED
        ./discord_command_help_impl.cpp
        ../../../../../module/module.cpp
)

# Find the dpp package
find_package(dpp REQUIRED CONFIG)

# Link dpp to the logging library
target_link_libraries(discord_command_help PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_command_help PRIVATE ${DPP_INCLUDE_DIR})