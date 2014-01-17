
LOCAL_PATH := $(call my-dir)
PROJECT_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)

#device name,eg: hw_stagefright_ics
v := 
a := 

ifeq ($(strip $(v)),)
$(warning "v is not be null, you should use prama eg:"v=hw_stagefright_gingerbread"") 
endif

ifeq ($(strip $(a)),)
$(warning "a is not be null, you should use prama eg:"a=sw_ffmpeg"") 
endif

V_CODEC_PATH := $(LOCAL_PATH)/src/codec/decoder/video/$(v)
A_CODEC_PATH := $(LOCAL_PATH)/src/codec/decoder/audio/$(a)

V_RENDER_PATH := $(LOCAL_PATH)/src/render/opengles2
A_RENDER_PATH := 

COM_INC := \
	$(LOCAL_PATH)/src/utils \
	$(LOCAL_PATH)/include		

COM_SRC := \
	src/utils/um_sys_event.c \
	src/utils/um_sys_mutex.c \
	src/utils/um_sys_thread.c \
	src/utils/um_circle_list.c \
	src/utils/um_circle_queue.c \
	src/utils/um_mtm.c
	
#import the kinds of device's config.mk
include $(V_CODEC_PATH)/config.mk
include $(A_CODEC_PATH)/config.mk

#if the video use software decode,it need opengl to render.
ifeq ($(v),sw_ffmpeg)
$(warning "sw_ffmpeg will use the opengl render.") 
include $(V_RENDER_PATH)/config.mk
endif

ifeq ($(v),hw_stagefright)
$(warning "hw_stagefright will use the opengl render.") 
include $(V_RENDER_PATH)/config.mk
endif

ifeq ($(v),hw_stagefright_jellybean43)
$(warning "hw_stagefright will use the opengl render.") 
include $(V_RENDER_PATH)/config.mk
endif

#if audio or video use sw_ffmpeg, it must add um_load_ffmpeg.c
ifeq ($(findstring sw_ffmpeg, $(a) $(v)),sw_ffmpeg)
COM_SRC += src/utils/um_load_ffmpeg.c 
endif
	
LOCAL_MODULE := libubmedia_$(v)


LOCAL_CXXFLAGS := -DHAVE_PTHREADS -w -DENABLE_LOG

LOCAL_C_INCLUDES := \
		$(COM_INC)	\
		$(OEM_INC)

LOCAL_SRC_FILES := 	\
		$(COM_SRC)	\
		$(OEM_SRC)
		
$(warning $(LOCAL_C_INCLUDES))
	
LOCAL_LDLIBS := -llog 
LOCAL_LDLIBS += $(OEM_LDLIBS)

include $(BUILD_SHARED_LIBRARY)
