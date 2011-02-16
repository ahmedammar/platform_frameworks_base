/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2010 Collabora Ltd
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

public class GESTrack
{
    private final static String TAG = "GESTrack";

    static {
        loadLibrary();
        native_init();
    }

    private int mNativeContext; // accessed by native methods

    public GESTrack() {
        /* Native setup requires a weak reference to our object.
         * It's easier to create it here than in C++.
         */
        Log.i("GES-JNI/Java",TAG+" ctor");
        native_setup(new WeakReference<GESTrack>(this));
    }

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
    private native final void native_setup(Object o);
    private native final void native_finalize();

    @Override
    protected void finalize() { native_finalize(); }
}
