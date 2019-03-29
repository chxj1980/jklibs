
USECC=gcc

obj-dep-$(CODEC) =

obj-codec-$(CODEC) = codec.o encoder.o enc_h265.o\
	cmyuv.o

obj-codec-demo-$(CODEC) = cmcodec.c \
	cmyuvtools.c

