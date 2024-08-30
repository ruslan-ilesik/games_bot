add_library(discord_achievements_processing SHARED
    ./discord_achievements_processing_impl.cpp
    ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_achievements_processing PRIVATE dpp)

target_include_directories(discord_achievements_processing PRIVATE ${DPP_INCLUDE_DIR})