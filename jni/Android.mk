LOCAL_PATH := $(call my-dir)

LOCAL_INCLUDE := $(LOCAL_PATH)\..\..\include

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LOCAL_INCLUDE)\ffmpeg
LOCAL_C_INCLUDES += $(LOCAL_INCLUDE)\libsav

LOCAL_CPPFLAGS += -D__STDC_CONSTANT_MACROS -fpermissive

LOCAL_LDLIBS += -L$(LOCAL_PATH)\..\..\so -llog	\
				-lc -lm -lpjlib-util -lavformat  -lavdevice -lavcodec  -lavutil -lswscale -lm -lz

LOCAL_MODULE    := sav
LOCAL_SRC_FILES := SAVCodec.c	\
					SAVUtil.c	\
					SAVContainer.c

include $(BUILD_SHARED_LIBRARY)
