CODEC_H_PATH	= $(PROJECT_PATH)/src/codecs/decoder/audio/sw_opencore
CODEC_A_PATH	= src/codecs/decoder/audio/sw_opencore

OEM_INC	+= $(CODEC_H_PATH)/OEM/include

OEM_SRC += 	$(CODEC_A_PATH)/um_audio_dec.cpp
			

CFLAGS		+=

LFLAGS		+=

#LOCAL_STATIC_LIBRARIES	+= -L$(CODEC_H_PATH)/OEM/lib/libpv_aac_dec
ifeq ($(TARGET_ARCH),arm)
OEM_LDLIBS += -L$(CODEC_H_PATH)/OEM/lib/libs/armeabi -lpv_aac_dec
endif

ifeq ($(TARGET_ARCH),mips)
OEM_LDLIBS += -L$(CODEC_H_PATH)/OEM/lib/libs/mips -lpv_aac_dec
endif
SHARED_LIBS	+=
