set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(om-phone main.cpp DBusObject.cpp Modem.cpp ModemApplication.cpp)
target_link_libraries(om-phone Qt5Core Qt5DBus Qt5Widgets)
