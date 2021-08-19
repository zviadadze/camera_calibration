cmake_minimum_required(VERSION 3.10)
project(cam_calibration)

enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 3.2 REQUIRED)

include_directories(lib/camera_calibration/include)
include_directories(include)

add_subdirectory(lib/camera_calibration)

add_executable(${PROJECT_NAME} src/main.cpp include/main.h)

target_link_libraries(${PROJECT_NAME} camera_calibration)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    CALIBRATION_PARAMETERS_FILENAME="${CMAKE_CURRENT_SOURCE_DIR}/share/calibration_paramters.txt"
)

set(COMMON_C_FLAGS "-pipe -Wall -Wextra -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_C_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_C_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")

set(COMMON_LINKER_FLAGS "${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COMMON_LINKER_FLAGS}")