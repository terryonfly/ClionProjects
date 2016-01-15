cmake_minimum_required(VERSION 2.8.4)
project(HomeBase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c TCPServer.c TCPServer.h)
add_executable(HomeBase ${SOURCE_FILES})

target_link_libraries(HomeBase m)
target_link_libraries(HomeBase pthread)