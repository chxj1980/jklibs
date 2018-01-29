
USECC=gcc

obj-dep-$(VDEV)=../common/$(OBJDIR)/build-in.o

obj-$(VDEV)=vdev.o operation.o

obj-demo-$(VDEV) = demo.c

