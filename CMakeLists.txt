cmake_minimum_required(VERSION 3.3)
project(jklibs)

set(BASE_DIR "/home/v/jkdata/work/source/openwrt/staging_dir/target-x86_64-linux-gnu")

set(JKTEST ON)
set(JSONC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(JKTEST)
    message(STATUS "JKTest is ON")
else()
    message(STATUS "JKTest is OFF")
endif()

include_directories(${BASE_DIR}/usr/include)
link_directories(${BASE_DIR}/usr/lib)

add_subdirectory(demo)

#add_library(jklibs STATIC)
add_library(jklibs SHARED)
set_target_properties(jklibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(jklibs)
