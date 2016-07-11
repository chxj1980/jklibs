
USECC=gcc

#KF_MBEDTLS=y
KF_MD5=y

ifeq ("$(KF_MBEDTLS)", "y")
CFLAGS+=-DKF_MBEDTLS
endif

obj-$(KF_MD5) = ngx_md5.o
obj-$(KF_MBEDTLS) = md5.o kfmd5_mbedtls.o 

obj-demo-y = kfmd5sum.c demo_md5.c

