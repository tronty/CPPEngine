#if defined(ANDROID)
/*
 * Copyright (C) 2009 The Android Open Source Project
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

// OpenGL ES 2.0 code

//include <STX/STX.h>

#include <stdio.h>
#include <android/asset_manager.h>

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdlib.h>

#include <errno.h>

#include <unistd.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define  LOG_PRINT(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOG_FNLN __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__)

#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#define LOG_PRINT_X LOG_PRINT
#define LOG_FNLN_X LOG_FNLN
#else
#define LOG_PRINT_X
#define LOG_FNLN_X
#endif

JNIEnv* g_env=0;

/*
static void 	getMemoryInfo(Debug.MemoryInfo memoryInfo)
Retrieves information about this processes memory usages.
static int 	getNativeHeapAllocatedSize()
Returns the amount of allocated memory in the native heap.
static int 	getNativeHeapFreeSize()
Returns the amount of free memory in the native heap.
static int 	getNativeHeapSize()
Returns the size of the native heap. 
*/
int getNativeHeapSize();
int getNativeHeapAllocatedSize();
int getNativeHeapFreeSize();

static int android_read(void* cookie, char* buf, int size) {
LOG_FNLN_X;
    
LOG_FNLN_X;
int r=AAsset_read((AAsset*)cookie, buf, size);
LOG_FNLN_X;

LOG_FNLN_X;
return r;
}
static int android_write(void* cookie, const char* buf, int size) {
LOG_FNLN_X;
    
LOG_FNLN_X;

LOG_FNLN_X;
return EACCES; // can't provide write access to the apk
}
static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
LOG_FNLN_X;
    
LOG_FNLN_X;
fpos_t r=AAsset_seek((AAsset*)cookie, offset, whence);
LOG_FNLN_X;

LOG_FNLN_X;
return r;
}

static int android_close(void* cookie) {
LOG_FNLN_X;
    
LOG_FNLN_X;
  AAsset_close((AAsset*)cookie);
LOG_FNLN_X;

LOG_FNLN_X;
  return 0;
}

AAssetManager* g_asset_manager=0;
FILE* android_fopen(const char* fname, const char* mode)
{
LOG_FNLN_X;
if (g_asset_manager==0) return NULL;
LOG_FNLN_X;
if(mode[0] == 'w') return NULL;
LOG_FNLN_X;
AAsset* asset = AAssetManager_open(g_asset_manager, fname, 0);
LOG_FNLN_X;
if(!asset) return NULL;
LOG_FNLN_X;
FILE* r=funopen(asset, android_read, android_write, android_seek, android_close);
LOG_FNLN_X;
    
LOG_FNLN_X;

LOG_FNLN_X;
return r;
}

JNIEXPORT jboolean JNICALL Java_com_android_gl2jni_GL2JNILib_nativeAssets(JNIEnv *env, jobject obj, jobject assetManager)
//JNIEXPORT jboolean JNICALL Java_com_android_gl2jni_GL2JNILib_nativeAssets(JNIEnv* env, jclass clazz, jobject assetManager)
{
LOG_FNLN_X("GL2JNI_nativeAssets called1");
    g_env=env;
LOG_FNLN_X("GL2JNI_nativeAssets called2");
    //JNIEnv *env2 = Android_JNI_GetEnv();
LOG_FNLN_X("GL2JNI_nativeAssets called3");
	//(*env2)->PushLocalFrame(env2, 10);
LOG_FNLN_X("GL2JNI_nativeAssets called4");
	//AAssetManager* mgr = AAssetManager_fromJava(assetManager);



AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == 0) {
LOG_FNLN_X("GL2JNI_nativeAssets called5:(mgr == 0)");
	return JNI_TRUE;
}



LOG_FNLN_X("GL2JNI_nativeAssets called5");
g_asset_manager=mgr;
LOG_FNLN_X("GL2JNI_nativeAssets called6");
	return JNI_TRUE;
}

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOG_FNLN_X("GL %s = %s\n", name, v);
}

/*
static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOG_FNLN_X("after %s() glError (0x%x)\n", op, error);
    }
}
*/
//extern "C" {
	int init();
	int render();
    //JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    //JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
//};

int Java_com_android_gl2jni_GL2JNILib_inited=0;

void setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOG_FNLN_X("setupGraphics(%d, %d)", w, h);

    glViewport(0, 0, w, h);
    //checkGlError("glViewport");
	init();
	
	Java_com_android_gl2jni_GL2JNILib_inited=1;
if(0)
{
	LOG_PRINT("NativeHeapAllocatedSize=%ld\n", getNativeHeapAllocatedSize());
	LOG_PRINT("NativeHeapSize=%ld\n", getNativeHeapSize());
	LOG_PRINT("NativeHeapFreeSize=%ld\n", getNativeHeapFreeSize());
}
}



void renderFrame() {

	if(!Java_com_android_gl2jni_GL2JNILib_inited) return;

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    //checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //checkGlError("glClear");
	render();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
#endif

