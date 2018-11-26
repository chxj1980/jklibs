
#USECC=gcc
COMPILE=$(CXX)

obj-dep-$(OPENAV)=../common/$(OBJDIR)/build-in.o

CFLAGS-y+=-I. -I../common
CFLAGS-$(OPENCV)+=-I${THIRD_BASEPATH}/opencv-3.1.0/$(OS)/include

LDFLAGS-$(OPENCV)+=-L${THIRD_BASEPATH}/opencv-3.1.0/$(OS)/lib
LDFLAGS-$(OPENCV)+=-lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs 
LDFLAGS-$(OPENCV)+=-lopencv_ml -lopencv_videoio -lopencv_highgui -lopencv_core

ifeq (x"$(OS)", x"hi3518")
DEMO_CFLAGS+= -D__NO_HIGHGUI
LINKPATH+= ${THIRD_BASEPATH}/fixlowgcc/lib/liblinux-atomic.a
endif

obj-openav-$(OPENCV) += BaseOperation.o  CVFaceDetect.o  DrawSomething.o

obj-openav-demo-$(OPENCV) += face_detect.cpp

CFLAGS-$(OPENGL) += -Iopenav/gl
CFLAGS-$(OPENGLQT) += -D__OPENGL_QT
LDFLAGS-$(OPENGL) += -lGL -lGLEW -lglut

obj-openav-$(OPENGL) += \
	gl/OpenGLGLUT.o \
	gl/OpenGLBase.o

obj-openav-$(OPENGLQT) += \
	gl/OpenGLQt.o \
	gl/OpenGLQtWidget.o

obj-openav-demo-$(OPENGL) += \
	gl/main.cpp

