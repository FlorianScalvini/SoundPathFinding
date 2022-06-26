cmake_minimum_required(VERSION 3.20)
project(Stag)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -w  ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -w ")

# include and link dirs of cuda and tensorrt, you need adapt them if yours are different
# cuda
include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)
# tensorrt
include_directories(/usr/include/x86_64-linux-gnu/)
link_directories(/usr/lib/x86_64-linux-gnu/)

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Find installed package
find_package(CUDA REQUIRED)
find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED )
find_package(ALSA REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
file(GLOB MARKER_FILE1 "marker/*.c*")
file(GLOB MARKER_FILE2 "marker/ED/*.c*")

file(GLOB SONIFIER_HEADER "sonifier/*.h*")
file(GLOB SONIFIER_SOURCE "sonifier/*.c*")

file(GLOB PATH_HEADER "path/*.h*")
file(GLOB PATH_SOURCE "path/*.c*")

file(GLOB SOUND_HEADER "sound/*.h*")
file(GLOB SOUND_SOURCE "sound/*.c*")

include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${ALSA_INCLUDE_DIRS}
        marker/
        marker/ED/
)

add_library(
        LibStag SHARED
        ${MARKER_FILE1}
        ${MARKER_FILE2})
target_link_libraries(
        LibStag
        ${OpenCV_LIBS} ${realsense2_LIBRARY}
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app)

add_executable(
        testrun
        main.cpp
        ${SONIFIER_HEADER}
        ${SONIFIER_SOURCE}
        ${PATH_HEADER}
        ${PATH_SOURCE}
        ${SOUND_HEADER}
        ${SOUND_SOURCE}
)

target_link_libraries(
        testrun
        LibStag
        ${realsense2_LIBRARY} ${OpenCV_LIBS} ${TORCH_LIBRARIES} ${ALSA_LIBRARIES}
        pthread
)

