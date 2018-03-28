
#USECC=gcc
COMPILE=$(CXX)

obj-dep-$(OPENAV)=../common/$(OBJDIR)/build-in.o

CFLAGS+=-I. -I../common
CFLAGS+=-I/opt/lib/opencv-3.1.0/$(OS)/include

LINKPATH+=-L/opt/lib/opencv-3.1.0/$(OS)/lib
LINKPATH+=-lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs 
LINKPATH+=-lopencv_ml -lopencv_videoio -lopencv_highgui -lopencv_core

ifeq (x"$(OS)", x"hi3518")
DEMO_CFLAGS+= -D__NO_HIGHGUI
LINKPATH+= /opt/lib/fixlowgcc/lib/liblinux-atomic.a
endif

obj-$(OPENAV)=BaseOperation.o  CVFaceDetect.o  DrawSomething.o

obj-demo-$(OPENAV) = face_detect

