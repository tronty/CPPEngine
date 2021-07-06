#ifndef _TIF_CONFIG_H_____
#define _TIF_CONFIG_H_____

#ifdef _MSC_VER
#include "tif_config.vc.h"
#elif defined(ANDROID)
#include "tif_config.ndk.h"
#else
#include "tif_config.lnx.h"
#endif

#endif

