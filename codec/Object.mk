
USECC=gcc

obj-dep-$(CODEC) =

obj-codec-$(CODEC) = codec.o encoder.o enc_h265.o

obj-codec-demo-$(CODEC) = demo.c

