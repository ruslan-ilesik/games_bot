add_library(webserver SHARED
        webserver_impl.cpp
        ./api/index_page.cpp
        ./api/commands_page.cpp
        ./api/discord_login.cpp
        ./utils/type_conversions.cpp
        ./utils/validators.cpp
        ./utils/cookie_manager.cpp
        ../../module/module.cpp
)

find_package(dpp REQUIRED CONFIG)
find_package(cpp-jwt REQUIRED)


target_link_libraries(webserver PRIVATE dpp)

target_include_directories(webserver PRIVATE ${DPP_INCLUDE_DIR})



add_definitions(-DCXX_FILESYSTEM_NO_LINK_NEEDED)
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(webserver PRIVATE Drogon::Drogon cpp-jwt::cpp-jwt)
