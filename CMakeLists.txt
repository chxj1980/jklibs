cmake_minimum_required(VERSION 3.3)
project(jklibs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(JKTEST)
    message(STATUS "JKTest is ON")
else()
    message(STATUS "JKTest is OFF")
endif()

include_directories(utils)
include_directories(lists)

add_subdirectory(net)
add_subdirectory(utils)

#add_library(jklibs STATIC)
add_library(jklibs SHARED net utils)
set_target_properties(jklibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(jklibs net utils)
