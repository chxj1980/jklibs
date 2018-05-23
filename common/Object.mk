
KFOPENSSL_AES=n

CFLAGS+= -Iopenssl-aes/include -I.

SRC_DIR=openssl-aes/src

obj-y += cm_print.o cm_conn_tcp.o rt_server_tcp.o  cm_utils.o jkbytes.o
obj-y += unixsocket/rt_unixsocket.o
obj-$(LIBJSON) += json/json.o
obj-$(KFCRYPTO) += crypto/base64.o
#crypto/aes.o 
obj-$(KFOPENSSL_AES) += $(SRC_DIR)/aes_misc.o $(SRC_DIR)/aes_ecb.o $(SRC_DIR)/aes_cfb.o $(SRC_DIR)/aes_ofb.o $(SRC_DIR)/aes_ctr.o $(SRC_DIR)/aes_core.o $(SRC_DIR)/aes_cbc.o $(SRC_DIR)/cbc128.o $(SRC_DIR)/ctr128.o $(SRC_DIR)/cts128.o $(SRC_DIR)/cfb128.o $(SRC_DIR)/ofb128.o $(SRC_DIR)/ccm128.o $(SRC_DIR)/xts128.o
# $(SRC_DIR)/aes_ige.o $(SRC_DIR)/gcm128.o $(SRC_DIR)/aes_wrap.o

#obj-demo-y = demo/isProgramRunning.c demo/demo_conn.c demo/demo_utils.c demo/demo_tcp.c
#obj-demo-$(LIBJSON) += demo/demo_json.c
#obj-demo-$(KFCRYPTO) += demo/demo_crypto.c
#obj-demo-y = demo/jk_demo.c
#obj-demo-y = demo/kfclientping.c

