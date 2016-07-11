
USECC=gcc

obj-dep-$(HTTP) = ../common/$(OBJDIR)/build-in.o ../kfmd5/$(OBJDIR)/build-in.o 

obj-$(HTTP) = http/ghttp.o http/http_date.o http/http_req.o http/http_trans.o http/http_base64.o 
obj-$(HTTP) += http/http_hdrs.o http/http_resp.o http/http_uri.o

obj-$(RASP) += rasp/kfrasp.o rasp/kfrasp_parse_data.o rasp/kfrasp_string.o

#obj-demo-$(HTTP) = demo/demo_http.c
#obj-demo-$(RASP) += demo/demo_rasp.c demo/demo_tcp.c

