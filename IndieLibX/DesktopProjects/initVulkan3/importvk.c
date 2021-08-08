/* San Angeles Observation OpenVK ES version example
 * Copyright 2004-2005 Jetro Lauha
 * All rights reserved.
 * Web: http://iki.fi/jetro/
 *
 * This source is free software; you can redistribute it and/or
 * modify it under the terms of EITHER:
 *   (1) The GNU Lesser General Public License as published by the Free
 *       Software Foundation; either version 2.1 of the License, or (at
 *       your option) any later version. The text of the GNU Lesser
 *       General Public License is included with this source in the
 *       file LICENSE-LGPL.txt.
 *   (2) The BSD-style license that is included with this source in
 *       the file LICENSE-BSD.txt.
 *
 * This source is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
 * LICENSE-LGPL.txt and LICENSE-BSD.txt for more details.
 *
 * $Id: importvk.c,v 1.4 2005/02/08 18:42:55 tonic Exp $
 * $Revision: 1.4 $
 */
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <stdio.h>
#include "importvk.h"
#ifdef __IMPORTVK__

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
static HMODULE sVULKANDLL = 0;

#define VKPROC(funcName) PFN_##funcName* funcName = 0
#define VKINIT(funcName) funcName = (PFN_##funcName) GetProcAddress(sVULKANDLL,_T(#funcName)); \
        if (funcName == 0) printf("%s=0\n",#funcName);
#elif defined(__APPLE__)
static void *sVULKANDLL = 0;
#define VKPROC(funcName) PFN_##funcName funcName
#define VKINIT(funcName) funcName = (PFN_##funcName) glXGetProcAddress("#funcName"); \
        if (funcName == 0) printf("%s=0\n",#funcName);
#else
#include <stdlib.h>
#include <dlfcn.h>
static void *sVULKANDLL = 0;

#define VKPROC(funcName)          PFN_##funcName funcName

#define VKINIT(funcName) do { \
        void *procAddress = (void *)dlsym(sVULKANDLL, #funcName); \
        if (procAddress == 0) printf("%s=0\n",#funcName); \
        funcName = (PFN_##funcName) procAddress; } while (0)
#endif

    	VKPROC(vkCreateInstance);

int importVKInit()
{

#if defined(_MSC_VER)
    if (sVULKANDLL == 0)
        sVULKANDLL = LoadLibrary(_T("vulkan-1.dll"));
#elif defined(__APPLE__)
    if (sVULKANDLL == 0)
        sVULKANDLL = dlopen("../Libraries/Molten-0.18.0/MoltenVK/macOS/libMoltenVK.dylib");
#else
    if (sVULKANDLL == 0)
        sVULKANDLL = dlopen("libvulkan.so", RTLD_NOW);
#endif
    if (sVULKANDLL == 0)
        return 1;

	VKINIT(vkCreateInstance);

    return 0;
}

void importVKDeinit()
{
#ifdef LINUX
    dlclose(sVULKANDLL);
#elif defined(__APPLE__)
    dlclose(sVULKANDLL);
#elif defined(_MSC_VER)
    FreeLibrary(sVULKANDLL);
#endif
}
#endif

