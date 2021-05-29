/*
 * CAEAGLLayer.h
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
#import "../../orig/QuartzCore/CAEAGLLayer.h"
#import <QuartzCore/CAMetalLayer.h>


#pragma mark -
#pragma mark CAMetalLayer extension

/** Adds EAGLDrawable protocol to CAMetalLayer so it can be used wherever a CAEAGLLayer is used. */
@interface CAMetalLayer (MGLEAGLDrawable) <EAGLDrawable>
@end


/** In any files that import this header file, redefine all references to CAEAGLLayer as MGLCAEAGLLayer instead. */
#if MGL_SUPPORT_OPENGL_ON_METAL
#	define CAEAGLLayer		MGLCAEAGLLayer
#endif


#pragma mark -
#pragma mark CAMetalLayer extension

/**
 * From its initialization methods, MGLCAEAGLLayer vends either a CAMetalLayer or a CAEAGLLayer,
 * depending on whether Metal is active or not.
 */
MLN_PUBLIC_SYMBOL @interface MGLCAEAGLLayer : CALayer <EAGLDrawable>
@end
