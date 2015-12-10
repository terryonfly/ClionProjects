cmake_minimum_required(VERSION 2.8)
project(IotRobotCtrl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_library(GL GLU glut)
add_executable(IotRobotCtrl ${SOURCE_FILES})