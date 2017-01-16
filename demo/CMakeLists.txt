
project(demo)

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
