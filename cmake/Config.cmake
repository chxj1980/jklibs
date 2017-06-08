
if(NOT BASE_EXTERN)
    set(BASE_EXTERN /svn/public/lib/)
endif()

if(NOT PLATFORM)
    set(PLATFORM x64)
endif()

## This structure like this
## -
## |x64
##   | usr/bin
##   | usr/lib
## There are no header files
## All put to one place.
set(FILESYS ${BASE_EXTERN}/../filesys)

option(DEBUG_M OFF)

## Define this for sometime IDE with transfer Debug args
## We first use Debug mode
## We first use DEBUG_M arg.
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBUG_M yes)
    message(STATUS "IDE define Debug")
else()
    message(STATUS "IDE define Release")
endif()

if(DEBUG_M)
    set(CMAKE_BUILD_TYPE Debug)
    set(D_SUFFIX "-Debug")
    message(STATUS "Compile with debug mode.")
else()
    message(STATUS "Compile with release mode.")
endif()

## You didn't give position to install, so will clear default position,
## let you can use make install DESTDIR=......
if(NOT INSTALL_TO)
    set(CMAKE_INSTALL_PREFIX /)
endif()

if (${PLATFORM} STREQUAL "corei7")
    set(PJ_SUFFIX -x86_64-pc-linux-gnu)
else()
    set(PJ_SUFFIX -x86_64-unknown-linux-gnu)
endif()

set(CMAKE_C_FLAGS -fPIC)
set(CMAKE_CXX_FLAGS -fPIC)

option(WINDOWS "windows platform" OFF)
option(LINUX "linux platform" ON)
option(COREI7 "corei7 platform" OFF)

message(STATUS "PLATFORM ${PLATFORM}")
if ("${PLATFORM}" STREQUAL "x64")
    set(LINUX ON)
elseif ("${PLATFORM}" STREQUAL "CentOS-x64")
elseif ("${PLATFORM}" STREQUAL "corei7")
    set(COREI7 ON)
    set(LINUX OFF)
endif ()

if(LINUX)
    message(STATUS "linux platform")
    add_definitions(-DLINUX -Wreturn-type)
elseif(WINDOWS)
    MESSAGE(STATUS "windows platform")
    add_definitions(-DWINDOWS)
elseif(COREI7)
    message(STATUS "corei7 platform")
    add_definitions(-DCOREI7)
endif()

option(CONCLUDE_STATIC "if conclude static to static libs" ON)

### Comment it only when we know where will get harm
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

## Only used for conclude extern static libs to project
set(TMP_AR_SCRIPT arstatic.ar)

if("${PLATFORM}" STREQUAL "corei7")
    set(CM_LINKS uuid)
endif()

macro(find_qt)
    if(WINDOWS)
        message(STATUS "Qt Window platform")
        set(CUSTOM_QT_BASE_PATH "C:/Qt/Qt5.5.1/5.5/msvc2012/lib/cmake")

        add_definitions(-DWIN32)
    elseif(LINUX)
        message(STATUS "Qt linux platform")
        set(CUSTOM_QT_BASE_PATH "/opt/data/data/work/Qt5.5.1/5.5/gcc_64")
    elseif (COREI7)
        message(STATUS "Qt Corei7 platform")
        set(CUSTOM_QT_BASE_PATH "/opt/poky/2.0.2/sysroots/corei7-64-poky-linux/usr")
        set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS $ENV{OE_QMAKE_PATH_HOST_BINS})
    endif ()

    message(STATUS "Qt: QtBasePath: ${CUSTOM_QT_BASE_PATH}")

    set(CMAKE_MODULE_PATH ${CUSTOM_QT_BASE_PATH}/lib/cmake)
    set(CMAKE_PREFIX_PATH ${CUSTOM_QT_BASE_PATH}/lib/cmake)
    include_directories(${CUSTOM_QT_BASE_PATH}/include)
    link_directories(${CUSTOM_QT_BASE_PATH}/lib)

    find_package(Qt5Core)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    find_package(Qt5WebKitWidgets)
    find_package(Qt5OpenGL)

    set(LINK_QT5 Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL Qt5::WebKitWidgets)
endmacro()

## name_base will be postion to baseposition/name-version/plat/
## name_base/include will be add to include directories
## name_base/lib will be add to lib directories
## You can set (name_libstatic) for static libfullname
macro(add_extern_libs name version baseposition plat)
    set(fullname ${name})
    if ("${version}" STREQUAL "")
    else()
        set(fullname ${name}-${version})
    endif()
    set(${name}_base ${baseposition}/${fullname}/${plat})
    set(basepath ${baseposition}/${fullname}/${plat})
    include_directories(${basepath}/include)
    link_directories(${basepath}/lib)
endmacro()

## name_base will be position to baseposition/name/subname-version/plat
## same with add_extern_libs otherwise
macro(add_extern_sublibs name subname version baseposition plat)
    set(fullname ${name}/${subname})
    if ("${version}" STREQUAL "")
    else()
        set(fullname ${name}/${subname}-${version})
    endif()
    set(${subname}_base ${baseposition}/${fullname}/${plat})
    set(basepath ${baseposition}/${fullname}/${plat})
    include_directories(${basepath}/include)
    link_directories(${basepath}/lib)
endmacro()

include_directories(${BASE_EXTERN}/../../library)

add_extern_libs(http-parser "" ${BASE_EXTERN} ${PLATFORM})
set(http-parser_libstatic ${http-parser_base}/lib/libhttp-parser${D_SUFFIX}.a)

add_extern_libs(libevent 2.0.22 ${BASE_EXTERN} ${PLATFORM})
set(libevent_libstatic ${libevent_base}/lib/libevent.a)

add_extern_libs(ace 6.4.1 ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(boost 1.62.0 ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(qt-gstreamer "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(pjproject "2.6" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(mysql "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(sqlite3 "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(zlib "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(ffmpeg "94a52417" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(resiprocate "1.6" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(opencv "3.2.0" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(activemq-cpp-library "3.9.3" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(protobuf "2.6.1" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(apr apr "1.5.2" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(apr apr-iconv "1.5.2" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(apr apr-util "1.5.2" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(curl "7.53.1" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(openssl "0.9.8a" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(tinyxpath "1.3.1" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(mongoose "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(yami libva "1.5.0" ${BASE_EXTERN} ${PLATFORM})

add_extern_sublibs(yami libyami 1.1 ${BASE_EXTERN} ${PLATFORM})
include_directories(${libyami_base}/include/libyami)
add_extern_libs(libbsd "0.8.3" ${BASE_EXTERN} ${PLATFORM})

add_extern_sublibs(AGS AGS "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(AGS AGSC "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(AGS DTC "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(AGS MCC "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(AGS xServer "" ${BASE_EXTERN} ${PLATFORM})

add_extern_libs(AGSCom "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(DMAgent "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(MOSBase "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(OnvifShell "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(SipShell "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(XPacket "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(AudioDecEnc "" ${BASE_EXTERN} ${PLATFORM})

add_extern_libs(AviLib "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(httpapi "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(libsav "" ${BASE_EXTERN} ${PLATFORM})
add_extern_libs(StreamConverter "" ${BASE_EXTERN} ${PLATFORM})

add_extern_libs(AGSDisplay "" ${BASE_EXTERN} ${PLATFORM})

## Choose current project include files first than in public
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SP_Com)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/DBS_Kernel)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/DB_Adapter)

add_extern_sublibs(SP SPCom "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(SP SQLite_Filter "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(SP DBAdapter "" ${BASE_EXTERN} ${PLATFORM})
add_extern_sublibs(SP DBSKernel "" ${BASE_EXTERN} ${PLATFORM})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/MQ_Com)
add_extern_sublibs(CustomEXS MQ_Com "" ${BASE_EXTERN} ${PLATFORM})

include_directories(${BASE_EXTERN}/)

include_directories(${apr_base}/include/apr-1)
include_directories(${apr-iconv_base}/include/apr-1)
include_directories(${apr-util_base}/include/apr-1)
set(apr_libstatic ${apr_base}/lib/libapr-1.a)
set(apr-iconv_libstatic ${apr-iconv_base}/lib/libapriconv-1.a)
set(apr-util_libstatic ${apr-util_base}/lib/libaprutil-1.a)
set(curl_libstatic ${curl_base}/lib/libcurl.a)
set(ssl_libstatic ${openssl_base}/lib/libssl.a)
set(crypto_libstatic ${openssl_base}/lib/libcrypto.a)
set(tinyxpath_libstatic ${tinyxpath_base}/lib/libtinyxpath.a)
set(mongoose_libstatic ${mongoose_base}/lib/libmongoose${D_SUFFIX}.a)

set(OnvifShell_libstatic ${OnvifShell_base}/lib/libOnvifShell${D_SUFFIX}.a)
set(SipShell_libstatic ${SipShell_base}/lib/libSipShell${D_SUFFIX}.a)
set(MOSBase_libstatic ${MOSBase_base}/lib/libMOSBase${D_SUFFIX}.a)
set(XPacket_libstatic ${XPacket_base}/lib/libXPacket${D_SUFFIX}.a)
set(AudioDecEnc_libstatic ${AudioDecEnc_base}/lib/libAudioDecEnc${D_SUFFIX}.a)
set(StreamConverter_libstatic ${StreamConverter_libstatic}/lib/libStreamConverter${D_SUFFIX}.a)

include_directories(${activemq-cpp-library_base}/include/activemq-cpp-3.9.3)
set(activemq_libstatic ${activemq-cpp-library_base}/lib/libactivemq-cpp.a)

set(ares_libstatic ${resiprocate_base}/lib/libares.a)
set(ARES_STATIC ${ares_libstatic})

set(httpapi_libstatic ${httpapi_base}/lib/libhttpapi${D_SUFFIX}.a)
set(httpapi_static ${httpapi_libstatic})

set(agscom_libstatic ${AGSCom_base}/lib/libAGSCom${D_SUFFIX}.a)
set(AGSCOM_STATIC ${agscom_libstatic})

set(dmagent_libstatic ${DMAgent_base}/lib/libDMAgent${D_SUFFIX}.a)
set(DMAGENT_STATIC ${dmagent_libstatic})

set(pj_libstatic ${pjproject_base}/lib/libpj${PJ_SUFFIX}.a)
set(PJ_STATIC ${pj_libstatic})
set(pjutil_libstatic ${pjproject_base}/lib/libpjlib-util${PJ_SUFFIX}.a)
set(PJUTIL_STATIC ${pjutil_libstatic})

set(spcom_libstatic ${SPCom_base}/lib/libSPCom${D_SUFFIX}.a)
set(SPCOM_STATIC ${CMAKE_BINARY_DIR}/SP_Com/libSPCom${D_SUFFIX}.a)

set(MQ_COM_static ${CMAKE_BINARY_DIR}/MQ_Com/libMQ_Com${D_SUFFIX}.a)
set(MQCOM_STATIC_INNER ${MQ_COM_static})

set(MQ_Com_libstatic ${MQ_Com_base}/lib/libMQ_Com${D_SUFFIX}.a)

set(sqlite_filter_libstatic ${SQLite_Filter_base}/lib/libSQLite_Filter${D_SUFFIX}.a)
set(SQLITE_FILTER_STATIC ${CMAKE_BINARY_DIR}/SQLite_Filter/libSQLite_Filter${D_SUFFIX}.a)

set(dbadapter_libstatic ${DBAdapter_base}/lib/libDBAdapter${D_SUFFIX}.a)
set(DBADAPTER_STATIC ${CMAKE_BINARY_DIR}/DB_Adapter/libDBAdapter${D_SUFFIX}.a)

set(dbskernel_libstatic ${DBSKernel}/lib/libDBSKernel${D_SUFFIX}.a)
set(DBSKERNEL_STATIC ${CMAKE_BINARY_DIR}/DBS_Kernel/libDBSKernel${D_SUFFIX}.a)

