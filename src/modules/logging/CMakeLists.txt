add_library(logging SHARED
        ./logging.cpp
        ../../module/module.cpp
)

# Find the spdlog package
find_package(spdlog REQUIRED)

# Print a message indicating spdlog was found
if (spdlog_FOUND)
    if (spdlog_VERSION)
        message(STATUS "Found spdlog: ${spdlog_VERSION}")
    else()
        message(STATUS "Found spdlog")
    endif()
else()
    message(FATAL_ERROR "spdlog library not found")
endif()

# Link spdlog to the logging library
target_link_libraries(logging PRIVATE spdlog::spdlog)

# Include spdlog headers
target_include_directories(logging PRIVATE ${SPDLOG_INCLUDE_DIR})