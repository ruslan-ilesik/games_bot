add_library(discord_statistics_collector SHARED
        ./discord_statistics_collector_impl.cpp
        ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_statistics_collector PRIVATE dpp)

target_include_directories(discord_statistics_collector PRIVATE ${DPP_INCLUDE_DIR})