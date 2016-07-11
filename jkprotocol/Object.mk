
obj-dep-y = ../kfmd5/$(OBJDIR)/build-in.o
obj-dep-y += ../common/$(OBJDIR)/build-in.o
obj-y = jkprotocol.o

CFLAGS+=-I../kfmd5/

obj-demo-y = demo_jkprotocol.c

