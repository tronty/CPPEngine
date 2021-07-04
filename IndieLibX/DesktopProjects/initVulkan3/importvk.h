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
 * $Id: importvk.h,v 1.4 2005/02/24 20:29:33 tonic Exp $
 * $Revision: 1.4 $
 */
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if !defined(_MSC_VER)
#ifndef __IMPORTVK__
#define __IMPORTVK__

#define VK_NO_PROTOTYPES

#ifdef __APPLE__
#define VK_USE_PLATFORM_XLIB_KHR 1
#include "../../Libraries/Molten-0.18.0/MoltenVK/include/vulkan/vulkan.h"
#elif defined(_MSC_VER)
#define VK_USE_PLATFORM_WIN32_KHR 1
#include "../../Libraries/Vulkan/include/vulkan/vulkan.h"
#else
#define VK_USE_PLATFORM_XLIB_KHR 1
#include "../../Libraries/Vulkan/include/vulkan/vulkan.h"
#endif

#define VKTYPEDEF(retType, funcName, args) typedef retType (VKAPI_PTR *PFN_##funcName) args

#define VKEXTERN(funcName) extern PFN_##funcName funcName

VKTYPEDEF(VkResult, vkCreateInstance, (const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*));

#ifdef __cplusplus
extern "C" {
#endif
	VKEXTERN(vkCreateInstance);
	int importVKInit();
	void importVKDeinit();
#ifdef __cplusplus
}
#endif
#endif
#endif

