
project(utils)

set(UTILS_FILE utils.c utils.h base64.c)

set(ULOOP_FILE uloop.c uloop.h)

add_library(utils ${UTILS_FILE} ${ULOOP_FILE})

if(JKTEST)
    set(UTILS_DEMO_FILE utils_demo.c)
    set(UTILS_DEMO_NAME utils_demo)
    add_executable(${UTILS_DEMO_NAME} ${UTILS_DEMO_FILE} ${UTILS_FILE})
    target_link_libraries(${UTILS_DEMO_NAME} utils)

    set(ULOOP_DEMO_FILE uloop_demo.c)
    set(ULOOP_DEMO_NAME uloop_demo)
    add_executable(${ULOOP_DEMO_NAME} ${ULOOP_DEMO_FILE} ${ULOOP_FILE})
    target_link_libraries(${UTILS_DEMO_NAME} utils)
endif()