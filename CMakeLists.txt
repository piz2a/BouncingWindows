cmake_minimum_required(VERSION 3.25)
project(BouncingWindows)

#find_package( OpenCV REQUIRED )
#include_directories( ${OpenCV_INCLUDE_DIRS} )
#include_directories( ${OpenCV_LIBS} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(DCMAKE_C_FLAGS "-mwindows")

add_executable(BouncingWindows WIN32 main.cpp MovingWindow.cpp MovingWindow.h MainWindow.cpp MainWindow.h)

#target_link_libraries( BouncingWindows ${OpenCV_LIBS} )
