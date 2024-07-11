add_library(discord SHARED
        discord_impl.cpp
        ../../../module/module.cpp

)


# Find the dpp package
find_package(dpp REQUIRED)

# Link dpp to the logging library
target_link_libraries(discord PRIVATE dpp)

# Include dpp headers
target_include_directories(discord PRIVATE ${DPP_INCLUDE_DIR})