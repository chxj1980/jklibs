
project(libzlog)

set(SOURCE_ZLOG_FILE
        buf.c
        buf.h
        category.c
        category.h
        category_table.c
        category_table.h
        conf.c
        conf.h
        event.c
        event.h
        fmacros.h
        format.c
        format.h
        level.c
        level.h
        level_list.c
        level_list.h
        mdc.c
        mdc.h
        record.c
        record.h
        record_table.c
        record_table.h
        rotater.c
        rotater.h
        rule.c
        rule.h
        spec.c
        spec.h
        thread.c
        thread.h
        version.h
        zc_arraylist.c
        zc_arraylist.h
        zc_profile.c
        zc_profile.h
        zc_util.c
        zc_util.h
        zc_xplatform.h
        zlog.c
        zlog.h
        zc_defs.h
        zc_hashtable.c
        zc_hashtable.h
        )

set(DEMO_ZLOG_FILE
        zlog_demo.c)

add_library(libzlog STATIC ${SOURCE_ZLOG_FILE})
target_link_libraries(libzlog pthread)

#link_directories(${CMAKE_BINARY_DIR}/common)

add_executable(demo_zlog ${DEMO_ZLOG_FILE})
target_link_libraries(demo_zlog libzlog)

