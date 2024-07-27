add_library(discord_bot SHARED
        discord_bot_impl.cpp
        ./discord_cluster.cpp
        ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_bot PRIVATE dpp)

target_include_directories(discord_bot PRIVATE ${DPP_INCLUDE_DIR})