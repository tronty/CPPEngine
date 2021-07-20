#ifndef __zconf_h__
#define __zconf_h__

#if defined(__MINGW32__) || defined(__MINGW64__)
#include "zconf._vc.h"
#elif defined(_MSC_VER)
#include "zconf.vc.h"
#else
#include "zconf._vc.h"
#endif

#endif
