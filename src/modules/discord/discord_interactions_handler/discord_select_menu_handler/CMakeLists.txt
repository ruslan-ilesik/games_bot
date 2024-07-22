add_library(discord_select_menu_handler SHARED
    ./discord_select_menu_handler_impl.cpp
    ../id_cache.cpp
    ../../../../module/module.cpp
)

# Find the dpp package
find_package(dpp REQUIRED CONFIG)

# Link dpp to the logging library
target_link_libraries(discord_select_menu_handler PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_select_menu_handler PRIVATE ${DPP_INCLUDE_DIR})