
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.c,%,$(obj-demo-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))
DEMO_CPP_EXEC=$(patsubst %.cpp,%,$(obj-demo-cpp-y))
DEMO_CPP_EXEC_GEN=$(DEMO_CPP_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

.PHONY: demo

all: deps $(DEMO_EXEC) $(DEMO_CPP_EXEC)

deps:
	@mkdir -p $(OBJDIR)

$(DEMO_EXEC):$(obj-demo-y)
	@echo "\t $(CC) \t $@"
	$(Q) $(CC) -o $@-$(OS) $(patsubst %,%.c,$@) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

$(DEMO_CPP_EXEC):$(obj-demo-cpp-y)
	@echo "\t $(CXX) \t $@"
	$(Q) $(CXX) -o $@-$(OS) $(patsubst %,%.cpp,$@) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN) $(DEMO_CPP_EXEC)

distclean:
	rm -rf $(OBJDIR)

