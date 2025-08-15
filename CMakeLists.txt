cmake_minimum_required(VERSION 3.10)
project(AutoBrightnessIluminance CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

add_executable(AutoBrightnessIluminance src/main.cpp)
target_include_directories(AutoBrightnessIluminance PRIVATE ${SYSTEMD_INCLUDE_DIRS})
target_link_libraries(AutoBrightnessIluminance PRIVATE ${SYSTEMD_LIBRARIES})