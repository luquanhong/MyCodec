VCODEC_R_PATH := src/codecs/decoder/video/sw_ffmpeg

OEM_INC	+= $(LOCAL_PATH)/src/codecs/decoder/video/sw_ffmpeg/OEM/include 

ifeq ($(TARGET_ARCH),arm)
OEM_SRC += $(VCODEC_R_PATH)/um_video_dec.c	
endif

ifeq ($(TARGET_ARCH),mips)
OEM_SRC := $(VCODEC_R_PATH)/um_video_dec.c	
endif
