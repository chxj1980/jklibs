############
### this file place the variable of will be change many times
### author: yuwei.zhang@besovideo.com
##########

#BASEDIR=`pwd`

## The first args is default here.
## If different platform has diff value,
## please comment it, and define it in the config_xxx.mk files.

#base function
# must be selected.
BVBASE=y

## outside zlog
BVZLOG=no

ifeq ("$(DISK)", "")
DISK=y
endif

ifeq ("$(CODEC)", "")
X264=y
CODEC=y
endif

ifeq (x"$(VDEV)", x"")
VDEV=n
endif

ifeq ("$(BVSTREAM)", "")
BVSTREAM=y
endif

ECHO=echo -e

VERSION=0.0.1
LIBSNAME=jk-v1

## debug use
#BVDEBUG=yes

sinclude $(BASEDIR)/config.mk

## If the command from command line, use it instead others
ifeq ("$(origin OS)", "command line")
	OS := $(OS)
endif

include $(BASEDIR)/config/config_$(OS).mk


######################################
### Constant args
CFLAGS += -O2 -fPIC -Wall
CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-switch

## the path when you use out lib
## base position of libs
LIBFLAGS+=-L$(HOME)/libs/opensource/$(OS)/lib
LIBFLAGS+=-L$(HOME)/libs/bvlib/$(OS)/lib

## module function
ifneq ($(OS), x86)
PJFLAGS += -DPJ_IS_LITTLE_ENDIAN=0 -DPJ_IS_BIG_ENDIAN=1
endif

ifeq ($(GCC_HIGH), yes) 
endif

ifeq ($(BVZLOG), no)
CFLAGS += -DUNUSE_ZLOG
endif

LINKPATH+=-lpthread

ifeq ($(JK_HISI), yes)
LINKPATH += -lmpi -lhiaudio
endif

ifeq ($(BVBASE), y)
CFLAGS+=-D__BVBASE
CFLAGS += -Icommon
endif

ifeq ($(DISK), y)
endif

ifeq ($(VDEV), y)
endif

ifeq ($(BVSTREAM), y)
CFLAGS += -DBVSTREAM
CFLAGS += -Istream
endif

ifeq ($(X264), y)
LINKPATH += -lx264
endif

Q=@

ifeq (x$(V), xs)
	Q=
endif

## We allowed to compile.
## it will convert to filedirs-y instream the enviroument value.
## It will be converted to filedirs-n and other not -y if we don't want to compile it.
filedirs-$(BVBASE) = common
filedirs-$(VDEV) += vdev
filedirs-$(CODEC) += codec
#filedirs-$(DISK) += disk
#filedirs-$(BVSTREAM) += stream

## all linked in build-in.o files in each directory
buildin-files = $(patsubst %,%/$(OBJDIR)/build-in.o,$(filedirs-y))

OBJDIR=.obj$(OS)
#OBJS=$(OBJS_y:%=$(OBJDIR)/%)

#####################################
### set args for diff platform
EXT=$(OS)

### Compile need
CC=$(CROSS_COMPILE)gcc
ifeq ($(BVPROTOBUF), yes)
CC=$(CROSS_COMPILE)g++
endif
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
STRIP=$(CROSS_COMPILE)strip

DYNJKLIB=lib$(LIBSNAME)-$(EXT).so.$(VERSION)
LINKJK=lib$(LIBSNAME).so
STATIC_JKLIB=lib$(LIBSNAME)-$(EXT).a
LINKSTATICJK=lib$(LIBSNAME).a

INSTALL_HEADERS=
INSTALL_LIBS=
INSTALL_DIRS=$(BASEDIR)/outlib/$(OS)/

LIBDIR_PATH=$(INSTALL_DIRS)/lib

## this include and lib for test and example
# libjk generate distination is here.
#### The demo use this libs will need it.
EXTERN_INCLUDES=-I./outlib/$(OS)/include
EXTERN_LIBPATH=-L./outlib/$(OS)/lib
EXTERN_LIBS=-l$(LIBSNAME)

export CC LD AR STRIP CFLAGS

