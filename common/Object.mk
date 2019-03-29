
CFLAGS-cm-y += -I. -Ilibconfig 
CXXFLAGS += -Icommon -std=c++11

obj-cm-y += cm_print.o cm_utils.o jkbytes.o \
	cm_list.o \
	cm_sys.o
obj-cm-y += cm_unixsocket.o
obj-cm-$(LIBCONFIG) += libconfig/grammar.o  \
	libconfig/libconfig.o  \
	libconfig/scanctx.o  \
	libconfig/scanner.o  \
	libconfig/strbuf.o
obj-cm-$(CMCONF) += cm_conf.o
obj-cm-$(BASE64) += cm_base64.o
obj-cm-$(LIBJSON) += json/json.o
obj-cm-$(KFCRYPTO) += \
	crypto/aeslib.o \
	crypto/polaraes.o
obj-cm-$(CMNET) += cm_http.o \
	cm_conn_tcp.o \
	cm_conn_ws.o \
	cm_http_download.o \
	cm_http_upload.o \
	http_parser.o \
	cm_conn_udp.o
obj-cm-$(CMFLASH) += cm_flash_cfg.o
obj-cm-$(CMUART) += cm_uart.o
obj-cm-$(NGXMD5) += ngx_md5.o

predirs-$(LLHTTP) += \
	common/llhttp/src/native \
	common/llhttp/build/c

CFLAGS-cm-$(LLHTTP) += \
	-Icommon/llhttp/src/native \
	-Icommon/llhttp/build

obj-cm-$(LLHTTP) +=\
	llhttp/src/native/api.o \
	llhttp/src/native/http.o \
	llhttp/build/c/llhttp.o

predirs-$(CMMISC) +=\
	misc
CFLAGS-cm-$(CMMISC) += \
	-Icommon/misc

predirs-$(JANSSON) += \
	jansson
CFLAGS-cm-$(JANSSON) += \
	-Icommon/jansson \
	-DHAVE_CONFIG_H
obj-cm-$(JANSSON) += \
	jansson/dump.o \
   	jansson/error.o \
	jansson/hashtable.o \
	jansson/hashtable_seed.o \
   	jansson/load.o  \
	jansson/memory.o  \
	jansson/pack_unpack.o \
   	jansson/strbuffer.o \
   	jansson/strconv.o  \
	jansson/utf.o \
   	jansson/value.o

obj-cm-cpp-$(CMEX) += \
	ex/cm_conf_ex.o \
	ex/cm_uart_ex.o \
	ex/cm_utils_ex.o

obj-cm-cpp-$(CMEXJSON) += \
	ex/json/json/json_reader.o \
	ex/json/json/json_writer.o \
	ex/json/json/json_value.o \
	ex/cm_msg.o

obj-cm-$(PROGDEBUG) += \
	misc/cm_local_debug.o \
	misc/cm_broadcast.o \
	misc/cm_debug_out.o

