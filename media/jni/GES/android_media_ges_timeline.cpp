/*
** Copyright 2010 Collabora Ltd
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

#define LOG_TAG "GESTimeline-JNI"
#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_media_ges_timeline.h"

#include <ges/ges.h>

using namespace android;

DECLARE_GES_PROXY(GESTimeline);

GESTimelineProxy::GESTimelineProxy():
  GESProxy<GESTimeline>(ges_timeline_new())
{
  LOGI("GESTimelineProxy::GESTimelineProxy");
}

GESTimelineProxy::~GESTimelineProxy()
{
  LOGI("GESTimelineProxy::~GESTimelineProxy");
  g_object_unref(proxied);
}

void GESTimelineProxy::native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
  LOGI("native_setup");
  GESTimelineProxy *p = new GESTimelineProxy();
  if (p == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
    return;
  }

  // Stow our new C++ object in an opaque field in the Java object.
  LOGI("native_setup storing");
  setGESProxy(env, thiz, p);
  LOGI("native_setup done");
}

jboolean GESTimelineProxy::loadFromURI(JNIEnv *env, jobject thiz, jstring uriObject)
{
  LOGI("loadFromURI");
  GESTimeline *p = getProxied(env, thiz);
  LOGI("loadFromURI, proxied %p",p);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return false;
  }
  const char *uri = env->GetStringUTFChars(uriObject, NULL);
  if (uri == NULL ) {
    jniThrowException(env, "java/lang/RuntimeException", "uri is null");
    return false;
  }

  LOGI("loadFromURI: uri %s", uri);
  jboolean ret = ges_timeline_load_from_uri(p, uri);
  LOGI("loadFromURI returned %s", ret?"true":"false");

  // Make sure that local ref is released before a potential exception
  env->ReleaseStringUTFChars(uriObject, uri);

  return ret;
}

jboolean GESTimelineProxy::saveToURI(JNIEnv *env, jobject thiz, jstring uriObject)
{
  GESTimeline *p = getProxied(env, thiz);
  if (p == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return false;
  }
  const char *uri = env->GetStringUTFChars(uriObject, NULL);
  if (uri == NULL ) {
    jniThrowException(env, "java/lang/RuntimeException", "uri is null");
    return false;
  }

  LOGI("saveToURI: uri %s", uri);
  jboolean ret = ges_timeline_save_to_uri(p, uri);
  LOGI("saveToURI returned %s", ret?"true":"false");

  // Make sure that local ref is released before a potential exception
  env->ReleaseStringUTFChars(uriObject, uri);

  return ret;
}

static JNINativeMethod gMethods[] = {
  GES_PROXY_BASE_METHODS(GESTimeline),
  {"native_setup",        "(Ljava/lang/Object;)V",    (void *)GESTimelineProxy::native_setup},
  {"loadFromURI",         "(Ljava/lang/String;)Z",    (void *)GESTimelineProxy::loadFromURI},
  {"saveToURI",           "(Ljava/lang/String;)Z",    (void *)GESTimelineProxy::saveToURI},
};

// This function only registers the native methods
int register_android_media_ges_GESTimeline(JNIEnv *env)
{
    LOGI("Registering %d native methods for %s", NELEM(gMethods), GESProxy<GESTimeline>::className);
    return AndroidRuntime::registerNativeMethods(env,
                GESProxy<GESTimeline>::className, gMethods, NELEM(gMethods));
}

