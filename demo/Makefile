

LIBSNAME=ademo
TMP_OBJ=.obj

SOURCE_FILE=$(wildcard *.c)
OBJSOURCE=$(patsubst %.c,$(TMP_OBJ)/%.o,$(SOURCE_FILE))

pre:
	@mkdir -p $(TMP_OBJ)

$(OBJSOURCE):$(TMP_OBJ)/%.o:%.c
	$(CXX) -fPIC -c -o $@ $<

all: pre $(OBJSOURCE)
	$(CXX) -shared -fPIC -o $(LIBSNAME).so $(OBJSOURCE)

.PHONY: clean

clean:
	rm -rf .obj *.so

