
project(demo)

include(../cmake/Config.cmake)

option(WITH_OPENGL "OpenGL" ON)
option(WITH_OPENCV "OpenCV" ON)
option(WITH_QT "With QT" OFF)

if(WINDOWS)
include_directories(D:/data/proj/jklibs/windows/include)
link_directories(D:/data/proj/jklibs/windows/lib)
else()
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../jklibs/windows/jklibs/jklibs)
endif()

if(JKTEST)
    include_directories(/usr/include)
#    set(ULOOP_DEMO_FILE uloop_demo.c)
#    set(ULOOP_DEMO_NAME uloop_demo)
#    add_executable(${ULOOP_DEMO_NAME} ${ULOOP_DEMO_FILE})
#    target_link_libraries(${ULOOP_DEMO_NAME} ubox)

#    add_executable(jsonc_demo jsonc_demo.c)
#    target_link_libraries(jsonc_demo json-c)

#    add_executable(list_demo list_demo.c)
#    target_link_libraries(list_demo ubox)

    add_executable(base_demo base_demo.c)
    target_link_libraries(base_demo kfcommon)

    add_executable(netlink_demo netlink_demo.c)
    target_link_libraries(netlink_demo kfcommon)

    add_executable(demo_small demo_small.c)

        if (NOT COREI7)

        include_directories(/opt/data/data/source/opengl/opengles3-book/Common/Include)
        link_directories(/opt/data/data/source/opengl/opengles3-book/build/Common)
            add_executable(demo_opengles demo_opengles.c)
        target_link_libraries(demo_opengles Common GLESv2 EGL SM ICE X11 Xext)
        endif()

        add_executable(demo_psstream psstream/demo_psstream.cpp
                psstream/PSPacket.h
                psstream/streamdef.h)
        add_executable(demo_ps_es psstream/demo_ps_es.cpp
                psstream/PS2ES.cpp psstream/PS2ES.h)

#    add_executable(demo_ncurses demo_ncurses.c)
endif()

include_directories(op_stream)

set(OP_STREAM_FILE
        op_stream/h264.cpp)

add_executable(h264_parse op_stream/h264_parse.cpp ${OP_STREAM_FILE})

add_executable(demo_cplus_list demo_cplusplus_list.cpp)

add_executable(demo_media_option media_option.c)

if (APPLE)
    add_definitions(-D__DARWIN)
    include_directories(/Users/jmdvirus/jmd/proj/work/source/public/lib/opencv-3.2.0/-x64/include)
    link_directories(/Users/jmdvirus/jmd/proj/work/source/public/lib/opencv-3.2.0/-x64/lib)
    set(OPENCV_LINK_LIBS opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_objdetect opencv_videoio)
#    set(OPENCL_LINK_LIBS OpenCL)
    set(OPENGL_NAME OpenGL)
    set(GLUT_NAME GLUT)
elseif (WINDOWS)
    include_directories("J:\\proj\\app\\opencv-3.2.0\\opencv\\build\\include")
    link_directories("J:\\proj\\app\\opencv-3.2.0\\opencv\\build\\x64\\vc14\\lib")
    set(OPENCV_LINK_LIBS opencv_world320d)
    set(OPENCL_LINK_LIBS OpenCL)
else()
    set(OPENCV_LINK_LIBS opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_objdetect opencv_videoio)
    if (COREI7)
        message(INFO "Poky version")
        set(OPENCV_LINK_LIBS opencv_core opencv_imgproc opencv_imgcodecs opencv_objdetect opencv_videoio)
        add_definitions(-D__NO_HIGHGUI)
    endif()
    set(OPENCL_LINK_LIBS OpenCL)
    include_directories(/opt/intel/opencl/include)
    link_directories(/opt/intel/opencl/lib64)
    set(OPENGL_NAME GL)
    set(GLUT_NAME glut)
endif ()

if (WINDOWS)
add_executable(fd_video opencv/fd_video.cpp)
target_link_libraries(fd_video ${OPENCV_LINK_LIBS})
endif()

if (WITH_OPENCV)
if (LINUX)
    include_directories(/opt/intel/opencl/include)
    link_directories(/opt/intel/opencl/lib64/)
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../windows/jklibs/opencv)

add_executable(opencv_face_detect opencv/face_detect.cpp
        opencv/CVFaceDetect.cpp
        opencv/CVFaceDetect.h
        opencv/BaseOperation.cpp
        opencv/BaseOperation.h
        opencv/DrawSomething.cpp
        opencv/DrawSomething.h
        )
target_link_libraries(opencv_face_detect ${OPENCV_LINK_LIBS} ${OPENCL_LINK_LIBS})

endif()

if (NOT COREI7)

add_executable(opencv_withcl opencv/opencl-opencv-interop.cpp)
target_link_libraries(opencv_withcl ${OPENCV_LINK_LIBS} OpenCL)

add_executable(opencv_withgl opencv/opengl_interop.cpp opencv/winapp.hpp)
target_link_libraries(opencv_withgl ${OPENCV_LINK_LIBS} GL X11)

endif ()

add_executable(demo_readdata readdata/demo_readdata.cpp ../common/unixsocket/rt_unixsocket.c)
target_link_libraries(demo_readdata kfcommon)

option(WITH_OPENCL "opencl function" ON)

macro(add_program dir)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${dir} ${dir}_SOURCE_FILE)
    add_executable(${dir} ${${dir}_SOURCE_FILE})
endmacro()

if (WITH_OPENCL)
    add_program(opencl)
    target_link_libraries(opencl OpenCL)
endif()

if (WITH_OPENGL)
        if (WITH_QT)
            add_definitions(-D__OPENGL_QT)
            message(STATUS "Enable Qt")
            find_qt()
                set(OPENGL_QT_FILE
                opengl/OpenGLQt.cpp opengl/OpenGLQt.h
                        opengl/OpenGLQtWidget.cpp opengl/OpenGLQtWidget.h
                )
                    endif()

    add_executable(opengl opengl/main.cpp
            opengl/OpenGLGLUT.cpp opengl/OpenGLGLUT.h
            opengl/OpenGLBase.cpp opengl/OpenGLBase.h
            ${OPENGL_QT_FILE})
    
if (NOT WINDOWS)
if (NOT COREI7)
    find_library(OPENGL_LIB GL)
    if (NOT OPENGL_LIB)
        message(FATAL_ERROR "No OpenGL, please install")
    endif()
        find_path(GLUT_INCLUDE_DIR NAMES glut.h)
        MESSAGE(STATUS "GLUT dir: ${GLUT_INCLUDE_DIR}")
        include_directories(${GLUT_INCLUDE_DIR})
    find_library(GLUT_LIB glut)
    if (NOT GLUT_LIB)
        message(FATAL_ERROR "No GLUT, please install")
    endif()
    find_library(GLEW_LIB GLEW)
    if (NOT GLEW_LIB)
        message(FATAL_ERROR "No GLEW, please install")
    endif()
else()
    set(OPENGL_LIB GL)
    set(GLUT_LIB glut)
    set(GLEW_LIB GLEW)
endif()
    MESSAGE(STATUS "glew: ${GLEW_LIB}")
    target_link_libraries(opengl ${OPENGL_LIB} ${GLUT_LIB} ${GLEW_LIB})
endif()
endif()

#target_link_libraries(program "-framework CoreFoundation")
#target_link_libraries(program "-framework your_frame_work_name)
#set_target_properties(program PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
