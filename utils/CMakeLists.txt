
project(utils)

set(UTILS_FILE utils.c utils.h base64.c)

set(UTILS_DEMO_FILE utils_demo.c)

set(UTILS_DEMO_NAME utils_demo)

add_library(utils ${UTILS_FILE})

add_executable(${UTILS_DEMO_NAME} ${UTILS_DEMO_FILE} ${UTILS_FILE})
target_link_libraries(${UTILS_DEMO_NAME} utils)