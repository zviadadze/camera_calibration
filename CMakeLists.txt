cmake_minimum_required(VERSION 3.10)
project(camera_calibration)

enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 3.2 REQUIRED)

include_directories(lib/camera_calibration/include)
include_directories(lib/json)
include_directories(include)

add_subdirectory(lib/camera_calibration)


set(HEADERS include/main_structures.h include/main_functions.h)

set(SOURCES src/main.cpp src/main_functions.cpp lib/json/json.hpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} camera_calibration_lib)


set(COMMON_C_FLAGS "-pipe -Wall -Wextra -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_C_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_C_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")

set(COMMON_LINKER_FLAGS "${CMAKE_THREAD_LIBS_INIT}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COMMON_LINKER_FLAGS}")