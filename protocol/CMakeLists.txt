## protocol
project(kfprotocol)

set(PROTOCOL_HTTP_FILE
        http/ghttp.c
        http/ghttp.h
        http/ghttp_constants.h
        http/http_base64.c
        http/http_base64.h
        http/http_date.c
        http/http_date.h
        http/http_global.h
        http/http_hdrs.c
        http/http_hdrs.h
        http/http_req.c
        http/http_req.h
        http/http_resp.c
        http/http_resp.h
        http/http_trans.c
        http/http_trans.h
        http/http_uri.c
        http/http_uri.h)

set(PROTOCOL_RASP_FILE
        rasp/kfrasp.c
        rasp/kfrasp.h
        rasp/kfrasp_parse_data.c
        rasp/kfrasp_parse_data.h
        rasp/kfrasp_string.c
        rasp/kfrasp_string.h)

set(DEMO_PROTOCOL_HTTP_FILE
        demo/demo_http.c)
set(DEMO_PROTOCOL_RASP_FILE
        demo/demo_rasp.c)

add_library(kfprotocol STATIC ${PROTOCOL_HTTP_FILE} ${PROTOCOL_RASP_FILE})

link_directories(${CMAKE_BINARY_DIR}/protocol)

add_executable(demo_http ${DEMO_PROTOCOL_HTTP_FILE})
target_link_libraries(demo_http kfprotocol kfcommon kfmd5 m)

add_executable(demo_rasp ${DEMO_PROTOCOL_RASP_FILE})
target_link_libraries(demo_rasp kfprotocol kfcommon kfmd5 m)

