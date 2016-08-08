
project(demo)

if(JKTEST)
    set(ULOOP_DEMO_FILE uloop_demo.c)
    set(ULOOP_DEMO_NAME uloop_demo)
    add_executable(${ULOOP_DEMO_NAME} ${ULOOP_DEMO_FILE})
    target_link_libraries(${ULOOP_DEMO_NAME} ubox)

    add_executable(jsonc_demo jsonc_demo.c)
    target_link_libraries(jsonc_demo json-c)

    add_executable(list_demo list_demo.c)
    target_link_libraries(list_demo ubox)

    add_executable(base_demo base_demo.c)
    target_link_libraries(base_demo kfcommon)
endif()