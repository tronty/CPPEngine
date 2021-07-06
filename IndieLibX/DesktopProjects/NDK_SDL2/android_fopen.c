#if defined(ANDROID)
#include <stdio.h>
//include <android_fopen.h>
#include <errno.h>
#include <android/asset_manager.h>
#include <jni.h>

#include <android/log.h>

#include <unistd.h>

//usleep(500)
#define LOG_TAG "SDL_android"
#if 0
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT
#define LOG_FNLN
#else
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);
//define LOG_PRINT(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);
#define LOG_FNLN __android_log_print(ANDROID_LOG_INFO,LOG_TAG, "%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#endif

#define LOG_FNLN_X LOG_FNLN

extern AAssetManager* g_asset_manager;
extern JNIEnv* g_env;

static int android_read(void* cookie, char* buf, int size) {
LOG_FNLN_X;
    JNIEnv *env = Android_JNI_GetEnv();
LOG_FNLN_X;
int r=AAsset_read((AAsset*)cookie, buf, size);
LOG_FNLN_X;
//(*env)->PopLocalFrame(env, NULL);
LOG_FNLN_X;
return r;
}
static int android_write(void* cookie, const char* buf, int size) {
LOG_FNLN_X;
    JNIEnv *env = Android_JNI_GetEnv();
LOG_FNLN_X;
//(*env)->PopLocalFrame(env, NULL);
LOG_FNLN_X;
return EACCES; // can't provide write access to the apk
}
static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
LOG_FNLN_X;
    JNIEnv *env = Android_JNI_GetEnv();
LOG_FNLN_X;
fpos_t r=AAsset_seek((AAsset*)cookie, offset, whence);
LOG_FNLN_X;
//(*env)->PopLocalFrame(env, NULL);
LOG_FNLN_X;
return r;
}

static int android_close(void* cookie) {
LOG_FNLN_X;
    JNIEnv *env = Android_JNI_GetEnv();
LOG_FNLN_X;
  AAsset_close((AAsset*)cookie);
LOG_FNLN_X;
//(*env)->PopLocalFrame(env, NULL);
LOG_FNLN_X;
  return 0;
}

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
    JNIEnv *env = Android_JNI_GetEnv();
LOG_FNLN_X;
//(*env)->PopLocalFrame(env, NULL);
LOG_FNLN_X;
return r;
}
#endif

