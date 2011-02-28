LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=               \
    MediaRecorderClient.cpp     \
    MediaPlayerService.cpp      \
    MetadataRetrieverClient.cpp \
    TestPlayerStub.cpp          \
    MidiMetadataRetriever.cpp   \
    MidiFile.cpp                \
    StagefrightPlayer.cpp       \
    StagefrightRecorder.cpp

LOCAL_SHARED_LIBRARIES :=     		\
	libcutils             			\
	libutils              			\
	libbinder             			\
	libvorbisidec         			\
	libsonivox            			\
	libmedia              			\
	libcamera_client      			\
	libandroid_runtime    			\
	libstagefright        			\
	libstagefright_omx    			\
	libstagefright_foundation       \
	libgui                          \
	libdl                                \
	libgstplayer

LOCAL_STATIC_LIBRARIES := \
        libstagefright_nuplayer                 \
        libstagefright_rtsp                     \

CONFIGURE_PKG_CONFIG_LIBDIR := $(TOP)/external/gstreamer_aggregate/glib:$(TOP)/external/gstreamer_aggregate/gstreamer/pkgconfig:$(TOP)/external/gstreamer_aggregate/gst-plugins-base/pkgconfig
PKG_CONFIG := PKG_CONFIG_LIBDIR=$(CONFIGURE_PKG_CONFIG_LIBDIR) PKG_CONFIG_TOP_BUILD_DIR=$(TOP) pkg-config
LOCAL_CFLAGS += \
	$(shell $(PKG_CONFIG) gstreamer-0.10 --cflags)		\
	$(shell $(PKG_CONFIG) gstreamer-app-0.10 --cflags)

LOCAL_C_INCLUDES :=                                                 \
	$(JNI_H_INCLUDE)                                                \
	$(call include-path-for, graphics corecg)                       \
	$(TOP)/frameworks/base/include/media/stagefright/openmax \
	$(TOP)/frameworks/base/media/libstagefright/include             \
	$(TOP)/frameworks/base/media/libstagefright/rtsp                \
        $(TOP)/external/tremolo/Tremolo \
	$(TOP)/external/gstreamer_aggregate/gst-android/gstplayer

LOCAL_MODULE:= libmediaplayerservice

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

