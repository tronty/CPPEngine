/*
 * GLKView.h
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
#import "../../orig/GLKit/GLKView.h"
#import "../OpenGLES/EAGL.h"


/** In any files that import this header file, redefine all references to GLKView as MGLGLKView instead. */
#if (MGL_SUPPORT_OPENGL_ON_METAL || MLN_MACOS)
#	define GLKView		MGLGLKView
#endif

/**
 * MGLGLKView replicates the functionality of GLKView, using Metal.
 *
 * To simplify instantiating MGLGLKView wherever you are currently instantiating GLKView,
 * in your source code, replace any imports of the <GLKit/GLKView.h> header file with the 
 * <MoltenGL/MGLGLKView.h> header file. Importing the <MoltenGL/MGLGLKView.h> header file will
 * redefine all subsequent references to GLKView in your source code to MGLGLKView, instead.
 *
 * If Metal is not available on the platform, or has not been activated via the glOpenGLImplMGL()
 * function, attempting to instantiate an instance of MGLGLKView will actually return an instance
 * of the original GLKView class, permitting normal OpenGL functionality.
 */
MLN_PUBLIC_SYMBOL @interface MGLGLKView : PlatformView


#pragma mark Framebuffer support

/** Replicates behaviour of GLKView. */
@property(nonatomic, assign) IBOutlet id <GLKViewDelegate> delegate;

/** Replicates behaviour of GLKView. */
@property(nonatomic, retain) EAGLContext *context;

/** Replicates behaviour of GLKView. */
@property(nonatomic, readonly) NSInteger drawableWidth;

/** Replicates behaviour of GLKView. */
@property(nonatomic, readonly) NSInteger drawableHeight;

/** Replicates behaviour of GLKView. */
@property(nonatomic) GLKViewDrawableColorFormat drawableColorFormat;

/** Replicates behaviour of GLKView. */
@property(nonatomic) GLKViewDrawableDepthFormat drawableDepthFormat;

/** Replicates behaviour of GLKView. */
@property(nonatomic) GLKViewDrawableStencilFormat drawableStencilFormat;

/** Replicates behaviour of GLKView. */
@property(nonatomic) GLKViewDrawableMultisample drawableMultisample;

/** Replicates behaviour of GLKView. */
-(void) bindDrawable;

/** Replicates behaviour of GLKView. */
-(void) deleteDrawable;


#pragma mark Drawing

/** Replicates behaviour of GLKView. */
@property(nonatomic) BOOL enableSetNeedsDisplay;

/** Replicates behaviour of GLKView. */
-(void) display;

/** Replicates behaviour of GLKView. */
-(PlatformImage*) snapshot;


#pragma mark Instantiation

/** Replicates behaviour of GLKView. */
-(instancetype) initWithFrame: (CGRect) frame context: (EAGLContext*) context;

@end
