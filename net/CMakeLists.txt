
project(net)

set(USOCK_FILE usock.c usock.h)

if(JKTEST)
    set(USOCK_DEMO_FILE usock_demo.c)
endif()

add_library(net ${USOCK_FILE})

if (JKTEST)
    add_executable(usock_demo ${USOCK_DEMO_FILE} ${USOCK_FILE})
    target_link_libraries(usock_demo net)
endif()