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

#define LOG_TAG "GES-JNI"
#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_media_ges_track.h"

#include <ges/ges.h>

using namespace android;

DECLARE_GES_PROXY(GESTrack);

GESTrackProxy::GESTrackProxy():
  GESProxy<GESTrack>(NULL) // TODO
{
}

GESTrackProxy::~GESTrackProxy()
{
  g_object_unref(proxied);
}

void GESTrackProxy::native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    LOGV("native_setup");
    GESTrackProxy *p = new GESTrackProxy();
    if (p == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
      return;
    }

    // Stow our new C++ object in an opaque field in the Java object.
    LOGV("native_setup storing");
    setGESProxy(env, thiz, p);
    LOGV("native_setup done");
}


static JNINativeMethod gMethods[] = {
  GES_PROXY_BASE_METHODS(GESTrack),
  {"native_setup",        "(Ljava/lang/Object;)V",    (void *)GESTrackProxy::native_setup},
};

// This function only registers the native methods
int register_android_media_ges_GESTrack(JNIEnv *env)
{
    LOGI("Registering %d native methods for %s", NELEM(gMethods), GESProxy<GESTrack>::className);
    return AndroidRuntime::registerNativeMethods(env,
                GESProxy<GESTrack>::className, gMethods, NELEM(gMethods));
}

