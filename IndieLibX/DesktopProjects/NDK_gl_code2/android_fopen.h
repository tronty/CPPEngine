#ifndef ANDROID_FOPEN_H
#define ANDROID_FOPEN_H
#if defined(ANDROID)
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
/* hijack fopen and route it through the android asset system so that
we can pull things out of our packagesk APK */
FILE* android_fopen(const char* fname, const char* mode);
#define fopen(name, mode) android_fopen(name, mode)
#ifdef __cplusplus
}
#endif
#endif
#endif

