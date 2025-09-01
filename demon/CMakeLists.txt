cmake_minimum_required(VERSION 3.10)
project(AutoBrightnessIluminance CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED libsystemd)

set(SOURCES
    src/main.cpp
    src/cpp/LoadConfig.cpp
    src/cpp/Config.cpp

)

set(HEADERS
    src/h/LoadConfig.h
    src/h/vec2_u16.h
    src/h/Config.h
)


add_executable(AutoBrightnessIluminance ${SOURCES} ${HEADERS})
target_include_directories(AutoBrightnessIluminance PRIVATE ${SYSTEMD_INCLUDE_DIRS})
target_link_libraries(AutoBrightnessIluminance PRIVATE ${SYSTEMD_LIBRARIES})