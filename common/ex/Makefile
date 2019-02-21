
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJS= $(obj-y:%=$(OBJDIR)/%)
OBJS_CPP= $(obj-cpp-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.c,%,$(obj-demo-y))
DEMO_CPP_EXEC=$(patsubst %.cpp,%,$(obj-demo-cpp-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)/json
	@mkdir -p $(OBJDIR)/crypto
	@mkdir -p $(OBJDIR)/unixsocket
	@mkdir -p $(OBJDIR)/openssl-aes/src/

$(OBJS):$(OBJDIR)/%.o:%.c
	@echo "\t $(CC) \t $^"
	$(Q) $(CC) -o $@ -c $^ $(CFLAGS) $(LINKFLAGS)

$(OBJS_CPP):$(OBJDIR)/%.o:%.cpp
	@echo "\t $(CXX) \t $^"
	$(Q) $(CXX) -o $@ -c $^ $(CFLAGS) $(LINKFLAGS)

$(BUILDIN): $(OBJS) $(OBJS_CPP)
	$(Q) $(LD) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(CC) \t $@"
	$(Q) $(CC) -o $@ $(patsubst %,%.c,$@) $(BUILDIN) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

$(DEMO_CPP_EXEC):$(obj-demo-cpp-y)
	@echo "\t $(CXX) \t $@"
	$(Q) $(CXX) -o $@ $(patsubst %,%.cpp,$@) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS) $(OBJS_CPP) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

