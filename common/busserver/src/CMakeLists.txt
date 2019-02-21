cmake_minimum_required(VERSION 3.13)
include_directories(../mongoose
        ../../)

set(SOURCE_FILE
        busserver_ex.c)

set(EXT_FILE
        ../mongoose/mongoose.c
        ../../cm_print.c)

add_executable(cmbusclient
        cmbusclient.c
        ${SOURCE_FIEL})

add_executable(cmbusserver
        cmbusserver.c
        ${SOURCE_FILE})
