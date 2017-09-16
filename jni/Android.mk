LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)  
LOCAL_MODULE := libmtee      
LOCAL_SRC_FILES := libmtee.so
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_SHARED_LIBRARY)      
  
include $(CLEAR_VARS)  
LOCAL_MODULE    := teeclientdemo
LOCAL_SRC_FILES := TEEClientDemo.c
  
LOCAL_SHARED_LIBRARIES  := libmtee   
LOCAL_LDLIBS += -ldl -llog

LOCAL_CFLAGS += -DANDROID_LOG_PRINT

include $(BUILD_SHARED_LIBRARY)  
