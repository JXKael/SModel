message(STATUS ">>> finding Qt 5.13.1")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(STATUS ">>> CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
