add_library(webserver SHARED
        webserver_impl.cpp
        ./api/index_page.cpp
        ./api/commands_page.cpp
        ./api/discord_login.cpp
        ./api/patreon.cpp
        ./api/premium.cpp
        ./utils/type_conversions.cpp
        ./utils/validators.cpp
        ./utils/cookie_manager.cpp
        ./utils/other.cpp
        ../../module/module.cpp
)

add_definitions(-DCXX_FILESYSTEM_NO_LINK_NEEDED)

find_package(dpp REQUIRED CONFIG)
find_package(cpp-jwt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Drogon CONFIG REQUIRED)

target_link_libraries(webserver PRIVATE dpp Drogon::Drogon cpp-jwt::cpp-jwt OpenSSL::Crypto)

target_include_directories(webserver PRIVATE ${DPP_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

