/*
 * Copyright (C) 2008 The Android Open Source Project
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

package android.graphics;

import android.graphics.PorterDuff.Mode;

public class PorterDuffXfermode extends Xfermode {
    private final Mode mMode;

    /**
     * Create an xfermode that uses the specified porter-duff mode.
     *
     * @param mode           The porter-duff mode that is applied
     */
    public PorterDuffXfermode(PorterDuff.Mode mode) {
        mMode = mode;
    }
    
    //---------- Custom Methods
    
    public PorterDuff.Mode getMode() {
        return mMode;
    }
    
    //----------
}