cmake_minimum_required(VERSION 2.8.4)
project(HomeBase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c TCPServer.c TCPServer.h cJSON.c cJSON.h TCPConnection.h TCPConnection.c Sensor.h Sensor.c DataBase.h DataBase.c Platform.h)
add_executable(HomeBase ${SOURCE_FILES})

target_link_libraries(HomeBase m)
target_link_libraries(HomeBase pthread)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
    MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
target_link_libraries(HomeBase mysqlclient)
ENDIF ()