add_library(database SHARED
    ./database_impl.cpp
    ../../module/module.cpp
)

# Link the MySQL client library
target_link_libraries(database mysqlclient)