add_library(gamescoin_manager SHARED
        ../../../module/module.cpp
        gamescoin_manager.hpp
        gamescoin_manager_impl.cpp
        gamescoin_manager_impl.hpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(gamescoin_manager PRIVATE dpp)

target_include_directories(gamescoin_manager PRIVATE ${DPP_INCLUDE_DIR})