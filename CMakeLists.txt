cmake_minimum_required(VERSION 2.8)

project(jklibs)

include_directories(common)
include_directories(storage)
include_directories(codec)

include_directories(${X264_PATH}/include)
include_directories(${X265_PATH}/include)
include_directories(${OPENCV_PATH}/include)
include_directories(${OPENCV_PATH}/include/opencv4)
link_directories(${X264_PATH}/lib)
link_directories(${X265_PATH}/lib)
link_directories(${OPENCV_PATH}/lib)

option(WINDOWS "windows platform" OFF)
option(LINUX "linux platform" OFF)

MESSAGE(STATUS "System is ${CMAKE_SYSTEM_NAME}")

IF (LINUX)
message(STATUS "This is linux platform")
ENDIF()

set(COMMON_FILES
        common/cm_utils.c
        common/cm_print.c)

set(CODEC_FILES
        codec/codec.c
        codec/encoder.c
        codec/enc_h265.c
        codec/cmyuv.c)

set(STORAGE_FILES
        storage/avstorage.c)

set(RECORDSERVER_FILES
        recordserver/cm_avprocess.c)

set(OPENAV_FILES
		openav/stitching.cpp)

if (LINUX)
add_library(jklibs
        ${STORAGE_FILES}
        ${COMMON_FILES}
        ${RECORDSERVER_FILES}
        ${CODEC_FILES}
		)
endif()

IF (LINUX)
add_executable(cmcodec
		codec/cmcodec.c
		codec/cmyuv.c
		${CODEC_FILES}
		${COMMON_FILES}
		)
target_link_libraries(cmcodec
		x264 x265)
ENDIF()

add_executable(cmyuvtools
		codec/cmyuvtools.c
		codec/cmyuv.c
		${COMMON_FILES}
		)

add_executable(avstorage_test
        storage/avstorage_test.c
        ${STORAGE_FILES}
        ${COMMON_FILES})

IF (LINUX)
add_executable(avprocess_test
        recordserver/cm_avprocess_test.c
        ${RECORDSERVER_FILES}
        ${CODEC_FILES}
        ${STORAGE_FILES}
        ${COMMON_FILES})

target_link_libraries(avprocess_test
        x264
        x265)
ENDIF()

IF (LINUX)
add_executable(cmstitch
		openav/stitching.cpp
		codec/cmyuv.c
		)

target_link_libraries(cmstitch
		opencv_stitching
		opencv_features2d
		opencv_calib3d
		opencv_flann
		opencv_imgproc
		opencv_imgcodecs
		opencv_core
		)
ENDIF()

IF (LINUX)
add_executable(catch_img
		openav/catch_imgs.cpp
		codec/cmyuv.c
		)

target_link_libraries(catch_img
		opencv_imgproc
		opencv_imgcodecs
		opencv_core
		)
ENDIF()
