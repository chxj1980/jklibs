
project(demo)

include(../cmake/Config.cmake)

if(JKTEST)
    include_directories(/usr/include)
#    set(ULOOP_DEMO_FILE uloop_demo.c)
#    set(ULOOP_DEMO_NAME uloop_demo)
#    add_executable(${ULOOP_DEMO_NAME} ${ULOOP_DEMO_FILE})
#    target_link_libraries(${ULOOP_DEMO_NAME} ubox)

#    add_executable(jsonc_demo jsonc_demo.c)
#    target_link_libraries(jsonc_demo json-c)

#    add_executable(list_demo list_demo.c)
#    target_link_libraries(list_demo ubox)

    add_executable(base_demo base_demo.c)
    target_link_libraries(base_demo kfcommon)

    add_executable(netlink_demo netlink_demo.c)
    target_link_libraries(netlink_demo kfcommon)

    add_executable(demo_small demo_small.c)

        if (POKY)
    add_executable(demo_opengl demo_opengl.c)
    target_link_libraries(demo_opengl glut GL)

        include_directories(/opt/data/data/source/opengl/opengles3-book/Common/Include)
        link_directories(/opt/data/data/source/opengl/opengles3-book/build/Common)
            add_executable(demo_opengles demo_opengles.c)
        target_link_libraries(demo_opengles Common GLESv2 EGL SM ICE X11 Xext)
        endif(POKY)

        add_executable(demo_psstream psstream/demo_psstream.cpp
                psstream/PSPacket.h
                psstream/streamdef.h)
        add_executable(demo_ps_es psstream/demo_ps_es.cpp
                psstream/PS2ES.cpp psstream/PS2ES.h)

#    add_executable(demo_ncurses demo_ncurses.c)
endif()

include_directories(op_stream)

set(OP_STREAM_FILE
        op_stream/h264.cpp)

add_executable(h264_parse op_stream/h264_parse.cpp ${OP_STREAM_FILE})

add_executable(demo_cplus_list demo_cplusplus_list.cpp)

add_executable(demo_media_option media_option.c)

if (APPLE)
    add_definitions(-D__MACOS__)
    include_directories(/Users/jmdvirus/jmd/proj/work/source/public/lib/opencv-3.2.0/-x64/include)
    link_directories(/Users/jmdvirus/jmd/proj/work/source/public/lib/opencv-3.2.0/-x64/lib)
    set(OPENCV_LINK_LIBS opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_objdetect opencv_videoio)
elseif (WINDOWS)
    include_directories("J:\\proj\\app\\opencv-3.2.0\\opencv\\build\\include")
    link_directories("J:\\proj\\app\\opencv-3.2.0\\opencv\\build\\x64\\vc14\\lib")
    set(OPENCV_LINK_LIBS opencv_world320d)
endif ()

add_executable(opencv_tutorial opencv/tutorial.cpp)
target_link_libraries(opencv_tutorial ${OPENCV_LINK_LIBS})
add_executable(opencv_image opencv/image_tool.cpp)
target_link_libraries(opencv_image ${OPENCV_LINK_LIBS})

add_executable(opencv_video opencv/play_video.cpp)
target_link_libraries(opencv_video ${OPENCV_LINK_LIBS})

add_executable(opencv_video_from_capture opencv/play_video_from_capture.cpp)
target_link_libraries(opencv_video_from_capture ${OPENCV_LINK_LIBS})

add_executable(opencv_face_detect opencv/face_detect.cpp
        opencv/CVFaceDetect.cpp
        opencv/CVFaceDetect.h
        )
target_link_libraries(opencv_face_detect ${OPENCV_LINK_LIBS})
