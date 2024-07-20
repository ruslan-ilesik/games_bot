add_library(crypto SHARED
        ./crypto_impl.cpp
        ../../module/module.cpp
)

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

# Include the OpenSSL directories
target_include_directories(crypto PRIVATE ${OPENSSL_INCLUDE_DIR})

# Link the OpenSSL libraries
target_link_libraries(crypto PRIVATE ${OPENSSL_LIBRARIES})