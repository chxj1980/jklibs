
project(kfmd5)

## kfmd5
set(KFMD5_FILE
        ngx_md5.c
        ngx_md5.h
        )

set(DEMO_KFMD5_FILE
        kfmd5sum.c)

add_library(kfmd5 STATIC ${KFMD5_FILE})

link_directories(${CMAKE_BINARY_DIR}/kfmd5)

add_executable(kfmd5sum ${DEMO_KFMD5_FILE})
target_link_libraries(kfmd5sum kfmd5)