cmake_minimum_required(VERSION 3.3)
project(jklibs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(utils)
include_directories(lists)

add_subdirectory(net)
add_subdirectory(utils)
