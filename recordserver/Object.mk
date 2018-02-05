
obj-dep-y = ../vdev/$(OBJDIR)/build-in.o
obj-dep-y += ../common/$(OBJDIR)/build-in.o
obj-dep-y += ../codec/$(OBJDIR)/build-in.o
obj-y = process.o

CFLAGS+=-I../common -I../common/unixsocket
CFLAGS+=-I../vdev -I../codec

LINKFLAGS+=-L/opt/data/libs/$(OS)/lib -lx264

obj-demo-y = jkrecord.c

