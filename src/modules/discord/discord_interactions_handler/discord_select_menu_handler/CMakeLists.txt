add_library(discord_select_menu_handler SHARED
    ./discord_select_menu_handler_impl.cpp
    ../id_cache.cpp
    ../../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord_select_menu_handler PRIVATE dpp)

target_include_directories(discord_select_menu_handler PRIVATE ${DPP_INCLUDE_DIR})