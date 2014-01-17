ACODEC_R_PATH := src/codec/decoder/audio/sw_ffmpeg

OEM_INC	+= $(LOCAL_PATH)/src/codec/decoder/video/sw_ffmpeg/OEM/include 
		
OEM_SRC += $(ACODEC_R_PATH)/um_audio_dec.c	

CFLAGS	+= 

LFLAGS	+=

#OEM_LDLIBS	+= -L$(LOCAL_PATH)/src/codec/decoder/video/sw_ffmpeg/OEM/libs -lavcodec_ub -lavformat_ub -lavutil_ub -lswscale_ub

