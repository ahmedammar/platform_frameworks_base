/*
** Copyright 2011 Collabora Ltd
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**/

#define LOG_TAG "GESTrack-JNI"
#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_media_ges_timeline.h"
#include "android_media_ges_track.h"

#include <ges/ges.h>

using namespace android;

DECLARE_GES_PROXY(GESTrack);

GESTrackProxy::GESTrackProxy(GESTrackType type, GstCaps *caps):
  GESProxy<GESTrack>(ges_track_new(type, caps))
{
}

GESTrackProxy::~GESTrackProxy()
{
  g_object_unref(proxied);
}

jobject GESTrackProxy::newJavaObject(JNIEnv *env, const char *type, void *ptr)
{
#if 1
  LOGE("Unimplemented");
  jniThrowException(env, "java/lang/RuntimeException", "Unimplemented");
  return NULL;
#else
  // Find and ref the class
  LOGI("Searching for %s class", type);
  jclass c = env->FindClass(type);
  if (c == NULL) {
    LOGE("Failed to find class %s", type);
    return NULL;
  }

  // Find ctor
  LOGI("Searching for %s ctor", type);
  jmethodID ctorid = env->GetMethodID(c, "<init>", "(Landroid/media/ges/GESTimeline;)V");
  if (ctorid == 0) {
    LOGE("Failed to find %s Java ctor", type);
    return NULL;
  }

  // Create the new object */
  LOGI("Allocating a new %s object for proxied %p", type, ptr);
  jobject o = env->NewObject(c, ctorid, (int)ptr);
  if (env->ExceptionCheck() != 0) {
    LOGE("Uncaught exception returned from %s Java ctor", type);
    env->ExceptionDescribe();
    return NULL;
  }
  if (o == NULL) {
    LOGE("Failed to create object of class %s", type);
    return NULL;
  }

  LOGI("Returning new %s object", type);
  return o;
#endif
}

void GESTrackProxy::native_setup(JNIEnv *env, jobject thiz, jobject weak_this, jint type, jstring capsObject)
{
    LOGV("native_setup");

    const char *caps = env->GetStringUTFChars(capsObject, NULL);
    if (caps == NULL ) {
      jniThrowException(env, "java/lang/RuntimeException", "caps are null");
      return;
    }
    GESTrackProxy *p = new GESTrackProxy((GESTrackType)type,
                                         gst_caps_from_string(caps));
    if (p == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
      return;
    }

    env->ReleaseStringUTFChars(capsObject, caps);

    // Stow our new C++ object in an opaque field in the Java object.
    LOGV("native_setup storing");
    setGESProxy(env, thiz, p);
    LOGV("native_setup done");
}

void GESTrackProxy::setTimeline(JNIEnv *env, jobject thiz, jobject timelineObject)
{
  GESTrack *p = getProxied(env, thiz);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return;
  }

/*
  jclass timelineClass = env->FindClass(GESTimelineProxy::className);
  if (timelineClass == NULL) {
    LOGE("Failed to find class %s", GESTimelineProxy::className);
    return;
  }
  GESTimeline *timeline = (GESTimeline*)env->GetIntField(timelineObject, contextField);
*/
  GESTimeline *timeline = GESTimelineProxy::getProxied(env, timelineObject);
  LOGI("setTimeline: timeline from track %p set to %p", p, timeline);
  ges_track_set_timeline(p, timeline);
}

jobject GESTrackProxy::getTimeline(JNIEnv *env, jobject thiz)
{
  GESTrack *p = getProxied(env, thiz);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return NULL;
  }
  const GESTimeline *timeline = ges_track_get_timeline(p);
  LOGI("getTimeline: timeline from track %p is %p", p, timeline);
  if (!timeline)
    return NULL; // It's OK, it can be

  // TODO: constness thrown away
  return newJavaObject(env, GESTimelineProxy::className, (void*)timeline);
}

void GESTrackProxy::setCaps(JNIEnv *env, jobject thiz, jstring capsObject)
{
  GESTrack *p = getProxied(env, thiz);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return;
  }
  const char *caps = env->GetStringUTFChars(capsObject, NULL);
  if (caps == NULL ) {
    jniThrowException(env, "java/lang/RuntimeException", "caps are null");
    return;
  }
  GstCaps *c = gst_caps_from_string(caps);
  g_object_unref(c);
  env->ReleaseStringUTFChars(capsObject, caps);
  ges_track_set_caps(p, c);
}

jobject GESTrackProxy::getCaps(JNIEnv *env, jobject thiz)
{
  GESTrack *p = getProxied(env, thiz);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return NULL;
  }
  const GstCaps *caps = ges_track_get_caps(p);
  if (!caps) {
    jniThrowException(env, "java/lang/RuntimeException", "caps are null");
    return NULL;
  }
  jobject ret = env->NewStringUTF(gst_caps_to_string(caps));
  return ret;
}

static JNINativeMethod gMethods[] = {
  GES_PROXY_BASE_METHODS(GESTrack),
  {"native_setup",        "(Ljava/lang/Object;ILjava/lang/String;)V",    (void *)GESTrackProxy::native_setup},
  {"setTimeline",         "(Landroid/media/ges/GESTimeline;)V",          (void *)GESTrackProxy::setTimeline},
  {"getTimeline",         "()Landroid/media/ges/GESTimeline;",           (void *)GESTrackProxy::getTimeline},
  {"setCaps",             "(Ljava/lang/String;)V",                       (void *)GESTrackProxy::setCaps},
  {"getCaps",             "()Ljava/lang/String;",                        (void *)GESTrackProxy::getCaps},
};

// This function only registers the native methods
int register_android_media_ges_GESTrack(JNIEnv *env)
{
    LOGI("Registering %d native methods for %s", NELEM(gMethods), GESProxy<GESTrack>::className);
    return AndroidRuntime::registerNativeMethods(env,
                GESProxy<GESTrack>::className, gMethods, NELEM(gMethods));
}

