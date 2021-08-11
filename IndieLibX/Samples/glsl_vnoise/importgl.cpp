/* San Angeles Observation OpenGL ES version example
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
 * $Id: importgl.c,v 1.4 2005/02/08 18:42:55 tonic Exp $
 * $Revision: 1.4 $
 */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "importgl.h"
#ifdef IMPORTGL_H_INCLUDED
#if defined(WIN_PHONE_APP) || defined(WIN8_APP)
#if defined(WIN_PHONE_APP)
int GL_ARB_shading_language_100_supported=0;
#else
int GL_ARB_shading_language_100_supported=1;
#endif
int GL_EXT_texture_filter_anisotropic_supported=1;
#else
#if defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
int GL_ARB_shading_language_100_supported=0;
#else
int GL_ARB_shading_language_100_supported=1;
#endif
int GL_ARB_fragment_shader_supported=1;
int GL_ARB_fragment_program_supported=1;
int GL_ARB_draw_buffers_supported=1;
int GL_EXT_texture_filter_anisotropic_supported=1;
#endif

#ifdef _MSC_VER

#ifdef _MSC_VER
#define WIN32_x
#elif defined(ANDROID)
#define LINUX_x
#else
#define LINUX_x
#endif

#ifdef _MSC_VER
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
//undef WIN32
#undef LINUX
#endif

#define IMPORTGL_NO_FNPTR_DEFS
#define IMPORTGL_API
#define IMPORTGL_FNPTRINIT = 0

#if defined(WIN32_x)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
static HMODULE sGLESDLL = 0;

#define GLPROC(funcName) PFNPROC_##funcName funcName = 0
#define GLINIT(funcName) funcName = (PFNPROC_##funcName) wglGetProcAddress(_T(#funcName)); \
        if (funcName == 0) LOG_PRINT("%s=0\n",#funcName);
#endif

#if defined(APPLE_x)
#define GLPROC(funcName) PFNPROC_##funcName funcName = 0
#define GLINIT(funcName) funcName = (PFNPROC_##funcName) wglxGetProcAddress("#funcName"); \
        if (funcName == 0) LOG_PRINT("%s=0\n",#funcName);
#define GLINIT2
#endif

#ifdef LINUX_x
#include <stdlib.h>
#include <dlfcn.h>
static void *sGLESSO = 0;
static void *sGLUSO = 0;

#define GLPROC(funcName) PFNPROC_##funcName funcName = 0
#define GLINIT(funcName) do { \
        void *procAddress = (void *)dlsym(sGLESSO, #funcName); \
        if (procAddress == 0) LOG_PRINT("%s=0\n",#funcName); \
        funcName = (PFNPROC_##funcName) procAddress; } while (0)
#define GLINIT2(funcName) do { \
        void *procAddress = (void *)dlsym(sGLUSO, #funcName); \
        if (procAddress == 0) LOG_PRINT("%s=0\n",#funcName); \
        funcName = (PFNPROC_##funcName) procAddress; } while (0)
#endif

#ifndef DISABLE_IMPORTGL

#if defined(_MSC_VER)
    	GLPROC(glUseProgram);
    	GLPROC(glUniform1i);
    	GLPROC(glShaderSource);
    	GLPROC(glLinkProgram);
    	GLPROC(glGetProgramiv);
    	GLPROC(glGetAttribLocation);
    	GLPROC(glGetUniformLocation);
    	GLPROC(glGetActiveAttrib);
    	GLPROC(glGetActiveUniform);
    	GLPROC(glCreateProgram);
    	GLPROC(glCreateShader);
    	GLPROC(glCompileShader);
    	GLPROC(glAttachShader);
#elif defined(_MSC_VER)
#if !defined(USE_GLES_SHADERS)
    	GLPROC(glBindBufferARB);
	GLPROC(glBindFramebufferEXT);
	GLPROC(glBindRenderbufferEXT);
	GLPROC(glBlendEquationSeparate);
	GLPROC(glBlendFuncSeparate);
	GLPROC(glBufferDataARB);
	GLPROC(glCompressedTexImage2DARB);
	GLPROC(glCompressedTexImage3DARB);
	GLPROC(glDeleteBuffers);
	GLPROC(glDeleteBuffersARB);
	GLPROC(glDeleteRenderbuffersEXT);
	GLPROC(glDrawBuffersARB);
	GLPROC(glFramebufferRenderbufferEXT);
	GLPROC(glFramebufferTexture2DEXT);
	GLPROC(glGenBuffersARB);
	GLPROC(glGenFramebuffersEXT);
	GLPROC(glGenRenderbuffersEXT);
	GLPROC(glMapBufferARB);
	GLPROC(glPointParameterfARB);
	GLPROC(glPointParameterfvARB);
	GLPROC(glRenderbufferStorageEXT);
	GLPROC(glTexImage3D);
	GLPROC(glUnmapBufferARB);
	GLPROC(glVertexAttribPointer);
	GLPROC(glEnableVertexAttribArray);
	GLPROC(glDeleteFramebuffersEXT);
	GLPROC(glEnableVertexAttribArrayARB);
	GLPROC(glDisableVertexAttribArrayARB);
	GLPROC(glClientActiveTexture);
#else
	GLPROC(glAttachShader);
	GLPROC(glBindAttribLocation);
	GLPROC(glBindBuffer);
	GLPROC(glBindFramebuffer);
	GLPROC(glBindRenderbuffer);
	GLPROC(glBlendEquationSeparate);
	GLPROC(glBlendFuncSeparate);
	GLPROC(glBufferData);
	GLPROC(glCompileShader);
	GLPROC(glCompressedTexImage2D);
	GLPROC(glCreateProgram);
	GLPROC(glCreateShader);
	GLPROC(glDeleteBuffers);
	GLPROC(glDeleteFramebuffers);
	GLPROC(glDeleteProgram);
	GLPROC(glDeleteRenderbuffers);
	GLPROC(glDeleteShader);
	GLPROC(glDisableVertexAttribArray);
	GLPROC(glEnableVertexAttribArray);
	GLPROC(glFramebufferRenderbuffer);
	GLPROC(glFramebufferTexture2D);
	GLPROC(glGenBuffers);
	GLPROC(glGenFramebuffers);
	GLPROC(glGenRenderbuffers);
	GLPROC(glGetActiveAttrib);
	GLPROC(glGetActiveUniform);
	GLPROC(glGetAttribLocation);
	GLPROC(glGetProgramInfoLog);
	GLPROC(glGetProgramiv);
	GLPROC(glGetShaderInfoLog);
	GLPROC(glGetShaderiv);
	GLPROC(glGetUniformLocation);
	GLPROC(glLinkProgram);
	GLPROC(glRenderbufferStorage);
	GLPROC(glShaderSource);
	GLPROC(glUniform1fv);
	GLPROC(glUniform1i);
	GLPROC(glUniform1iv);
	GLPROC(glUniform2fv);
	GLPROC(glUniform2iv);
	GLPROC(glUniform3fv);
	GLPROC(glUniform3iv);
	GLPROC(glUniform4fv);
	GLPROC(glUniform4iv);
	GLPROC(glUniformMatrix2fv);
	GLPROC(glUniformMatrix3fv);
	GLPROC(glUniformMatrix4fv);
	GLPROC(glUseProgram);
	GLPROC(glVertexAttribPointer);
#endif
#endif

#if !defined(WIN32_x)
    inline void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOG_PRINT("GL %s = %s\n", name, v);
};
    #endif
#ifdef _MSC_VER
int importGLInit()
{
    int result = 1;
    #if !defined(WIN32_x)
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    #endif
#ifndef DISABLE_IMPORTGL

#undef IMPORT_FUNC

#ifdef ANDROID
    if (sGLESSO == 0)
        sGLESSO = dlopen("libGLESv2.so", RTLD_NOW);
#elif defined(LINUX_x)
    if (sGLUSO == 0)
        sGLUSO = dlopen("libGLU.so", RTLD_NOW);
    if (sGLESSO == 0)
        sGLESSO = dlopen("libGL.so", RTLD_NOW);
#ifdef ANDROID_NDK
    if (sGLESSO == 0)
        sGLESSO = dlopen("libGLESv1_CM.so", RTLD_NOW);
#else
    if (sGLESSO == 0)
        sGLESSO = dlopen("libGLES_CM.so", RTLD_NOW);
    if (sGLESSO == 0)
        sGLESSO = dlopen("libGLES_CL.so", RTLD_NOW);
#endif
    if (sGLESSO == 0)
        return 0;
#elif defined(WIN32_x)
    if (sGLESDLL == 0)
        sGLESDLL = LoadLibrary(_T("OpenGL32.dll"));
    if (sGLESDLL == 0)
        sGLESDLL = LoadLibrary(_T("libGLES_CM.dll"));
    if (sGLESDLL == 0)
        sGLESDLL = LoadLibrary(_T("libGLES_CL.dll"));
    if (sGLESDLL == 0)
        return 0;
#endif
#ifndef DISABLE_IMPORTGL

#if defined(_MSC_VER)
    	GLINIT(glUseProgram);
    	GLINIT(glUniform1i);
    	GLINIT(glShaderSource);
    	GLINIT(glLinkProgram);
    	GLINIT(glGetProgramiv);
    	GLINIT(glGetAttribLocation);
    	GLINIT(glGetUniformLocation);
    	GLINIT(glGetActiveAttrib);
    	GLINIT(glGetActiveUniform);
    	GLINIT(glCreateProgram);
    	GLINIT(glCreateShader);
    	GLINIT(glCompileShader);
    	GLINIT(glAttachShader);
#elif defined(_MSC_VER)
#if !defined(USE_GLES_SHADERS)
	GLINIT(glBindBufferARB);
	GLINIT(glBindFramebufferEXT);
	GLINIT(glBindRenderbufferEXT);
	GLINIT(glBlendEquationSeparate);
	GLINIT(glBlendFuncSeparate);
	GLINIT(glBufferDataARB);
	GLINIT(glCompressedTexImage2DARB);
	GLINIT(glCompressedTexImage3DARB);
	GLINIT(glDeleteBuffers);
	GLINIT(glDeleteBuffersARB);
	GLINIT(glDeleteRenderbuffersEXT);
	GLINIT(glDrawBuffersARB);
	GLINIT(glFramebufferRenderbufferEXT);
	GLINIT(glFramebufferTexture2DEXT);
	GLINIT(glGenBuffersARB);
	GLINIT(glGenFramebuffersEXT);
	GLINIT(glGenRenderbuffersEXT);
	GLINIT(glMapBufferARB);
	GLINIT(glPointParameterfARB);
	GLINIT(glPointParameterfvARB);
	GLINIT(glRenderbufferStorageEXT);
	GLINIT(glTexImage3D);
	GLINIT(glUnmapBufferARB);
	GLINIT(glVertexAttribPointer);
	GLINIT(glEnableVertexAttribArray);
	GLINIT(glDeleteFramebuffersEXT);
	GLINIT(glEnableVertexAttribArrayARB);
	GLINIT(glDisableVertexAttribArrayARB);
	GLINIT(glClientActiveTexture);
#else
	GLINIT(glAttachShader);
	GLINIT(glBindAttribLocation);
	GLINIT(glBindBuffer);
	GLINIT(glBindFramebuffer);
	GLINIT(glBindRenderbuffer);
	GLINIT(glBlendEquationSeparate);
	GLINIT(glBlendFuncSeparate);
	GLINIT(glBufferData);
	GLINIT(glCompileShader);
	GLINIT(glCompressedTexImage2D);
	GLINIT(glCreateProgram);
	GLINIT(glCreateShader);
	GLINIT(glDeleteBuffers);
	GLINIT(glDeleteFramebuffers);
	GLINIT(glDeleteProgram);
	GLINIT(glDeleteRenderbuffers);
	GLINIT(glDeleteShader);
	GLINIT(glDisableVertexAttribArray);
	GLINIT(glEnableVertexAttribArray);
	GLINIT(glFramebufferRenderbuffer);
	GLINIT(glFramebufferTexture2D);
	GLINIT(glGenBuffers);
	GLINIT(glGenFramebuffers);
	GLINIT(glGenRenderbuffers);
	GLINIT(glGetActiveAttrib);
	GLINIT(glGetActiveUniform);
	GLINIT(glGetAttribLocation);
	GLINIT(glGetProgramInfoLog);
	GLINIT(glGetProgramiv);
	GLINIT(glGetShaderInfoLog);
	GLINIT(glGetShaderiv);
	GLINIT(glGetUniformLocation);
	GLINIT(glLinkProgram);
	GLINIT(glRenderbufferStorage);
	GLINIT(glShaderSource);
	GLINIT(glUniform1fv);
	GLINIT(glUniform1i);
	GLINIT(glUniform1iv);
	GLINIT(glUniform2fv);
	GLINIT(glUniform2iv);
	GLINIT(glUniform3fv);
	GLINIT(glUniform3iv);
	GLINIT(glUniform4fv);
	GLINIT(glUniform4iv);
	GLINIT(glUniformMatrix2fv);
	GLINIT(glUniformMatrix3fv);
	GLINIT(glUniformMatrix4fv);
	GLINIT(glUseProgram);
	GLINIT(glVertexAttribPointer);
#endif
#endif
#endif
#endif
    return result;
}

void importGLDeinit()
{
#ifndef DISABLE_IMPORTGL
#ifdef LINUX_x
    dlclose(sGLESSO);
    dlclose(sGLUSO);
#endif
#ifdef WIN32_x
    FreeLibrary(sGLESDLL);
#endif
#endif
}
#endif
#endif
#endif
#endif

