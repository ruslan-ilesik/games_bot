add_library(premium_manager SHARED
        ./premium_manager_impl.cpp
        ../../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)

target_link_libraries(premium_manager PRIVATE dpp)

target_include_directories(premium_manager PRIVATE ${DPP_INCLUDE_DIR})