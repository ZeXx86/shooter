LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
		    $(LOCAL_PATH)/../SDL_image

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.cpp \
	main.cpp bot.cpp gl.cpp level.cpp player.cpp event.cpp kbd.cpp logic.cpp mouse.cpp tex.cpp mdl/mdl.cpp

LOCAL_SHARED_LIBRARIES := SDL2_image SDL2

LOCAL_LDLIBS := -llog -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)
