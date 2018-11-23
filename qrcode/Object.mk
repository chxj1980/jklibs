
COMPILE=$(CC)
obj-dep-y += ../common/$(OBJDIR)/build-in.o

CFLAGS+= -I. -I../common -DHAVE_CONFIG_H=1

LINKPATH += $(STATICLIBS)

obj-qrcode-$(QRCODE) += bitstream.o  mask.o  mmask.o  mqrspec.o qrencode.o  qrinput.o  qrspec.o  rsecc.o  split.o

obj-qrcode-demo-$(QRCODE) += demo_qrcode qrenc

