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

#include "android_media_ges_proxy.h"
#include <ges/ges-track.h>

class GESTrackProxy: public GESProxy<GESTrack> {
protected:
  GESTrackProxy(GESTrackType type, GstCaps *caps);
  virtual ~GESTrackProxy();

private:
  static jobject newJavaObject(JNIEnv *env, const char *type, void *ptr);

public:
  static void native_setup(JNIEnv *env, jobject thiz, jobject weak_this, jint type, jstring capsObject);
  static void setTimeline(JNIEnv*, jobject, jobject timelineObject);
  static jobject getTimeline(JNIEnv*, jobject);
  static void setCaps(JNIEnv*, jobject, jstring capsObject);
  static jobject getCaps(JNIEnv*, jobject);
};

int register_android_media_ges_GESTrack(JNIEnv *env);

