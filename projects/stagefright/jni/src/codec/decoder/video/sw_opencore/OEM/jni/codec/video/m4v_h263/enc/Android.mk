LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/bitstream_io.cpp \
	src/combined_encode.cpp \
	src/datapart_encode.cpp \
	src/dct.cpp \
	src/findhalfpel.cpp \
	src/fastcodemb.cpp \
	src/fastidct.cpp \
	src/fastquant.cpp \
	src/pvm4vencoder.cpp \
	src/me_utils.cpp \
	src/mp4enc_api.cpp \
	src/rate_control.cpp \
	src/motion_est.cpp \
	src/motion_comp.cpp \
	src/sad.cpp \
	src/sad_halfpel.cpp \
	src/vlc_encode.cpp \
	src/vop.cpp \
	../port/encoderwrap.cpp



LOCAL_MODULE := libpvm4vencoder

LOCAL_CFLAGS := -DBX_RC -D__arm__ -DYUV_INPUT -DRGB12_INPUT -DRGB24_INPUT  $(PV_CFLAGS)
ifeq ($(TARGET_ARCH),arm)
 LOCAL_CFLAGS += -DPV_ARM_GCC 
 endif

LOCAL_ARM_MODE := arm 

LOCAL_C_INCLUDES := \
	include/ \
	src/ 	\
	../port/ 
LOCAL_CFLAGS += \
							-I$(LOCAL_PATH)/include/ \
							-I$(LOCAL_PATH)/../port \
							-I$(LOCAL_PATH)/src/ \
							-I$(LOCAL_PATH)/../../../utilities/colorconvert/include/ 

LOCAL_COPY_HEADERS_TO := $(PV_COPY_HEADERS_TO)
LOCAL_COPY_HEADERS := \
	../port/uc_port.h \
	include/cvei.h \
	include/pvm4vencoder.h \
	include/mp4enc_api.h

include $(BUILD_STATIC_LIBRARY)

