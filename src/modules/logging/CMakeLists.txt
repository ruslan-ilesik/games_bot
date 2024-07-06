add_library(logging SHARED
        ./logging.cpp
        ../../module/module.cpp
)

# Find the spdlog package
find_package(spdlog REQUIRED)

# Link spdlog to the logging library
target_link_libraries(logging PRIVATE spdlog::spdlog)

# Include spdlog headers
target_include_directories(logging PRIVATE ${SPDLOG_INCLUDE_DIR})