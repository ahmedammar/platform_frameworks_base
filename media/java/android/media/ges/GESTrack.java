/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2011 Collabora Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.media.ges;

import android.util.Log;

import java.io.IOException;
import java.lang.ref.WeakReference;

public class GESTrack extends GESBase
{
    private final static String TAG = "GESTrack";

    static {
        loadLibrary();
        native_init();
    }

    private int mNativeContext; // accessed by native methods

    // XXX: synced to the GESTrackType enum
    public static final int TYPE_UNKNOWN = 1 << 0;
    public static final int TYPE_AUDIO   = 1 << 1;
    public static final int TYPE_VIDEO   = 1 << 2;
    public static final int TYPE_TEXT    = 1 << 3;
    public static final int TYPE_CUSTOM  = 1 << 4;

    public static String rawAudioCaps = "audio/x-raw-int;audio/x-raw-float";
    public static String rawVideoCaps = "video/x-raw-yuv;video/x-raw-rgb";

    public GESTrack(int type, String caps) {
        /* Native setup requires a weak reference to our object.
         * It's easier to create it here than in C++.
         */
        Log.i("GES-JNI/Java",TAG+" ctor/type");
        native_setup(new WeakReference<GESTrack>(this), type, caps);
    }

    public native void setTimeline(GESTimeline timeline);
    public native final GESTimeline getTimeline();
    public native void setCaps(String caps);
    public native String getCaps();

    /**
     * Releases resources associated with this GESTrack object.
     * It is considered good practice to call this method when you're
     * done using the GESTrack.
     */
    public void release() {
        Log.i("GES-JNI/Java",TAG+"::release");
        _release();
    }

    private native void _release();

    private static native final void native_init();
    private native final void native_setup(Object o, int type, String caps);
    private native final void native_finalize();

    @Override
    protected void finalize() { native_finalize(); }
}
