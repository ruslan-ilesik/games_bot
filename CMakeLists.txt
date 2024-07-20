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
)


# Add compile options to disable specific warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
            -Wno-return-type-c-linkage
            -Wno-unused-command-line-argument
            -Wdocumentation
            -Wno-unused-value
            -DDPP_CORO
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
