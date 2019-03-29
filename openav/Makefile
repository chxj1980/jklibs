
shell=/bin/bash
CURDIR=$(shell pwd)
CONFIG_FILE=$(CURDIR)/../config/config_all.mk
BASEDIR=$(CURDIR)/../

include $(CONFIG_FILE)
include Object.mk

OBJDIRC=$(OBJDIR)/codec
OBJS= $(obj-openav-y:%=$(OBJDIR)/%)
OBJSC= $(obj-openavc-y:%=$(OBJDIRC)/%)
OBJ_DEPS=$(obj-dep-y)
## form *.c to *(exec demo name)
DEMO_EXEC=$(patsubst %.cpp,%,$(obj-openav-demo-y))
DEMO_EXEC_GEN=$(DEMO_EXEC:%=%-$(OS))

DEMO_CFLAGS += -I.

CXXFLAGS += $(CXXFLAGS-y)
LIBS += $(LIBS-y)

.PHONY: demo

BUILDIN=$(OBJDIR)/build-in.o

all: deps $(BUILDIN) $(DEMO_EXEC)

deps:
	@mkdir -p $(OBJDIR)/gl $(OBJDIR)/codec

$(OBJSC):$(OBJDIRC)/%.o:%.c
	@echo "\t $(COMPILE) \t $^"
	$(Q) $(CC) $(CFLAGS) -o $@ -c $^

$(OBJS):$(OBJDIR)/%.o:%.cpp
	@echo "\t $(COMPILE) \t $^"
	$(Q) $(COMPILE) -o $@ -c $^ $(CXXFLAGS)

$(BUILDIN): $(OBJS) $(OBJSC)
	$(Q) $(LD) $(LDFLAGS) -r -o $@ $^

$(DEMO_EXEC):$(BUILDIN) $(obj-demo-y)
	@echo "\t $(COMPILE) \t $@"
	$(Q) $(COMPILE) -o $@ $(patsubst %,%.cpp,$(@))  $(BUILDIN) $(OBJ_DEPS) $(CXXFLAGS) $(DEMO_CFLAGS) $(LINKFLAGS) $(LINKPATH) $(LIBS)

clean:
	rm -rf $(OBJS) $(OBJSC) $(BUILDIN)
	rm -rf $(DEMO_EXEC) $(DEMO_EXEC_GEN)

distclean:
	rm -rf $(OBJDIR)

