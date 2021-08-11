#ifndef __OSINCLUDE0_H
#define __OSINCLUDE0_H

#if 0
#include "Windows/osincludeMSC.h"
#else

#if defined(LINUX) || defined(__MINGW32__) || defined(__MINGW64__)
#include "Linux/osincludeLNX.h"
#elif defined(_MSC_VER)
#include "Windows/osincludeMSC.h"
#elif defined(__APPLE__) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
#include "Mac/osincludeMAC.h"
#else
#include "Linux/osincludeLNX.h"
#endif

#endif

#endif // __OSINCLUDE0_H
