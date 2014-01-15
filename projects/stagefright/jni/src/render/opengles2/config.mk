OPENGL_PATH	= src/render/opengles2

OEM_SRC += 	$(OPENGL_PATH)/um_glrender_impl.cpp \
			$(OPENGL_PATH)/um_opengl_render.cpp	

OEM_LDLIBS	+= -L$(CODEC_PATH)/OEM/libs -lGLESv2 -lGLESv1_CM			