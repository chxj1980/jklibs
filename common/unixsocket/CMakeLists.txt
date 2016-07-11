
project(rtunixsocket)

set(SOURCE_UNIXSOCKET_FILE
        rt_unixsocket.c
        rt_unixsocket.h)

set(DEMO_UNIXSOCKET_FILE
        rt_unixsocket_demo.c)

add_library(rtunixsocket STATIC ${SOURCE_UNIXSOCKET_FILE})
target_link_libraries(rtunixsocket kfcommon m)

link_directories(${CMAKE_BINARY_DIR}/common)

add_executable(demo_unixsocket_server ${DEMO_UNIXSOCKET_FILE})
target_link_libraries(demo_unixsocket_server rtunixsocket kfcommon)

