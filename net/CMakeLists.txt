
project(net)

set(USOCK_FILE usock.c usock.h)

set(USOCK_DEMO_FILE usock_demo.c)

add_library(net ${USOCK_FILE})

add_executable(usock_demo ${USOCK_DEMO_FILE} ${USOCK_FILE})
target_link_libraries(usock_demo net)