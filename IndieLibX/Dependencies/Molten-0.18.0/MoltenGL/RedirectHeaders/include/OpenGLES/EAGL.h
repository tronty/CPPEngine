/*
 * EAGL.h
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
#import "../../orig/OpenGLES/EAGL.h"


#pragma mark -
#pragma mark MGLEAGLSharegroup

/** In any files that import this header file, redefine all references to EAGLSharegroup as MGLEAGLSharegroup instead. */
#if MGL_SUPPORT_OPENGL_ON_METAL
#	define EAGLSharegroup	MGLEAGLSharegroup
#endif

/**
 * MGLEAGLSharegroup replicates the functionality of EAGLSharegroup, using Metal.
 *
 * To simplify instantiating MGLEAGLSharegroup wherever you are currently instantiating EAGLSharegroup,
 * in your source code, replace any imports of the <OpenGLES/EAGL.h> header file with the <MoltenGL/EAGL.h>
 * header file. Importing the <MoltenGL/EAGL.h> header file will redefine all subsequent references to 
 * EAGLSharegroup in your source code to MGLEAGLSharegroup, instead.
 *
 * If Metal is not available on the platform, or has not been activated via the glOpenGLImplMGL()
 * function, attempting to instantiate an instance of MGLEAGLSharegroup will actually return an instance
 * of the original EAGLSharegroup class, permitting normal OpenGL functionality.
 */
MLN_PUBLIC_SYMBOL @interface MGLEAGLSharegroup : NSObject

/** Replicates behaviour of EAGLSharegroup. */
@property(copy, nonatomic) NSString* debugLabel;

@end


#pragma mark -
#pragma mark MGLEAGLContext

/** In any files that import this header file, redefine all references to EAGLContext as MGLEAGLContext instead. */
#if MGL_SUPPORT_OPENGL_ON_METAL
#	define EAGLContext MGLEAGLContext
#endif

/**
 * MGLEAGLContext replicates the functionality of EAGLContext, using Metal.
 *
 * To simplify instantiating MGLEAGLContext wherever you are currently instantiating EAGLContext,
 * in your source code, replace any imports of the <OpenGLES/EAGL.h> header file with the 
 * <MoltenGL/EAGL.h> header file. Importing the <MoltenGL/EAGL.h> header file will redefine all 
 * subsequent references to EAGLContext in your source code to MGLEAGLContext, instead.
 *
 * If Metal is not available on the platform, or has not been activated via the glOpenGLImplMGL()
 * function, attempting to instantiate an instance of MGLEAGLContext will actually return an instance
 * of the original EAGLContext class, permitting normal OpenGL functionality.
 */
MLN_PUBLIC_SYMBOL @interface MGLEAGLContext : NSObject

/** Replicates behaviour of EAGLContext. */
@property (readonly) EAGLRenderingAPI API;

/** Replicates behaviour of EAGLContext. */
@property (readonly) EAGLSharegroup* sharegroup;

/** Replicates behaviour of EAGLContext. */
@property (copy, nonatomic) NSString* debugLabel;

/** Replicates behaviour of EAGLContext. */
@property (getter=isMultiThreaded, nonatomic) BOOL multiThreaded;


#pragma mark Instantiation

/** 
 * Replicates behaviour of EAGLContext.
 *
 * If Metal is not active, this will return an instance of EAGLContext.
 */
-(instancetype) initWithAPI: (EAGLRenderingAPI) api;

/**
 * Replicates behaviour of EAGLContext.
 *
 * If Metal is not active, this will return an instance of EAGLContext.
 */
-(instancetype) initWithAPI: (EAGLRenderingAPI) api sharegroup: (EAGLSharegroup*) sharegroup;


#pragma mark Current context

/** Replicates behaviour of EAGLContext. */
+(EAGLContext*) currentContext;

/** Replicates behaviour of EAGLContext. */
+(BOOL) setCurrentContext: (EAGLContext*) context;

@end
