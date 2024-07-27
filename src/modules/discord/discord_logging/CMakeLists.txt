add_library(discord_logging SHARED
        discord_logging_impl.cpp
        ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_logging PRIVATE dpp)

target_include_directories(discord_logging PRIVATE ${DPP_INCLUDE_DIR})