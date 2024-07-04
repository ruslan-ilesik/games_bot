cmake_minimum_required(VERSION 3.29)
project(games_bot)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/modules")

# Set custom compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -I/usr/lib/llvm-19/include/c++/v1/")
add_link_options(
        -stdlib=libc++
        -L/usr/lib/llvm-19/lib
        -Wl,-rpath,/usr/lib/llvm-19/lib
        -Wdocumentation
)


# Add compile options to disable specific warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
            -Wno-return-type-c-linkage
            -Wno-unused-command-line-argument
    )
endif ()

# Include the main directory
include_directories(.)


# Add the main executable
add_executable(games_bot
        src/main.cpp
        src/main.hpp
        src/module/modules_manager.cpp
        src/module/module.cpp
)

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
# Link spdlog to the main executable
target_link_libraries(games_bot PRIVATE spdlog::spdlog)
target_include_directories(games_bot PRIVATE ${SPDLOG_INCLUDE_DIRS})









# Function to add all subdirectories recursively, excluding specific paths
function(add_all_subdirectories base_dir)
    file(GLOB_RECURSE subdirs LIST_DIRECTORIES true "${base_dir}/*")
    foreach(subdir ${subdirs})
        if (IS_DIRECTORY ${subdir})
            # Skip the utils directory and its subdirectories
            if (subdir MATCHES ".*src/utils.*")
                continue()
            endif ()

            if (EXISTS ${subdir}/CMakeLists.txt)
                message(STATUS "Adding subdirectory: ${subdir}")
                add_subdirectory(${subdir})
            endif ()
        endif ()
    endforeach ()
endfunction()


# Call the function with the base directory
add_all_subdirectories(${CMAKE_SOURCE_DIR}/src)
