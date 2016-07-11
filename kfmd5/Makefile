
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJS= $(obj-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.c,%,$(obj-demo-y))

CFLAGS+=-I../common
DEMO_CFLAGS += -I.

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)

$(OBJS):$(OBJDIR)/%.o:%.c
	@echo "\t $(CC) \t $^"
	$(Q) $(CC) -o $@ -c $^ $(CFLAGS)

$(BUILDIN): $(OBJS)
	$(Q) $(LD) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(CC) \t $@"
	$(Q) $(CC) -o $@ $(patsubst %,%.c,$@) $(BUILDIN) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS)

clean:
	rm -rf $(OBJS) $(BUILDIN)
	rm -rf $(DEMO_EXEC)

