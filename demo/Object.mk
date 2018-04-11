
#USECC=gcc

DEMO_CFLAGS+=-I../common

obj-dep-y=../common/$(OBJDIR)/build-in.o

obj-demo-y = demo_small.c  demo_jkbytes.c base_demo.c
obj-demo-cpp-y += demo_cplusplus_list.cpp

