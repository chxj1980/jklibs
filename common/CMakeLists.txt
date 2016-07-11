
project(kfcommon)

set(SOURCE_COMMON_FILE
        bvpu_utils.c
        bvpu_utils.h
        jk_common.h
        jk_conn_tcp.c
        jk_conn_tcp.h
        jk_list.c
        jk_list.h
        rt_print.c
        rt_print.h
        rt_server_tcp.c
        rt_server_tcp.h
        XN_Queue.h)

set(SOURCE_CRYPTO_FILE
        crypto/polaraes.c
        crypto/aeslib.c
        crypto/aeslib.h
        crypto/base64.c
        crypto/base64.h
        crypto/polaraes.h)

set(SOURCE_JSON_FILE
        json/json.c
        json/json.h
        json/llrb.h)

set(BASE_FILES
        ${SOURCE_COMMON_FILE} ${SOURCE_CRYPTO_FILE} ${SOURCE_JSON_FILE} jk_conn_udp.c jk_conn_udp.h)


set(DEMO_CONN_FILE
        demo/demo_conn.c)

set(DEMO_CRYPTO_FILE
        demo/demo_crypto.c)

set(DEMO_JSON_FILE
        demo/demo_json.c)

set(DEMO_TCP_FILE
        demo/demo_tcp.c)

set(DEMO_UTILS_FILE
        demo/demo_utils.c)

set(DEMO_UDP_FILE
        demo/demo_udp.c)

set(DEMO_ISPROGRAM_RUNNING_FILE
        demo/isProgramRunning.c)

set(KFCLIENTPING
        demo/kfclientping.c)

add_library(kfcommon STATIC ${BASE_FILES})
target_link_libraries(kfcommon m libzlog)

link_directories(${CMAKE_BINARY_DIR}/common)

add_executable(demo_conn ${DEMO_CONN_FILE})
target_link_libraries(demo_conn kfcommon)

add_executable(demo_crypto ${DEMO_CRYPTO_FILE})
target_link_libraries(demo_crypto kfcommon m)

add_executable(demo_json ${DEMO_JSON_FILE})
target_link_libraries(demo_json kfcommon m)

add_executable(demo_tcp ${DEMO_TCP_FILE})
target_link_libraries(demo_tcp kfcommon pthread dl)

add_executable(demo_udp ${DEMO_UDP_FILE})
target_link_libraries(demo_udp kfcommon)

add_executable(demo_utils ${DEMO_UTILS_FILE})
target_link_libraries(demo_utils kfcommon m)

add_executable(isProgramRunning ${DEMO_ISPROGRAM_RUNNING_FILE})
target_link_libraries(isProgramRunning kfcommon)

add_executable(kfclientping ${KFCLIENTPING})
target_link_libraries(kfclientping kfcommon)

add_executable(demo_remove_m demo_remove_m.c)

add_subdirectory(unixsocket)
add_subdirectory(libconfig)
add_subdirectory(zlog)