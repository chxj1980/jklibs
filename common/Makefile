
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJS= $(obj-cm-y:%=$(OBJDIR)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.c,%,$(obj-demo-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

CFLAGS += $(CFLAGS-cm-y)
LDFLAGS += $(LDFLAGS-cm-y)

DEMO_CFLAGS += -I.

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)/json
	@mkdir -p $(OBJDIR)/crypto
	@mkdir -p $(OBJDIR)/openssl-aes/src/
	@mkdir -p $(OBJDIR)/libconfig

$(OBJS):$(OBJDIR)/%.o:%.c
	@echo "\t $(CC) \t $^"
	$(Q) $(CC) -o $@ -c $^ $(CFLAGS) $(LINKFLAGS)

$(BUILDIN): $(OBJS)
	$(Q) $(LD) $(LDFLAGS) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(CC) \t $@"
	$(Q) $(CC) -o $@ $(patsubst %,%.c,$@) $(BUILDIN) $(OBJ_DEPS) $(CFLAGS) $(DEMO_CFLAGS) $(LDFLAGS) $(LINKFLAGS) $(LINKPATH)

clean:
	rm -rf $(OBJS) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

