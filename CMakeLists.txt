cmake_minimum_required(VERSION 3.3)
project(jklibs)

set(JKUTILS ON)
set(JKNET ON)
set(JKLISTS OFF)
set(JKTEST ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(JKLISTS)
    include_directories(lists)
endif()

if(JKTEST)
    message(STATUS "JKTest is ON")
else()
    message(STATUS "JKTest is OFF")
endif()

if(JKUTILS)
    include_directories(utils)
    add_subdirectory(utils)
endif()

if(JKNET)
    add_subdirectory(net)
endif()

#add_library(jklibs STATIC)
add_library(jklibs SHARED net utils lists)
set_target_properties(jklibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(jklibs net utils lists)
