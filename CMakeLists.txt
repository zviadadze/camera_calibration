cmake_minimum_required(VERSION 3.10)
project(camera_calibration)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 3.4 REQUIRED)

add_subdirectory(lib/camera_calibration)

set(INCLUDE_BASE_DIR include)
set(SOURCE_BASE_DIR src)

set(HEADERS 
    ${INCLUDE_BASE_DIR}/secondary_structures_and_literals.h 
    lib/json/include/nlohmann/json.hpp
)
set(SOURCES 
    ${SOURCE_BASE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_BASE_DIR}
    lib/json/include
    lib/camera_calibration/include
)

target_link_libraries(${PROJECT_NAME} 
    camera_calibration_library
    ${OpenCV_LIBS}
)
