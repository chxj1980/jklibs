
obj-dep-y = ../vdev/$(OBJDIR)/build-in.o
obj-dep-y += ../common/$(OBJDIR)/build-in.o
obj-dep-y += ../codec/$(OBJDIR)/build-in.o
obj-recordserver-y = process.o

CFLAGS+=-I../common -I../common/unixsocket
CFLAGS+=-I../vdev -I../codec

LINKFLAGS+=-L/opt/data/libs/$(OS)/lib -lx264

obj-recordserver-demo-y = jkrecord.c

