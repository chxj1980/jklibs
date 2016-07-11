cmake_minimum_required(VERSION 3.3)
project(jklibs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include_directories(lists)
#include_directories(mem)
#include_directories(os)
#include_directories(thread)
add_subdirectory(net)

