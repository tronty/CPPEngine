#ifndef __AL3__
#define __AL3__
#if defined(ANDROID)
#include "config.h.android"
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#include "config.h.iOS"
#elif defined(_MSC_VER)
#include "config.h.msc"
#else
//if defined(WIN_PHONE_APP)???
#include "config.h.linux"
#endif
#endif

