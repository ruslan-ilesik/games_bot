add_library(discord_button_click_handler SHARED
        ./discord_button_click_handler_impl.cpp
        ../../../../module/module.cpp
        ../id_cache.cpp
)


# Find the dpp package
find_package(dpp REQUIRED CONFIG)

# Link dpp to the logging library
target_link_libraries(discord_button_click_handler PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_button_click_handler PRIVATE ${DPP_INCLUDE_DIR})