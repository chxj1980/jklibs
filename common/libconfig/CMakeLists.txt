
project(libconfig)

set(SOURCE_LIBCONFIG_FILE
        libconfig.c
        libconfig.h
        grammar.c
        grammar.h
        parsectx.h
        scanctx.c
        scanctx.h
        scanner.c
        scanner.h
        strbuf.c
        strbuf.h
        )

set(DEMO_LIBCONFIG_FILE
        libconfig_demo.c)

add_library(libconfig STATIC ${SOURCE_LIBCONFIG_FILE})
#target_link_libraries(libconfig)

#link_directories(${CMAKE_BINARY_DIR}/common)

add_executable(demo_libconfig ${DEMO_LIBCONFIG_FILE})
target_link_libraries(demo_libconfig libconfig kfcommon)

