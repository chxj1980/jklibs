
#USECC=gcc
COMPILE=$(CXX)

obj-dep-$(OPENAV)=../common/$(OBJDIR)/build-in.o

CXXFLAGS-y+=-I. -I../common
CXXFLAGS-$(OPENCV)+=-I${THIRD_BASEPATH}/opencv-3.4.0/$(OS)/include

LDFLAGS-$(OPENCV)+=-L${THIRD_BASEPATH}/opencv-3.4.0/$(OS)/lib
LDFLAGS-$(OPENCV)+=-lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs 
LDFLAGS-$(OPENCV)+=-lopencv_ml -lopencv_videoio -lopencv_highgui -lopencv_core \
	-lopencv_stitching

ifeq (x"$(OS)", x"hi3518")
DEMO_CFLAGS+= -D__NO_HIGHGUI
LINKPATH+= ${THIRD_BASEPATH}/fixlowgcc/lib/liblinux-atomic.a
endif

obj-openav-$(OPENCV) += BaseOperation.o  CVFaceDetect.o  DrawSomething.o

obj-openav-demo-$(OPENCV) += face_detect.cpp \
	stitching.cpp

CFLAGS-$(OPENGL) += -Iopenav/gl
CFLAGS-$(OPENGLQT) += -D__OPENGL_QT
CFLAGS-$(OPENGLUT) += -D__OPENGL_GLUT
LDFLAGS-$(OPENGL) += -lGL -lGLEW -lglfw
LDFLAGS-$(OPENGLUT) += -lglut

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

