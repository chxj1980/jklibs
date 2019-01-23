
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJS= $(obj-openav-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.cpp,%,$(obj-openav-demo-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

CXXFLAGS += $(CXXFLAGS-y)
LDFLAGS += $(LDFLAGS-y)

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)/gl

$(OBJS):$(OBJDIR)/%.o:%.cpp
	@echo "\t $(COMPILE) \t $^"
	$(Q) $(COMPILE) -o $@ -c $^ $(CXXFLAGS) $(LINKFLAGS)

$(BUILDIN): $(OBJS)
	$(Q) $(LD) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(COMPILE) \t $@"
	$(Q) $(COMPILE) -o $@ $(patsubst %,%.cpp,$(@))  $(BUILDIN) $(OBJ_DEPS) $(CXXFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

distclean:
	rm -rf $(OBJDIR)

