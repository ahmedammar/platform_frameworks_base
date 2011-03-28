/*
 * Copyright (C) 2010 Edward Hervey <bilboed@bilboed.com>
 * Copyright (C) 2011 Collabora Ltd
 */

#define LOG_TAG "GESOps-JNI"
#include "utils/Log.h"

#include <string.h>
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <ges/ges.h>
#include <gst/pbutils/gstdiscoverer.h>
#include <gst/pbutils/encoding-profile.h>

#define TRACE() LOGE("%s:%d",__FILE__,__LINE__)

#include "cut_worker.cpp"

void gesops_cut_jni(JNIEnv *env, jobject thiz, jstring inputUriObject, jint start_ms, jint stop_ms, jstring outputUriObject)
{
  char *input_uri_copy = NULL, *output_uri_copy = NULL;
  const char *input_uri = NULL, *output_uri = NULL;
  bool throwException = false;

  TRACE();
  guint64 start = start_ms*(guint64)(GST_SECOND/1000);
  guint64 duration = (stop_ms-start_ms)*(guint64)(GST_SECOND/1000);
  TRACE();

  input_uri = env->GetStringUTFChars(inputUriObject, NULL);
  if (input_uri == NULL ) {
    LOGE("input URI is NULL");
    goto bail;
  }

  output_uri = env->GetStringUTFChars(outputUriObject, NULL);
  if (output_uri == NULL ) {
    LOGE("output URI is NULL");
    goto bail;
  }

  TRACE();

  /* TODO: I'm not convinced it's really const inside GES, so pass a
   * dupe for safety for now
   */
  input_uri_copy = strdup(input_uri);
  output_uri_copy = strdup(output_uri);

  TRACE();
  LOGE("Calling worker");
  if (!gesops_cut(input_uri_copy, start, duration, output_uri_copy))
    goto bail;
  LOGE("worker back");

end:
  TRACE();
  if (input_uri) env->ReleaseStringUTFChars(inputUriObject, input_uri);
  if (output_uri) env->ReleaseStringUTFChars(outputUriObject, output_uri);

  TRACE();
  if (throwException)
    jniThrowException(env, "java/lang/RuntimeException", "Failed to cut");
  TRACE();
  return;

bail:
  TRACE();
  throwException = true;
  goto end;
}

jint gesops_cut_start_jni(JNIEnv *env, jobject thiz, jstring inputUriObject, jint start_ms, jint stop_ms, jstring outputUriObject)
{
  char *input_uri_copy = NULL, *output_uri_copy = NULL;
  const char *input_uri = NULL, *output_uri = NULL;
  bool throwException = false;
  jint cookie = 0;

  TRACE();
  guint64 start = start_ms*(guint64)(GST_SECOND/1000);
  guint64 duration = (stop_ms-start_ms)*(guint64)(GST_SECOND/1000);
  TRACE();

  input_uri = env->GetStringUTFChars(inputUriObject, NULL);
  if (input_uri == NULL ) {
    LOGE("input URI is NULL");
    goto bail;
  }

  output_uri = env->GetStringUTFChars(outputUriObject, NULL);
  if (output_uri == NULL ) {
    LOGE("output URI is NULL");
    goto bail;
  }

  TRACE();

  /* TODO: I'm not convinced it's really const inside GES, so pass a
   * dupe for safety for now
   */
  input_uri_copy = strdup(input_uri);
  output_uri_copy = strdup(output_uri);

  TRACE();
  LOGE("Calling worker");
  //LOGE(" ************ ASYNC DISABLED **************");
  cookie = gesops_cut_start(input_uri_copy, start, duration, output_uri_copy);
  if (cookie == 0) {
    LOGE("Worker failed to start");
    goto bail;
  }
  LOGE("worker back, async");

end:
  TRACE();
  if (input_uri) env->ReleaseStringUTFChars(inputUriObject, input_uri);
  if (output_uri) env->ReleaseStringUTFChars(outputUriObject, output_uri);

  TRACE();
  if (throwException)
    jniThrowException(env, "java/lang/RuntimeException", "Failed to cut");
  TRACE();
  return cookie;

bail:
  TRACE();
  throwException = true;
  goto end;
}

jboolean gesops_cut_update_jni(JNIEnv *env, jobject thiz, jint cookie)
{
  return gesops_cut_update(cookie);
}

static JNINativeMethod gMethods[] = {
  {"cut",                   "(Ljava/lang/String;IILjava/lang/String;)V",                (void *)gesops_cut_jni},
  {"cut_start",             "(Ljava/lang/String;IILjava/lang/String;)I",                (void *)gesops_cut_start_jni},
  {"cut_update",            "(I)Z",                                                     (void *)gesops_cut_update_jni},
};

int register_android_media_ges_ops(JNIEnv *env)
{
    LOGI("Registering %d native methods for GES ops", NELEM(gMethods));
    return android::AndroidRuntime::registerNativeMethods(env,
                "android/media/ges/GESOps", gMethods, NELEM(gMethods));
}

