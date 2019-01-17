############
### this file place the variable of will be change many times
### author: jmdvirus@aliyun.com
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

CONFIG_FILE=common/config.h

ifeq ("$(DISK)", "")
DISK=n
endif

ifeq ("$(CODEC)", "y")
X264=y
X265=y
CFLAGS+=-Icodec
endif

ifeq (x"$(VDEV)", x"")
VDEV=n
endif

ifeq ("$(BVSTREAM)", "")
BVSTREAM=n
endif

ifeq ("$(RECORDSERVER)", "")
RECORDSTREAM=n
endif

ifeq ("$(USE_LIBCURL)", "y")
	CFLAGS += -DUSE_LIBCURL
endif

ECHO=echo -e

VERSION=0.0.1
LIBSNAME=jk-v1

## debug use
#BVDEBUG=yes

export THIRD_BASEPATH=/opt/data/libs/

export THIRD_BASE=third

sinclude $(BASEDIR)/config.mk

## If the command from command line, use it instead others
ifeq ("$(origin OS)", "command line")
	OS := $(OS)
endif

sinclude $(BASEDIR)/config/config_$(OS).mk

ifeq (x$(DEBUG_M), xy)
DEBUG_OP= -g
else
DEBUG_OP= -O2
endif

######################################
### Constant args
CFLAGS += $(DEBUG_OP) -fPIC -Wall
CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-switch

CXXFLAGS += $(DEBUG_OP) -fPIC -Wall

## module function
ifeq ($(GCC_HIGH), yes) 
endif

ifeq ($(BVZLOG), no)
CFLAGS += -DUNUSE_ZLOG
endif

LDFLAGS+=-lpthread

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
CFLAGS += -Ivdev
endif

ifeq ($(BVSTREAM), y)
CFLAGS += -DBVSTREAM
CFLAGS += -Istream
endif

ifeq ($(X264), y)
CFLAGS += -I/opt/data/libs/$(OS)/include
LINKPATH += -L/opt/data/libs/$(OS)/lib -lx264
endif

ifeq ($(X265), y)
CFLAGS += -I/opt/data/libs/$(OS)/include
LINKPATH += -L/opt/data/libs/$(OS)/lib -lx265
endif

Q=

ifeq (x$(V), xn)
	Q=@
endif

## We allowed to compile.
## it will convert to filedirs-y instream the enviroument value.
## It will be converted to filedirs-n and other not -y if we don't want to compile it.
filedirs-$(BVBASE) = common
filedirs-$(VDEV) += vdev
filedirs-$(KFMD5) += kfmd5
filedirs-$(CODEC) += codec
filedirs-$(DISK) += disk
filedirs-$(BVSTREAM) += stream
filedirs-$(RECORDSERVER) += recordserver
filedirs-$(OPENAV) += openav
filedirs-$(QRCODE) += qrcode
filedirs-$(AITING) += aiting

## all linked in build-in.o files in each directory
buildin-files = $(patsubst %,%/$(OBJDIR)/build-in.o,$(filedirs-y))
filedirs-d-$(DEMO) += demo

OBJDIR=.obj$(OS)
#OBJS=$(OBJS_y:%=$(OBJDIR)/%)

export DEP_COMMON=common/$(OBJDIR)/build-in.o
export DEP_COMMONEX=commonex/$(OBJDIR)/build-in.o
export DEP_KFMD5=kfmd5/$(OBJDIR)/build-in.o

#####################################
### set args for diff platform
EXT=$(OS)

### Compile need
CC=$(CROSS_COMPILE)gcc
ifeq ($(BVPROTOBUF), yes)
CC=$(CROSS_COMPILE)g++
endif
CXX=$(CROSS_COMPILE)g++
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

