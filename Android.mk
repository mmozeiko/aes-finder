LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  aes-finder.cpp

LOCAL_MODULE := aes-finder
LOCAL_LDFLAGS   += -llog -fPIE -pie
LOCAL_CFLAGS    += -fPIE

TARGET_ABI := android-23-all

include $(BUILD_EXECUTABLE)
