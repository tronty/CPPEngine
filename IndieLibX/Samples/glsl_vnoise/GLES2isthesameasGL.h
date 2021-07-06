/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#pragma once

#if defined(USE_GLES_SHADERS) // ???
// if defined(GLES3_2) || defined(GLES2)

// ??? GL_POINT_SPRITE_OES

    // Core GLES
    #define glClearDepth 			glClearDepthf

    // OES_vertex_array_object
    #define glGenVertexArrays 			glGenVertexArraysOES
    #define glBindVertexArray 			glBindVertexArrayOES
    #define glDeleteVertexArrays 		glDeleteVertexArraysOES

    // OES_mapbuffer
    #define glMapBuffer 			glMapBufferOES
    #define glUnmapBuffer 			glUnmapBufferOES
    #define GL_WRITE_ONLY 			GL_WRITE_ONLY_OES

    // EXT_map_buffer_range
    #define glMapBufferRange 			glMapBufferRangeEXT
    #define GL_MAP_WRITE_BIT 			GL_MAP_WRITE_BIT_EXT
    #define GL_MAP_INVALIDATE_BUFFER_BIT 	GL_MAP_INVALIDATE_BUFFER_BIT_EXT

    // EXT_texture_storage
    #define glTexStorage2D 			glTexStorage2DEXT
    #define GL_LUMINANCE8 			GL_LUMINANCE8_EXT
    #define GL_LUMINANCE8_ALPHA8 		GL_LUMINANCE8_ALPHA8_EXT

    // GL_OES_packed_depth_stencil
#if !defined(__APPLE__)
    #define GL_DEPTH_STENCIL 			GL_DEPTH_STENCIL_OES
    #define GL_UNSIGNED_INT_24_8 		GL_UNSIGNED_INT_24_8_OES
#else
    #define GL_DEPTH_STENCIL 			0
    #define GL_UNSIGNED_INT_24_8 		0
#endif
#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
    #define GL_DEPTH24_STENCIL8 		GL_DEPTH24_STENCIL8_OES
    #define GL_TEXTURE_3D 			GL_TEXTURE_3D_OES
    #define GL_TEXTURE_WRAP_R 			GL_TEXTURE_WRAP_R_OES
    #define GL_DEPTH_COMPONENT24 		GL_DEPTH_COMPONENT24_OES
#else
    #define GL_DEPTH24_STENCIL8 		0
    #define GL_TEXTURE_3D 			0
    #define GL_TEXTURE_WRAP_R 			0
    #define GL_DEPTH_COMPONENT24 		0
#endif

    // OES_rgb8_rgba8
    #define GL_RGBA8 				GL_RGBA8_OES

    // OES_texture_half_float
    #define GL_HALF_FLOAT 			GL_HALF_FLOAT_OES

    // EXT_color_buffer_half_float
    #define GL_RGBA16F 				GL_RGBA16F_EXT

    // EXT_texture_compression_s3tc
    #define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 	0
    #define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 	0
    #define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 	0

    // 3D textures: not supported
    #define GL_TEXTURE_3D 			0
    #define glTexSubImage3D 			(void)sizeof
    #define glTexStorage3D 			(void)sizeof
#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
    #define glCompressedTexImage3D 		glCompressedTexImage3DOES
#else
    #define glCompressedTexImage3D 		(void)sizeof
    #define glTexImage3D 			(void)sizeof
#endif

    // RG format: not supported
    #define GL_RG 				0
    #define GL_RG16 				0
#elif 0
//defined(GLSL1_1)
/*
    #define GL_FRAGMENT_SHADER_ARB			GL_FRAGMENT_SHADER
    #define GL_OBJECT_ACTIVE_UNIFORMS_ARB		GL_OBJECT_ACTIVE_UNIFORMS
    #define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB	GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH
    #define GL_OBJECT_COMPILE_STATUS_ARB 		GL_OBJECT_COMPILE_STATUS
    #define GL_OBJECT_LINK_STATUS_ARB			GL_OBJECT_LINK_STATUS
    #define GL_SHADING_LANGUAGE_VERSION_ARB		GL_SHADING_LANGUAGE_VERSION
    #define GL_VERTEX_SHADER_ARB 			GL_VERTEX_SHADER
*/
    #define GLcharARB 					GLchar
    #define glAttachObjectARB				glAttachObject
    #define glBindAttribLocationARB			glBindAttribLocation
    #define glCompileShaderARB				glCompileShader
    #define glGetInfoLogARB				glGetInfoLog
    #define glGetObjectParameterivARB			glGetObjectParameteriv
    #define glCreateProgramObjectARB			glCreateProgramObject
    #define glCreateShaderObjectARB			glCreateShaderObject
    #define glLinkProgramARB				glLinkProgram
    #define glShaderSourceARB				glShaderSource
#elif 0
    // IMG_texture_compression_pvrtc
    #define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 		0
    #define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 	0
    #define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 		0
    #define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 	0
#endif

