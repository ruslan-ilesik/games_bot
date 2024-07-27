add_library(discord SHARED
        discord_impl.cpp
        ../../../module/module.cpp

)


find_package(dpp REQUIRED CONFIG)

target_link_libraries(discord PRIVATE dpp)

target_include_directories(discord PRIVATE ${DPP_INCLUDE_DIR})