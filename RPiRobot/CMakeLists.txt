cmake_minimum_required(VERSION 3.3)
project(RPiRobot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c TCPServer.c TCPServer.h)
add_executable(RPiRobot ${SOURCE_FILES})

IF (WIN32)
    MESSAGE(STATUS "Now is windows")
ELSEIF (APPLE)
    MESSAGE(STATUS "Now is Apple systens.")
    message("FreeBSD")
ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
    target_link_libraries(RPiRobot pthread m)
ENDIF ()