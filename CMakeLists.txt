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

add_library(jklibs
        ${STORAGE_FILES}
        ${COMMON_FILES}
        ${RECORDSERVER_FILES}
        ${CODEC_FILES}
		)

add_executable(avstorage_test
        storage/avstorage_test.c
        ${STORAGE_FILES}
        ${COMMON_FILES})

add_executable(avprocess_test
        recordserver/cm_avprocess_test.c
        ${RECORDSERVER_FILES}
        ${CODEC_FILES}
        ${STORAGE_FILES}
        ${COMMON_FILES})

target_link_libraries(avprocess_test
        x264
        x265)

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

