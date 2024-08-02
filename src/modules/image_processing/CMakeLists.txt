add_library(image_processing SHARED
    ./image_processing_impl.cpp
    ./image_impl.cpp
    ../../module/module.cpp
)

find_package(OpenCV REQUIRED CONFIG)
target_link_directories(image_processing PUBLIC ${OpenCV_LIB_DIR})
target_link_libraries(image_processing ${OpenCV_LIBS})
target_include_directories(image_processing PUBLIC ${OpenCV_INCLUDE_DIRS})