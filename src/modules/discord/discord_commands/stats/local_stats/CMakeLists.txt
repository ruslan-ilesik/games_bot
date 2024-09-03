add_library(discord_command_local_stats SHARED
        ./discord_command_local_stats_impl.cpp
        ../../discord_general_command.cpp
        ../../../../../module/module.cpp
)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_command_local_stats PRIVATE dpp)

target_include_directories(discord_command_local_stats PRIVATE ${DPP_INCLUDE_DIR})