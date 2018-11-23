
USECC=gcc

DEMO_CFLAGS += -Icommon

obj-dep-$(VDEV)=../common/$(OBJDIR)/build-in.o

obj-vdev-$(VDEV)=vdev.o operation.o

obj-vdev-demo-$(VDEV) = demo.c

