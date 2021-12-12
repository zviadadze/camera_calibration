cmake_minimum_required(VERSION 3.10)
project(camera_calibration_lib)

set(INCLUDE_BASE_DIR include)
set(INCLUDE_DIR ${INCLUDE_BASE_DIR}/camera_calibration)

set(CAMERA_CALIBRATION_HEADERS
    ${INCLUDE_DIR}/camera_calibration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../json/include/nlohmann/json.hpp
)

set(CAMERA_CALIBRATION_SOURCES
    src/camera_calibration.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${CAMERA_CALIBRATION_HEADERS}
    ${CAMERA_CALIBRATION_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_BASE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../json/include
)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})