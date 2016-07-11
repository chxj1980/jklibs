## jkprotocol

project(jkprotocol)
set(JKPROTOCOL_FILE
        jkprotocol.c
        jkprotocol.h)

set(JKPROTOCOL_BIN_FILE
        binary/jkprotocolbin.c binary/jkprotocolbin.h)

set(DEMO_JKPROTOCOL_FILE
        demo_jkprotocol.c)

set(DEMO_JKPROTOCOL_BIN_FILE
        binary/demo_jkprotocolbin.c)

add_library(jkprotocol ${JKPROTOCOL_FILE} ${JKPROTOCOL_BIN_FILE})

link_directories(${CMAKE_BINARY_DIR})

add_executable(demo_jkprotocol ${DEMO_JKPROTOCOL_FILE})
target_link_libraries(demo_jkprotocol jkprotocol kfmd5 kfcommon)

add_executable(demo_jkprotocol_bin ${DEMO_JKPROTOCOL_BIN_FILE})
target_link_libraries(demo_jkprotocol_bin jkprotocol kfmd5 kfcommon)