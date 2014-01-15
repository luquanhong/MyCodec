CODEC_H_PATH	= $(PROJECT_PATH)/src/codecs/decoder/audio/sw_audiotrack
CODEC_A_PATH	= src/codecs/decoder/audio/sw_audiotrack

OEM_INC	+= $(CODEC_H_PATH)/OEM/include

OEM_SRC += 	$(CODEC_A_PATH)/um_audio_dec.cpp
			

CFLAGS		+=

LFLAGS		+=

#LOCAL_STATIC_LIBRARIES	+= -L$(CODEC_H_PATH)/OEM/lib/libpv_aac_dec

OEM_LDLIBS += -L$(CODEC_H_PATH)/OEM/lib -lpv_aac_dec

SHARED_LIBS	+=
