cmake_minimum_required(VERSION 3.10)
project(camera_calibration)

enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 3.2 REQUIRED)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake-modules
    ${CMAKE_SOURCE_DIR}/cmake)

include_directories(include)
include_directories(lib/json/include)
include_directories(lib/camera_calibration/include)

add_subdirectory(lib/camera_calibration)

set(HEADERS include/main_structures.h include/main_functions.h)

set(SOURCES src/main.cpp src/main_functions.cpp lib/json/include/nlohmann/json.hpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} camera_calibration_lib)