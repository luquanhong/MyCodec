CODEC_PATH	= $(ROOT_PATH)codecs/decoder/video/sw_opencore/OEM/jni/codec/
MEDIA_PATH	= $(ROOT_PATH)codecs/decoder/video/sw_opencore/OEM/jni/media/

INC_PATH 	+= -I$(CODEC_PATH)video/m4v_h263/port \
			   -I$(CODEC_PATH)video/m4v_h263/dec/include \
			   -I$(CODEC_PATH)video/m4v_h263/dec/src \
			   -I$(CODEC_PATH)video/avc_h264/dec/include \
			   -I$(CODEC_PATH)video/avc_h264/dec/src \
			   -I$(CODEC_PATH)video/avc_h264/common/include

SRC_FILES 	+= $(ROOT_PATH)codecs/decoder/video/sw_opencore/um_video_dec.c

CFLAGS		+=

LFLAGS		+=

STATIC_LIBS	+= -L$(ROOT_PATH)codecs/decoder/video/sw_opencore/OEM/obj/local/armeabi \
			   -lpvmp4decoder \
			   -lm4v_config \
			   -lcolorconvert \
			   -lh264decoder \
			   -lhwh264decoder \
			   -lavc_common_lib

SHARED_LIBS	+=
