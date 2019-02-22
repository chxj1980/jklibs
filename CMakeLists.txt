cmake_minimum_required(VERSION 2.8)

project(jklibs)

include_directories(common)
include_directories(storage)
include_directories(codec)

include_directories(/opt/data/libs/x264/amd64/include)
include_directories(/opt/data/libs/x265/amd64/include)
link_directories(/opt/data/libs/x264/amd64/lib)
link_directories(/opt/data/libs/x265/amd64/lib)

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

add_library(jklibs
        ${STORAGE_FILES}
        ${COMMON_FILES}
        ${RECORDSERVER_FILES}
        ${CODEC_FILES})

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