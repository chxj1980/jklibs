
#USECC=gcc

DEMO_CFLAGS+=-I../common

obj-dep-y=../common/$(OBJDIR)/build-in.o

obj-demo-y = demo_small.c demo_jkbytes.c base_demo.c
obj-demo-y += demo_thread.c
obj-demo-y += demo_unix_server.c demo_unix_client.c
obj-demo-cpp-y += demo_cplusplus_list.cpp

