/*
** Copyright 2007, The Android Open Source Project
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

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <ges/ges.h>

template<class gesclass>
class GESProxy: public android::RefBase {
private:
  friend class Autolock;
  static android::Mutex sLock;
  static jfieldID contextField;

public:
  GESProxy() { }
  virtual ~GESProxy() { }

  typedef android::sp<GESProxy> ptr;

  static const char * const className;

public:
  /* The base methods exposed to Java/JNI */
  static void native_init(JNIEnv *env);
  static void native_setup(JNIEnv *env, jobject thiz, jobject weak_this);
  static void release(JNIEnv *env, jobject thiz);
  static void native_finalize(JNIEnv *env, jobject thiz);

private:
  class Autolock: public android::Mutex::Autolock {
    public:
      Autolock(): android::Mutex::Autolock(GESProxy::sLock) { }
  };

private:
  static ptr setGESProxy(JNIEnv* env, jobject thiz, const ptr &object);
  static ptr getGESProxy(JNIEnv* env, jobject thiz);
};

#include "android_media_ges_proxy.template"

#define DECLARE_GES_PROXY(c) \
  template class GESProxy<c>; \
  template<> const char * const GESProxy<c>::className = "android/media/ges/"#c; \
  template<class gesclass> Mutex GESProxy<gesclass>::sLock; \
  template<class gesclass> jfieldID GESProxy<gesclass>::contextField

#define GES_PROXY_BASE_METHODS(c) \
  {"native_init",         "()V",                      (void *)GESProxy<c>::native_init}, \
  {"native_setup",        "(Ljava/lang/Object;)V",    (void *)GESProxy<c>::native_setup}, \
  {"_release",            "()V",                      (void *)GESProxy<c>::release}, \
  {"native_finalize",     "()V",                      (void *)GESProxy<c>::native_finalize}

