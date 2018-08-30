cmake_minimum_required(VERSION 3.0)
project(RGBserial)

set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})


add_executable(
	RGBserial
	src/main.c
	src/serial.c
)

add_definitions(${GTK3_CFLAGS_OTHER})
target_link_libraries(RGBserial ${GTK3_LIBRARIES} ${catkin_LIBRARIES} pthread)
