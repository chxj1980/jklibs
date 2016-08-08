## jkprotocol

project(jkprotocol)
set(JKPROTOCOL_FILE
        jkprotocol.c
        jkprotocol.h)

set(JKPROTOCOL_V4_FILE
        v4/v4.c v4/v4.h)

set(JKPROTOCOL_BIN_FILE
        binary/jkprotocolbin.c binary/jkprotocolbin.h)

set(DEMO_JKPROTOCOL_FILE
        demo_jkprotocol.c)

set(DEMO_JKPROTOCOL_BIN_FILE
        binary/demo_jkprotocolbin.c)

set(DEMO_JKPROTOCOL_V4_FILE
        v4/v4_demo.c)

add_library(jkprotocol ${JKPROTOCOL_FILE} ${JKPROTOCOL_V4_FILE} ${JKPROTOCOL_BIN_FILE})

link_directories(${CMAKE_BINARY_DIR})

if (JKTEST)
add_executable(demo_jkprotocol ${DEMO_JKPROTOCOL_FILE})
target_link_libraries(demo_jkprotocol jkprotocol kfmd5 kfcommon)

add_executable(demo_jkprotocol_bin ${DEMO_JKPROTOCOL_BIN_FILE})
target_link_libraries(demo_jkprotocol_bin jkprotocol kfmd5 kfcommon)

    add_executable(v4_demo ${DEMO_JKPROTOCOL_V4_FILE} ${JKPROTOCOL_V4_FILE})
    target_link_libraries(v4_demo kfcommon)
endif()