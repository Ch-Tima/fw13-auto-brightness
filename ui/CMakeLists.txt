cmake_minimum_required(VERSION 3.10)
project(AutoBrightnessUI CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Widgets Charts)

qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/view/MainView.cpp
    src/dbus/DbusClient.cpp

)

set(HEADERS
    src/view/MainView.h
    src/dbus/DbusClient.h
)

add_executable(AutoBrightnessUI ${SOURCES} ${HEADERS})
target_include_directories(AutoBrightnessUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(AutoBrightnessUI Qt6::Core Qt6::Widgets Qt6::DBus Qt6::Charts)
