
USECC=gcc

obj-dep-$(CODEC) = ../common/$(OBJDIR)/build-in.o

obj-$(CODEC) = codec.o encoder.o

obj-demo-$(CODEC) = demo.c

