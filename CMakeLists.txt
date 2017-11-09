cmake_minimum_required(VERSION 3.3)
project(jklibs)

if(EXTERN)
set(BASE_DIR "/opt/data/proj/openwrt-program/staging_dir/target-x86_64-linux-gnu")
include_directories(${BASE_DIR}/usr/include)
link_directories(${BASE_DIR}/usr/lib)
endif()

option(JKTEST "Build Test demos" ON)
option(JSONC "Json c use" ON)
option(POKY "Poky embbed" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(JKTEST)
    message(STATUS "JKTest is ON")
else()
    message(STATUS "JKTest is OFF")
endif()

include_directories(common)
include_directories(common/json)
include_directories(kfmd5)

add_subdirectory(demo)

add_subdirectory(common)
add_subdirectory(kfmd5)
add_subdirectory(jkprotocol)
add_subdirectory(jni)

add_library(jklibs STATIC common kfmd5 jkprotocol)
set_target_properties(jklibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(jklibs common kfmd5 jkprotocol)

install(FILES demo/jsonc_demo DESTINATION /usr/bin)
install(FILES libjklibs.so DESTINATION /usr/lib)

aux_source_directory(vdev VDEV_SOURCE)
add_executable(vdev ${VDEV_SOURCE})
target_link_libraries(vdev common)
