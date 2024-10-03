add_library(discord_command_premium SHARED
        ./discord_command_premium_impl.cpp
        ../../discord_general_command.cpp
        ../../../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_command_premium PRIVATE dpp)

target_include_directories(discord_command_premium PRIVATE ${DPP_INCLUDE_DIR})