LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pngpacket_static
LOCAL_MODULE_FILENAME := libPngPacket
LOCAL_SRC_FILES := libPngPacket.a  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../
include $(PREBUILT_STATIC_LIBRARY)