add_library( discord_command_handler SHARED
        discord_command_handler_impl.cpp
    ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_command_handler PRIVATE dpp)

target_include_directories(discord_command_handler PRIVATE ${DPP_INCLUDE_DIR})