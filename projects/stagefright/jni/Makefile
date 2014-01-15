
NDK_PATH :=/home/leven/android-ndk-r7b
CROSS_COMILER_PATH :=/opt/arm-unknown-linux-gnueabi-4.5.3-glibc/arm-unknown-linux-gnueabi
SYSROOT:=/opt/arm-unknown-linux-gnueabi-4.5.3-glibc/target-arm-unknown-linux-gnueabi

CC :=$(CROSS_COMILER_PATH)/bin/gcc
CXX :=$(CROSS_COMILER_PATH)/bin/g++
AR :=$(CROSS_COMILER_PATH)/bin/ar
STRIP :=$(CROSS_COMILER_PATH)/bin/strip

PLATFORM=$(NDK_PATH)/platforms/android-14/arch-arm
LOCAL_PATH := $(CURDIR)

LOCAL_LDLIBS := -L$(CROSS_COMILER_PATH)/lib -lc -ldl -lpthread -lutil 
LOCAL_LDLIBS += -L$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/libs -lavapi -lbinder -lmedia 
LOCAL_LDLIBS += -L$(LOCAL_PATH)/src/codecs/decoder/video/sw_ffmpeg_asus_gtv3/OEM/libs/GTV_GLIBC -lavformat -lavutil -lavcodec -lswscale

######################################################################################
MACRO := -DHAVE_PTHREADS -w -DENABLE_LOG -DBUILD_GOOGLETV
######################################################################################
all : full

MODULE_NAME := libubmedia
######################################################################################
COM_INC := \
	-I$(SYSROOT)/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3/include \
	-I$(SYSROOT)/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3/include-fixed \
	-I$(SYSROOT)/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3/include/c++ \
	-I$(SYSROOT)/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3/include/c++/arm-unknown-linux-gnueabi \
	-I$(SYSROOT)/usr/include \
	-I$(SYSROOT)/usr/include/linux \
	-I$(LOCAL_PATH)/src/utils \
	-I$(LOCAL_PATH)/include	\
	-I$(PLATFORM)/usr/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/hardware/libhardware/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/system/core/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/skia/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/pvmi/pvmf/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/extern_libs_v2/khronos/openmax/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/include \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/android \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/gtvinclude \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/OEM/inc/utils  \
	-I$(LOCAL_PATH)/src/codecs/decoder/video/sw_ffmpeg_asus_gtv3/OEM/include
	
	
COM_SRC := \
	$(LOCAL_PATH)/src/codecs/decoder/video/hw_asus_gtv3_honeycomb/um_video_dec.cpp \
	$(LOCAL_PATH)/src/codecs/decoder/audio/sw_ffmpeg/um_audio_dec.c \
	$(LOCAL_PATH)/src/render/hw_asus_gtv3_honeycomb/um_audio_render.cpp \
	$(LOCAL_PATH)/src/utils/um_sys_event.c \
	$(LOCAL_PATH)/src/utils/um_sys_mutex.c \
	$(LOCAL_PATH)/src/utils/um_sys_thread.c \
	$(LOCAL_PATH)/src/utils/um_circle_list.c \
	$(LOCAL_PATH)/src/utils/um_mtm.c

LOCAL_C_INCLUDES := \
	$(COM_INC)	\
	$(OEM_INC)

LOCAL_SRC_FILES := 	\
	$(COM_SRC)	\
	$(OEM_SRC)
######################################################################################make
all:full

SRC_FILES =  $(LOCAL_SRC_FILES)
INC_PATH = $(LOCAL_C_INCLUDES)


S_SRC_FILES = $(filter %.S, $(SRC_FILES))
C_SRC_FILES = $(filter %.c, $(SRC_FILES))
CPP_SRC_FILES = $(filter %.cpp, $(SRC_FILES))

TARGET = $(MODULE_NAME).so
SRC_PATH = ./
OBJ_PATH = ./obj/
LIB_PATH = ./lib/
MKDIR	   	= mkdir -p



CFLAGS = $(LOCAL_LDLIBS) $(MACRO)
CXXFLAGS =$(LOCAL_LDLIBS) $(MACRO)

S_OBJ_FILES = $(patsubst %.S, $(OBJ_PATH)%.o, $(S_SRC_FILES))
C_OBJ_FILES = $(patsubst %.c, $(OBJ_PATH)%.o, $(C_SRC_FILES))
CPP_OBJ_FILES = $(patsubst %.cpp, $(OBJ_PATH)%.o, $(CPP_SRC_FILES))
OBJ_FILES = $(S_OBJ_FILES) $(C_OBJ_FILES) $(CPP_OBJ_FILES)
SHARED_LIBRARY = $(LIB_PATH)

EXFLAGS := -fexceptions -fno-rtti -Wno-psabi -fno-short-enums -funwind-tables -fno-short-enums 

####### Build rules
full: domkdir $(SHARED_LIBRARY)


$(SHARED_LIBRARY): $(OBJ_FILES) 
	@$(CC) $^ -o $(TARGET) $(EXFLAGS) $(CFLAGS) $(INC_PATH) -shared -nostdinc -fPIC -g
#	@echo _[compile gcc] $(TARGET)
$(OBJ_PATH)%.o: %.S
	@$(CC) $(CFLAGS) $(INC_PATH) -nostdinc -c $< -o $@ -fPIC -g
	@echo _[compile gcc] $<
$(OBJ_PATH)%.o: %.c
	@$(CC) $(CFLAGS) $(EXFLAGS) $(INC_PATH)  -nostdinc -c $< -o $@ -fPIC -g
	@echo _[compile gcc] $<
$(OBJ_PATH)%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(EXFLAGS) $(INC_PATH) -nostdinc  -c $< -o $@ -fPIC -g
	@echo _[compile g++] $<
domkdir:
	@$(MKDIR) $(dir $(OBJ_FILES))
	@$(MKDIR) $(LIB_PATH)

####### clean #######
clean:
	rm -rf $(OBJ_PATH)
	rm -rf $(SHARED_LIBRARY)
	rm -f $(TARGET)

###### info ######
info:
	@echo $(COM_INC)





