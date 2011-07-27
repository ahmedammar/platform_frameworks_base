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

ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl -lpthread
endif

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
	libstagefright_color_conversion         \
	libstagefright_foundation               \
	libsurfaceflinger_client		\
	lib_omx_player_arm11_elinux		\
	lib_omx_player_arm11_elinux \
	lib_omx_osal_v2_arm11_elinux \
	lib_omx_client_arm11_elinux \
	lib_omx_utils_v2_arm11_elinux \
	lib_omx_core_mgr_v2_arm11_elinux \
	lib_omx_res_mgr_v2_arm11_elinux \
	lib_id3_parser_arm11_elinux \
	libgstplayer

LOCAL_STATIC_LIBRARIES := \
        libstagefright_rtsp

ifneq ($(BUILD_WITHOUT_PV),true)
LOCAL_SHARED_LIBRARIES += \
	libopencore_player    \
	libopencore_author
else
LOCAL_CFLAGS += -DNO_OPENCORE
endif

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

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
	$(TOP)/external/tremolo/Tremolo 				\
	$(TOP)/external/gstreamer_aggregate/gst-android/gstplayer

LOCAL_MODULE:= libmediaplayerservice

include $(BUILD_SHARED_LIBRARY)

