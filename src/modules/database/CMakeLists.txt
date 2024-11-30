add_library(database SHARED
        ./database_impl.cpp
        ../../module/module.cpp
)

if (NOT LINUX)
    # Add MySQL library and include paths for FreeBSD
    target_link_directories(database PRIVATE /usr/local/lib/mysql)
endif()

# Link the MySQL client library
target_link_libraries(database mysqlclient)
