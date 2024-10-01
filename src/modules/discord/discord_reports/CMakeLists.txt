add_library(discord_reports SHARED
        ./discord_reports_impl.cpp
        ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_reports PRIVATE dpp)

target_include_directories(discord_reports PRIVATE ${DPP_INCLUDE_DIR})