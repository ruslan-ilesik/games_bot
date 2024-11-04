add_library(webserver SHARED
        webserver_impl.cpp
        ./utils/validators.cpp
        ./api/index_page.cpp
        ./utils/type_convertions.cpp
        ../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)


target_link_libraries(webserver PRIVATE dpp)

target_include_directories(webserver PRIVATE ${DPP_INCLUDE_DIR})



add_definitions(-DCXX_FILESYSTEM_NO_LINK_NEEDED)
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(webserver PRIVATE Drogon::Drogon )
