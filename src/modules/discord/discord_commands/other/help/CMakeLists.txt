add_library(discord_command_help SHARED
        ./discord_command_help_impl.cpp
        ../../../../../module/module.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_command_help PRIVATE dpp)

target_include_directories(discord_command_help PRIVATE ${DPP_INCLUDE_DIR})