
## kfconfig
project(kfconfig)
set(KFCONFIG_FILE
        kfconfig.c
        kfconfig.h)
set(DEMO_KFCONFIG_FILE
        kfrouter_start.c)

add_library(kfconfig ${KFCONFIG_FILE})

link_directories(${CMAKE_BINARY_DIR})

add_executable(kfrouter_start ${DEMO_KFCONFIG_FILE})
target_link_libraries(kfrouter_start kfconfig kfcommon)