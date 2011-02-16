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

#define LOG_TAG "GES-JNI"
#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_media_ges_timeline.h"
#include "android_media_ges_track.h"

using namespace android;

static bool setup_ges()
{
  /* gst_init_check wants no consts, and seems to want to modify the contents
     of argc and argv (it takes pointers to both). It doesn't seem to require
     argv[0] set, so we're good to pass it totally empty stuff. */
  char **argv = NULL;
  int argc = 0;
  GError *error = NULL;
  if (!gst_init_check(&argc, &argv, &error)) {
    LOGI("gst_init_check. I have error here. Wanna see it ? Do the code then :P");
    return false;
  }
  ges_init();
  return true;
}

static const struct {
  const char *name;
  int (*init)(JNIEnv*);
} modules[] = {
  { "GESTimeline", register_android_media_ges_GESTimeline },
  { "GESTrack", register_android_media_ges_GESTrack },
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
  JNIEnv* env = NULL;

  if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
    LOGE("ERROR: GetEnv failed\n");
    goto bail;
  }
  assert(env != NULL);
  if (env == NULL) {
    LOGE("ERROR: GetEnv returned a NULL env\n");
    goto bail;
  }

  for (size_t n=0; n<sizeof(modules)/sizeof(modules[0]); ++n) {
    LOGI("Registering %s native methods", modules[n].name);
    if ((*modules[n].init)(env) < 0) {
      LOGE("ERROR: %s native registration failed", modules[n].name);
      goto bail;
    }
  }

  if (!setup_ges()) {
    LOGE("ERROR: GES setup failed");
    goto bail;
  }

  /* success -- return valid version number */
  return JNI_VERSION_1_4;

bail:
  return -1;
}

