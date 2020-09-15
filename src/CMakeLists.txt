set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(phone main.cpp DBusObject.cpp Modem.cpp SIM.cpp Call.cpp Phone.cpp)
target_link_libraries(phone Qt5Core Qt5DBus Qt5Gui Qt5Widgets)
