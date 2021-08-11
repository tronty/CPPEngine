#ifndef __jconfig_8c__
#define __jconfig_8c__

#if defined(_MSC_VER)
#include "jconfig.vc"
#elif defined(__APPLE__)
#include "jconfig.mac"
#else
#include "jconfig.lnx"
#endif

#endif
