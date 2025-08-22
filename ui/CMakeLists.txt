cmake_minimum_required(VERSION 3.10)
project(AutoBrightnessUI CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Widgets Charts)

qt_standard_project_setup()

add_executable(AutoBrightnessUI src/main.cpp)
target_link_libraries(AutoBrightnessUI Qt6::Core Qt6::Widgets Qt6::DBus Qt6::Charts)
