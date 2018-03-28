
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

DEMO_CFLAGS += -I.

.PHONY: demo

all: deps $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)

$(DEMO_EXEC):$(obj-demo-y)
	@echo "\t $(CC) \t $@-$(OS)"
	$(Q) $(CC) -o $@-$(OS) $(patsubst %,%.c,$@) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

distclean:
	rm -rf $(OBJDIR)

