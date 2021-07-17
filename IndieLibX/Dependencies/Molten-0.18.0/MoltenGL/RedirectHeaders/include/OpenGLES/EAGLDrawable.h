/*
 * EAGLDrawable.h
 *
 * Copyright (c) 2014-2017 The Brenwill Workshop Ltd. All rights reserved.
 * http://www.brenwill.com
 *
 * Use of this document is governed by the MoltenGL License Agreement, as included
 * in the MoltenGL distribution package. CAREFULLY READ THAT LICENSE AGREEMENT BEFORE
 * READING AND USING THIS DOCUMENT. BY READING OR OTHERWISE USING THIS DOCUMENT,
 * YOU ACCEPT AND AGREE TO BE BOUND BY THE TERMS AND CONDITIONS OF THAT LICENSE
 * AGREEMENT. IF YOU DO NOT ACCEPT THE TERMS AND CONDITIONS OF THAT LICENSE AGREEMENT,
 * DO NOT READ OR USE THIS DOCUMENT.
 */

/** @file */	// Doxygen marker


#import <MoltenGL/mglEnv.h>
#import "EAGL.h"
#import "../../orig/OpenGLES/EAGLDrawable.h"

#if MGL_SUPPORT_OPENGL_ON_METAL


/* Extends EAGLContext interface */
@interface EAGLContext (MGLEAGLContextDrawableAdditions)

/** Replicates behaviour of EAGLContext extension. */
-(BOOL) renderbufferStorage: (NSUInteger) target fromDrawable: (id<EAGLDrawable>) drawable;

/** Replicates behaviour of EAGLContext extension. */
-(BOOL) presentRenderbuffer: (NSUInteger) target;

@end


#endif	// MGL_SUPPORT_OPENGL_ON_METAL
