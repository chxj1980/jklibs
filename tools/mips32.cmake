
set(CMAKE_SYSTEM_NAME linux)
set(ARCH mips32)
set(CMAKE_C_COMPILER mips-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER mips-linux-gnu-g++)
set(X264_PATH /opt/data/libs/x264/${ARCH})
set(X265_PATH /opt/data/libs/x265/${ARCH})
set(OPENCV_PATH /opt/data/libs/opencv-4.0.1/${ARCH})
add_definitions(-std=c++11)
