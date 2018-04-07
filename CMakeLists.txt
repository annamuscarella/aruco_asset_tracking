cmake_minimum_required(VERSION 3.7)
project(AssetTracking)

set(CMAKE_CXX_STANDARD 11)


find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )


find_package(aruco REQUIRED )
include_directories(aruco_INCLUDE_DIRS)


set(SOURCE_FILES main.cpp ArucoTracker.cpp ArucoTracker.h ArucoMarker.cpp ArucoMarker.h)
add_executable(AssetTracking ${SOURCE_FILES})
target_link_libraries( AssetTracking ${OpenCV_LIBS})
target_link_libraries( AssetTracking ${aruco_LIBS})
