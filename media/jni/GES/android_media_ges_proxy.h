/*
** Copyright 2007, The Android Open Source Project
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

#ifndef included_ndroid_media_ges_proxy_jni
#define included_ndroid_media_ges_proxy_jni

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

protected:
  gesclass *proxied;

protected:
  GESProxy(gesclass *p): proxied(p) { }
  virtual ~GESProxy() { }

public:
  typedef android::sp<GESProxy> ptr;

  static const char * const className;

public:
  /* The base methods exposed to Java/JNI */
  static void native_init(JNIEnv *env);
  static void release(JNIEnv *env, jobject thiz);
  static void native_finalize(JNIEnv *env, jobject thiz);

private:
  class Autolock: public android::Mutex::Autolock {
    public:
      Autolock(): android::Mutex::Autolock(GESProxy::sLock) { }
  };

public:
  static gesclass *getProxied(JNIEnv *env, jobject thiz) { return getGESProxy(env, thiz)->proxied; }

protected:
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
  {"native_init",         "()V",                      (void *)c##Proxy::native_init}, \
  {"_release",            "()V",                      (void *)c##Proxy::release}, \
  {"native_finalize",     "()V",                      (void *)c##Proxy::native_finalize}

#endif

