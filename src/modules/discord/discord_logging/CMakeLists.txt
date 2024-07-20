add_library(discord_logging SHARED
        discord_logging_impl.cpp
        ../../../module/module.cpp
)

# Find the dpp package
find_package(dpp REQUIRED CONFIG)

# Link dpp to the logging library
target_link_libraries(discord_logging PRIVATE dpp)

# Include dpp headers
target_include_directories(discord_logging PRIVATE ${DPP_INCLUDE_DIR})