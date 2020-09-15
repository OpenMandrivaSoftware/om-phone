cmake_minimum_required(VERSION 3.18.2)
project(om-phone VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt5 5.15.0 CONFIG REQUIRED COMPONENTS Core DBus)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
include_directories(/usr/include/qt5 /usr/include/qt5/QtCore /usr/include/qt5/QtDBus)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MODEMMANAGER ModemManager REQUIRED)
add_definitions(${MODEMMANAGER_CFLAGS})

add_subdirectory(src)
