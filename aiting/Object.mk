
USECC=g++

obj-dep-y += ../common/$(OBJDIR)/build-in.o
obj-dep-y += ../kfmd5/$(OBJDIR)/build-in.o

AITING_BASE=../$(THIRD_BASE)/aiting/$(OS)

CFLAGS += -I../kfmd5 -I$(AITING_BASE)/include/ultimate
LINKFLAGS += -L$(AITING_BASE)/lib -lultimate

CFLAGS += -I/opt/data/sctek/mozart/mozart/output/molib/app/usr/include
CFLAGS += -I/opt/data/sctek/mozart/mozart/output/molib/updater/usr/include

LINKFLAGS += -L/opt/data/sctek/mozart/mozart/output/molib/app/usr/lib
LINKFLAGS += -L/opt/data/sctek/mozart/mozart/output/molib/updater/usr/lib -lcurl -lssl -lcrypto -lz -lstdc++

obj-$(AITING)= base.o

obj-demo-$(AITING) = demo-aiting.cc

DESTDIR=/opt/data/output
