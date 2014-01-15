ifeq ($(v), sw_ffmpeg)
APP_ABI := armeabi mips
endif
ifeq ($(v), hw_nexus_bcm7249)
APP_ABI := mips
endif

