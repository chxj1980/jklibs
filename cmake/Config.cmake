
if(NOT BASE_EXTERN)
    set(BASE_EXTERN /svn/public/lib/)
endif()

if(NOT PLATFORM)
    set(PLATFORM x64)
endif()

message(STATUS "system name: ${CMAKE_SYSTEM_NAME}")

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

if (NOT MINGW)
set(CMAKE_C_FLAGS -fPIC)
set(CMAKE_CXX_FLAGS -fPIC)
endif()

option(WINDOWS "windows platform" OFF)
option(LINUX "linux platform" ON)
option(COREI7 "corei7 platform" OFF)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    MESSAGE(STATUS "Force to Darwin")
    set(PLATFORM "Darwin")
    set(LINUX OFF)
    set(DARWIN ON)
endif()

message(STATUS "PLATFORM ${PLATFORM}")
if ("${PLATFORM}" STREQUAL "x64")
    set(LINUX ON)
elseif ("${PLATFORM}" STREQUAL "CentOS-x64")
elseif ("${PLATFORM}" STREQUAL "corei7")
    set(COREI7 ON)
    set(LINUX OFF)
elseif ("${PLATFORM}" STREQUAL "Win64")
    set(LINUX OFF)
    set(WINDOWS ON)
endif ()

if(LINUX)
    message(STATUS "linux platform")
    add_definitions(-DLINUX -Wreturn-type)
elseif(WINDOWS)
    MESSAGE(STATUS "windows platform")
    add_definitions(-DWINDOWS -DWin64)
    include_directories("C:\\proj\\code\\opencv-3.2.0\\opencv\\build\\include")
elseif(COREI7)
    message(STATUS "corei7 platform")
    add_definitions(-DCOREI7)
elseif(DARWIN)
    message(STATUS "Darwin platform")
    add_definitions(-D__APPLE)
endif()

if (MINGW)
    add_definitions(-D_MINGW)
    add_definitions(-DLINUX -Wreturn-type)
    set(OPENCV_INCLUDE_DIR D:\\data\\work\\source\\library\\opencv-3.2.0\\libs\\include)
    set(OPENCV_LIB_DIR D:\\data\\work\\source\\library\\opencv-3.2.0\\lib\\Debug)
endif()

include_directories(${OPENCV_INCLUDE_DIR})
link_directories(${OPENCV_LIB_DIR})

option(CONCLUDE_STATIC "if conclude static to static libs" ON)

### Comment it only when we know where will get harm
if (NOT WINDOWS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
endif()

## Only used for conclude extern static libs to project
set(TMP_AR_SCRIPT arstatic.ar)

if("${PLATFORM}" STREQUAL "corei7")
    set(CM_LINKS uuid)
endif()

macro(find_qt)
    set(CMAKE_AUTOMOC ON)
    if(WINDOWS)
        message(STATUS "Qt Window platform")
        set(CUSTOM_QT_BASE_PATH "C:/Qt/Qt5.5.1/5.5/msvc2012/lib/cmake")

        if (NOT MINGW)
            add_definitions(-DWIN32)
        endif()
    elseif(LINUX)
        message(STATUS "Qt linux platform")
        set(CUSTOM_QT_BASE_PATH "/opt/data/data/work/Qt5.5.1/5.5/gcc_64")
    elseif (COREI7)
        message(STATUS "Qt Corei7 platform")
        set(CUSTOM_QT_BASE_PATH "/opt/poky/2.0.2/sysroots/corei7-64-poky-linux/usr")
        set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS $ENV{OE_QMAKE_PATH_HOST_BINS})
    elseif (DARWIN)
        message(STATUS "Qt Darwin platform")
        set(CUSTOM_QT_BASE_PATH /Users/jmdvirus/jmd/proj/Qt5.5.1/5.5/clang_64/lib)
    endif ()

        if (NOT DARWIN)
            message(STATUS "Qt: QtBasePath: ${CUSTOM_QT_BASE_PATH}")
    set(CMAKE_MODULE_PATH ${CUSTOM_QT_BASE_PATH}/lib/cmake)
    set(CMAKE_PREFIX_PATH ${CUSTOM_QT_BASE_PATH}/lib/cmake)
    include_directories(${CUSTOM_QT_BASE_PATH}/include)
    link_directories(${CUSTOM_QT_BASE_PATH}/lib)
endif()

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

#add_extern_libs(http-parser "" ${BASE_EXTERN} ${PLATFORM})
#set(http-parser_libstatic ${http-parser_base}/lib/libhttp-parser${D_SUFFIX}.a)
#add_extern_sublibs(apr apr-iconv "1.5.2" ${BASE_EXTERN} ${PLATFORM})
