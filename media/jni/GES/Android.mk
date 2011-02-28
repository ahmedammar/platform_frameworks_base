LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    android_media_ges_timeline.cpp \
    android_media_ges_track.cpp \
    android_media_ges_proxy.cpp \
    android_media_ges_ops.cpp \
    android_media_ges.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libandroid_runtime \
	libnativehelper \
	libmedia

CONFIGURE_PKG_CONFIG_LIBDIR := $(TOP)/external/gstreamer_aggregate/glib:$(TOP)/external/gstreamer_aggregate/gstreamer/pkgconfig:$(TOP)/external/gstreamer_aggregate/gst-plugins-base/pkgconfig:$(TOP)/external/gstreamer_aggregate/gst-editing-services/pkgconfig
PKG_CONFIG := PKG_CONFIG_LIBDIR=$(CONFIGURE_PKG_CONFIG_LIBDIR) PKG_CONFIG_TOP_BUILD_DIR=$(TOP) pkg-config

PKG_CONFIG_CFLAGS = $(shell $(PKG_CONFIG) gst-editing-services --cflags)

LOCAL_CFLAGS += $(PKG_CONFIG_CFLAGS)
LOCAL_SHARED_LIBRARIES +=   \
    libges-0.10             \
    libgstpbutils-0.10      \
    libgstbase-0.10         \
    libgstreamer-0.10       \
    libglib-2.0             \
    libgthread-2.0          \
    libgmodule-2.0          \
    libgobject-2.0

LOCAL_LDLIBS := -lpthread

LOCAL_MODULE:= libges_jni
LOCAL_MODULE_TAGS := eng
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
