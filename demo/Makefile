

#OBJSOURCE=$(patsubst %.c,$(TMP_OBJ)/%.o,$(SOURCE_FILE))

JKRECORD_FILE=jkrecord.c

CFLAGS+=-I../vdev -I../codec -I../common
LDFLAGS+=-L/opt/data/libs/$(OS)/lib -lx264

OBJDIR=.obj$(OS)

BUILDIN_FILES=../common/$(OBJDIR)/build-in.o
BUILDIN_FILES+=../codec/$(OBJDIR)/build-in.o
BUILDIN_FILES+=../vdev/$(OBJDIR)/build-in.o

pre:
	@mkdir -p .obj

jkrecord:
	$(CC) -o jkrecord-$(OS) $(JKRECORD_FILE) $(BUILDIN_FILES) $(CFLAGS) $(LDFLAGS)

all:

.PHONY: clean

clean:
	rm -rf jkrecord-$(OS)

