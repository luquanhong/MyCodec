ACODEC_R_PATH := src/codecs/decoder/audio/hw_netcom_dongle

OEM_INC	+= $(LOCAL_PATH)/src/codecs/decoder/video/hw_netcom_dongle/OEM/include 
		
OEM_SRC += $(ACODEC_R_PATH)/um_audio_dec.cpp

CFLAGS	+= -DDECODER_WITH_RENDERER

LFLAGS	+= 

OEM_LDLIBS += -L$(CODEC_PATH)/OEM/libs -ltsplayer

