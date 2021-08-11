cmake_minimum_required(VERSION 3.10)
project(camera_calibration)

set(INCLUDE_BASE_DIR include)
set(INCLUDE_DIR ${INCLUDE_BASE_DIR}/camera_calibration)

include_directories(${INCLUDE_BASE_DIR})

set(CAMERA_CALIBRATION_HEADERS
    ${INCLUDE_DIR}/camera_calibration.h
)

set(CAMERA_CALIBRATION_SOURCES
    src/camera_calibration.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${CAMERA_CALIBRATION_HEADERS}
    ${CAMERA_CALIBRATION_SOURCES}
)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})


