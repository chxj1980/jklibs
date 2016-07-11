
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
sinclude Config.mk
include Object.mk

OBJS= $(obj-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.c,%,$(obj-demo-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/http
	@mkdir -p $(OBJDIR)/rasp

$(OBJS):$(OBJDIR)/%.o:%.c
	@echo "\t $(CC) \t $^"
	$(Q) $(CC) -o $@ -c $^ $(CFLAGS) $(LINKFLAGS)

$(BUILDIN): $(OBJS)
	$(Q) $(LD) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(CC) \t $@-$(OS)"
	$(Q) $(CC) -o $@-$(OS) $(patsubst %,%.c,$@) $(BUILDIN) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

distclean:
	rm -rf $(OBJDIR)

