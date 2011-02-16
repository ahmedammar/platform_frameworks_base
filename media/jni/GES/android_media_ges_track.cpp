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
#include "android_media_ges_proxy.h"

#include <ges/ges.h>

using namespace android;

DECLARE_GES_PROXY(GESTrack);

//template<class gesclass> Mutex GESProxy<gesclass>::sLock;

static JNINativeMethod gMethods[] = {
  GES_PROXY_BASE_METHODS(GESTrack)
};

// This function only registers the native methods
int register_android_media_ges_GESTrack(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(env,
                GESProxy<GESTrack>::className, gMethods, NELEM(gMethods));
}

