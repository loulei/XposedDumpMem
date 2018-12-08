LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := dumpmem
LOCAL_SRC_FILES := dump.c
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)