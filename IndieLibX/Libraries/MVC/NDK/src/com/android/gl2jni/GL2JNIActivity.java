/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.gl2jni;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import java.io.File;

import android.content.res.AssetManager;
import android.util.Log;

public class GL2JNIActivity extends Activity {
    private static final String TAG = "GL2JNIActivity";

    GL2JNIView mView;
    private static AssetManager assetManager;
    public static native boolean nativeAssets(AssetManager assetManager);

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GL2JNIView(getApplication());
	setContentView(mView);

	//Log.v(TAG, "assetManager_1:");
	assetManager = getAssets();
	//Log.v(TAG, "assetManager_2:");
	mView.nativeAssets(assetManager);
	//Log.v(TAG, "assetManager_3:");
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
/*
    @Override public boolean onKeyUp(int keyCode, KeyEvent event) {
    	char pressedKey = (char) event.getUnicodeChar();
    	Log.v(TAG, "onKeyUp is: ", Character.toString(pressedKey));
    	return super.onKeyUp(keyCode, event);
    }
*/
}
