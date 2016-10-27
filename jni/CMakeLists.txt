
project(sav)

set(SOURCE_FILE
        SAVCodec.c
        SAVContainer.c
        SAVPriv.c
        SAVUtil.c
        SAVContainer.h
        SAVPriv.h)

include_directories(${CMAKE_SOURCE_DIR}/jni)
include_directories(${CMAKE_SOURCE_DIR}/jni/libsav)
include_directories(/opt/data/data/libs/x64/ffmpeg-94a52417/include)
link_directories(/opt/data/data/libs/x64/ffmpeg-94a52417/lib)
add_library(sav ${SOURCE_FILE})

add_executable(demo_sav demo_sav.c)
target_link_libraries(demo_sav sav avcodec swresample swscale avutil avformat m pthread z)
