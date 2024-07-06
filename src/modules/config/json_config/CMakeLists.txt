add_library(json_config SHARED
./json_config.cpp
../../../module/module.cpp
)

find_package(nlohmann_json 3.10.0 REQUIRED)

target_link_libraries(json_config PRIVATE nlohmann_json::nlohmann_json)