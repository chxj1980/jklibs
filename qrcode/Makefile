
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJS= $(obj-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
#DEMO_EXEC=$(patsubst %.cpp,%,$(obj-demo-y))
DEMO_EXEC=$(obj-demo-y)
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)/unixsocket

$(OBJS):$(OBJDIR)/%.o:%.c
	@echo "\t $(COMPILE) \t $^"
	$(Q) $(COMPILE) -o $@ -c $^ $(CFLAGS) $(LINKFLAGS)

$(BUILDIN): $(OBJS)
	$(Q) $(LD) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN)
	@echo "\t $(COMPILE) \t $@"
	$(Q) $(COMPILE) -o $@ $(patsubst %,%.c,$@) $(BUILDIN) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

