
#USECC=gcc
COMPILE=$(CXX)

obj-dep-$(OPENAV)=../common/$(OBJDIR)/build-in.o

CFLAGS-$(OPENCV) += -I../codec
CXXFLAGS-y+=-I. -I../common -I../codec
OPENCV_VER=4.0.1
OPENCV_INC_POST=opencv4
CXXFLAGS-$(OPENCV)+=-I${THIRD_BASEPATH}/opencv-${OPENCV_VER}/$(OS)/include/${OPENCV_INC_POST}

LIBS-$(OPENCV)+=-L${THIRD_BASEPATH}/opencv-${OPENCV_VER}/$(OS)/lib
LIBS-$(OPENCV)+= \
	-lopencv_dnn -lopencv_gapi -lopencv_ml -lopencv_objdetect \
	-lopencv_photo \
	-lopencv_stitching -lopencv_video -lopencv_calib3d -lopencv_features2d \
	-lopencv_flann -lopencv_highgui -lopencv_videoio \
	-lopencv_imgcodecs -lopencv_imgproc -lopencv_core

# ../../lib/libopencv_dnn.so.4.0.1 ../../lib/libopencv_gapi.so.4.0.1 ../../lib/libopencv_ml.so.4.0.1 ../../lib/libopencv_objdetect.so.4.0.1 ../../lib/libopencv_photo.so.4.0.1 ../../lib/libopencv_stitching.so.4.0.1 ../../lib/libopencv_video.so.4.0.1 ../../lib/libopencv_calib3d.so.4.0.1 ../../lib/libopencv_features2d.so.4.0.1 ../../lib/libopencv_flann.so.4.0.1 ../../lib/libopencv_highgui.so.4.0.1 ../../lib/libopencv_videoio.so.4.0.1 ../../lib/libopencv_imgcodecs.so.4.0.1 ../../lib/libopencv_imgproc.so.4.0.1 ../../lib/libopencv_core.so.4.0.1 

ifeq (x"$(OS)", x"hi3518")
DEMO_CFLAGS+= -D__NO_HIGHGUI
LINKPATH+= ${THIRD_BASEPATH}/fixlowgcc/lib/liblinux-atomic.a
endif

obj-openavc-$(OPENCV) += ../codec/cmyuv.o
obj-openav-$(OPENCV) += BaseOperation.o  CVFaceDetect.o  DrawSomething.o
obj-openav-demo-$(OPENCV) += \
	catch_imgs.cpp \
	face_detect.cpp \
	stitching.cpp

obj-openav-demo-$(OPENCV) += \
	stitching_detailed.cpp

obj-openav-demo-$(OPENCV) += \
	facedetect.cpp

CFLAGS-$(OPENGL) += -Iopenav/gl
CFLAGS-$(OPENGLQT) += -D__OPENGL_QT
CFLAGS-$(OPENGLUT) += -D__OPENGL_GLUT
LIBS-$(OPENGL) += -lGL -lGLEW -lglfw
LIBS-$(OPENGLUT) += -lglut

obj-openav-$(OPENGL) += \
	gl/OpenGLBase.o \
	gl/glbase.o

obj-openav-$(OPENGLQT) += \
	gl/OpenGLQt.o \
	gl/OpenGLQtWidget.o

obj-openav-$(OPENGLUT) += \
	gl/OpenGLGLUT.o

obj-openav-demo-$(OPENGL) += \
	gl/demo.cpp

