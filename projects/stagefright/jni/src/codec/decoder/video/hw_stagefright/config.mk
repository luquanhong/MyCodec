CODEC_PATH	= $(PROJECT_PATH)/src/codec/decoder/video/hw_stagefright
CODEC_R_PATH := src/codec/decoder/video/hw_stagefright


OEM_INC	+=   $(CODEC_PATH)/OEM/include/include \
        $(CODEC_PATH)/OEM/include/frameworks/base/include \
		$(CODEC_PATH)/OEM/include/frameworks/base/core/jni \
		$(CODEC_PATH)/OEM/include/frameworks/native/include \
		$(CODEC_PATH)/OEM/include/frameworks/native/include/media/openmax \
		$(CODEC_PATH)/OEM/include/frameworks/native/include/gui \
		$(CODEC_PATH)/OEM/include/frameworks/native/include/media/hardware \
		$(CODEC_PATH)/OEM/include/frameworks/native/opengl/include	\
		$(CODEC_PATH)/OEM/include/frameworks/av/include \
		$(CODEC_PATH)/OEM/include/frameworks/av/media/libstagefright \
		$(CODEC_PATH)/OEM/include/frameworks/av/media/libstagefright/include \
		$(CODEC_PATH)/OEM/include/hardware/libhardware/include \
		$(CODEC_PATH)/OEM/include/system/core/include \
		$(CODEC_PATH)/OEM/include/libnativehelper/include \
		$(CODEC_PATH)/um_stagefright/inc 
       
	
		
OEM_SRC += $(CODEC_R_PATH)/um_video_dec.cpp


LFLAGS		+=

OEM_LDLIBS	+= -L$(CODEC_PATH)/OEM/libs -lavutil_ub -lOpenMAXAL -lOpenSLES -lvorbisidec -lz -lutils -lsysutils -lstagefright_foundation -landroid -lcutils -licuuc -lui -ldl -lbinder -landroid_runtime -lstagefright -lstagefright_omx -lmedia -lstagefright_avc_common -lstagefright_amrnb_common -lsurfaceflinger -lgui
