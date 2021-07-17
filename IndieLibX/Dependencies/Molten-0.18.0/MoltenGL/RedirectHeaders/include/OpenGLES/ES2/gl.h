/*
 * gl.h
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


#ifdef __cplusplus
extern "C" {
#endif	//  __cplusplus
	
#import <MoltenGL/mglEnv.h>
#import "../../../orig/OpenGLES/ES2/gl.h"

	
#if MGL_SUPPORT_OPENGL_ON_METAL
	
#define			glActiveTexture    mglActiveTexture
void			mglActiveTexture(GLenum texture);

#define			glAttachShader    mglAttachShader
void			mglAttachShader(GLuint program, GLuint shader);

#define			glBindAttribLocation    mglBindAttribLocation
void			mglBindAttribLocation(GLuint program, GLuint index, const GLchar* name);

#define			glBindBuffer    mglBindBuffer
void			mglBindBuffer(GLenum target, GLuint buffer);

#define			glBindFramebuffer    mglBindFramebuffer
void			mglBindFramebuffer(GLenum target, GLuint framebuffer);

#define			glBindRenderbuffer    mglBindRenderbuffer
void			mglBindRenderbuffer(GLenum target, GLuint renderbuffer);

#define			glBindTexture    mglBindTexture
void			mglBindTexture(GLenum target, GLuint texture);

#define			glBlendColor    mglBlendColor
void			mglBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

#define			glBlendEquation    mglBlendEquation
void			mglBlendEquation(GLenum mode);

#define			glBlendEquationSeparate    mglBlendEquationSeparate
void			mglBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);

#define			glBlendFunc    mglBlendFunc
void			mglBlendFunc(GLenum sfactor, GLenum dfactor);

#define			glBlendFuncSeparate    mglBlendFuncSeparate
void			mglBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

#define			glBufferData    mglBufferData
void			mglBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

#define			glBufferSubData    mglBufferSubData
void			mglBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

#define			glCheckFramebufferStatus    mglCheckFramebufferStatus
GLenum			mglCheckFramebufferStatus(GLenum target);

#define			glClear    mglClear
void			mglClear(GLbitfield mask);

#define			glClearColor    mglClearColor
void			mglClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

#define			glClearDepthf    mglClearDepthf
void			mglClearDepthf(GLclampf depth);

#define			glClearStencil    mglClearStencil
void			mglClearStencil(GLint s);

#define			glColorMask    mglColorMask
void			mglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

#define			glCompileShader    mglCompileShader
void			mglCompileShader(GLuint shader);

#define			glCompressedTexImage2D    mglCompressedTexImage2D
void			mglCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);

#define			glCompressedTexSubImage2D    mglCompressedTexSubImage2D
void			mglCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data);

#define			glCopyTexImage2D    mglCopyTexImage2D
void			mglCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

#define			glCopyTexSubImage2D    mglCopyTexSubImage2D
void			mglCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

#define			glCreateProgram    mglCreateProgram
GLuint			mglCreateProgram(void);

#define			glCreateShader    mglCreateShader
GLuint			mglCreateShader(GLenum type);

#define			glCullFace    mglCullFace
void			mglCullFace(GLenum mode);

#define			glDeleteBuffers    mglDeleteBuffers
void			mglDeleteBuffers(GLsizei n, const GLuint* buffers);

#define			glDeleteFramebuffers    mglDeleteFramebuffers
void			mglDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);

#define			glDeleteProgram    mglDeleteProgram
void			mglDeleteProgram(GLuint program);

#define			glDeleteRenderbuffers    mglDeleteRenderbuffers
void			mglDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);

#define			glDeleteShader    mglDeleteShader
void			mglDeleteShader(GLuint shader);

#define			glDeleteTextures    mglDeleteTextures
void			mglDeleteTextures(GLsizei n, const GLuint* textures);

#define			glDepthFunc    mglDepthFunc
void			mglDepthFunc(GLenum func);

#define			glDepthMask    mglDepthMask
void			mglDepthMask(GLboolean flag);

#define			glDepthRangef    mglDepthRangef
void			mglDepthRangef(GLclampf zNear, GLclampf zFar);

#define			glDetachShader    mglDetachShader
void			mglDetachShader(GLuint program, GLuint shader);

#define			glDisable    mglDisable
void			mglDisable(GLenum cap);

#define			glDisableVertexAttribArray    mglDisableVertexAttribArray
void			mglDisableVertexAttribArray(GLuint index);

#define			glDrawArrays    mglDrawArrays
void			mglDrawArrays(GLenum mode, GLint first, GLsizei count);

#define			glDrawElements    mglDrawElements
void			mglDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

#define			glEnable    mglEnable
void			mglEnable(GLenum cap);

#define			glEnableVertexAttribArray    mglEnableVertexAttribArray
void			mglEnableVertexAttribArray(GLuint index);

#define			glFinish    mglFinish
void			mglFinish(void);

#define			glFlush    mglFlush
void			mglFlush(void);

#define			glFramebufferRenderbuffer    mglFramebufferRenderbuffer
void			mglFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

#define			glFramebufferTexture2D    mglFramebufferTexture2D
void			mglFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

#define			glFrontFace    mglFrontFace
void			mglFrontFace(GLenum mode);

#define			glGenBuffers    mglGenBuffers
void			mglGenBuffers(GLsizei n, GLuint* buffers);

#define			glGenerateMipmap    mglGenerateMipmap
void			mglGenerateMipmap(GLenum target);

#define			glGenFramebuffers    mglGenFramebuffers
void			mglGenFramebuffers(GLsizei n, GLuint* framebuffers);

#define			glGenRenderbuffers    mglGenRenderbuffers
void			mglGenRenderbuffers(GLsizei n, GLuint* renderbuffers);

#define			glGenTextures    mglGenTextures
void			mglGenTextures(GLsizei n, GLuint* textures);

#define			glGetActiveAttrib    mglGetActiveAttrib
void			mglGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);

#define			glGetActiveUniform    mglGetActiveUniform
void			mglGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);

#define			glGetAttachedShaders    mglGetAttachedShaders
void			mglGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);

#define			glGetAttribLocation    mglGetAttribLocation
int				mglGetAttribLocation(GLuint program, const GLchar* name);

#define			glGetBooleanv    mglGetBooleanv
void			mglGetBooleanv(GLenum pname, GLboolean* params);

#define			glGetBufferParameteriv    mglGetBufferParameteriv
void			mglGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);

#define			glGetError    mglGetError
GLenum			mglGetError(void);

#define			glGetFloatv    mglGetFloatv
void			mglGetFloatv(GLenum pname, GLfloat* params);

#define			glGetFramebufferAttachmentParameteriv    mglGetFramebufferAttachmentParameteriv
void			mglGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);

#define			glGetIntegerv    mglGetIntegerv
void			mglGetIntegerv(GLenum pname, GLint* params);

#define			glGetProgramiv    mglGetProgramiv
void			mglGetProgramiv(GLuint program, GLenum pname, GLint* params);

#define			glGetProgramInfoLog    mglGetProgramInfoLog
void			mglGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);

#define			glGetRenderbufferParameteriv    mglGetRenderbufferParameteriv
void			mglGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);

#define			glGetShaderiv    mglGetShaderiv
void			mglGetShaderiv(GLuint shader, GLenum pname, GLint* params);

#define			glGetShaderInfoLog    mglGetShaderInfoLog
void			mglGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);

#define			glGetShaderPrecisionFormat    mglGetShaderPrecisionFormat
void			mglGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);

#define			glGetShaderSource    mglGetShaderSource
void			mglGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);

#define			glGetString    mglGetString
const GLubyte*	mglGetString(GLenum name);

#define			glGetTexParameterfv    mglGetTexParameterfv
void			mglGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params);

#define			glGetTexParameteriv    mglGetTexParameteriv
void			mglGetTexParameteriv(GLenum target, GLenum pname, GLint* params);

#define			glGetUniformfv    mglGetUniformfv
void			mglGetUniformfv(GLuint program, GLint location, GLfloat* params);

#define			glGetUniformiv    mglGetUniformiv
void			mglGetUniformiv(GLuint program, GLint location, GLint* params);

#define			glGetUniformLocation    mglGetUniformLocation
int				mglGetUniformLocation(GLuint program, const GLchar* name);

#define			glGetVertexAttribfv    mglGetVertexAttribfv
void			mglGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);

#define			glGetVertexAttribiv    mglGetVertexAttribiv
void			mglGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);

#define			glGetVertexAttribPointerv    mglGetVertexAttribPointerv
void			mglGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer);

#define			glHint    mglHint
void			mglHint(GLenum target, GLenum mode);

#define			glIsBuffer    mglIsBuffer
GLboolean		mglIsBuffer(GLuint buffer);

#define			glIsEnabled    mglIsEnabled
GLboolean		mglIsEnabled(GLenum cap);

#define			glIsFramebuffer    mglIsFramebuffer
GLboolean		mglIsFramebuffer(GLuint framebuffer);

#define			glIsProgram    mglIsProgram
GLboolean		mglIsProgram(GLuint program);

#define			glIsRenderbuffer    mglIsRenderbuffer
GLboolean		mglIsRenderbuffer(GLuint renderbuffer);
	
#define			glIsShader    mglIsShader
GLboolean		mglIsShader(GLuint shader);
	
#define			glIsTexture    mglIsTexture
GLboolean		mglIsTexture(GLuint texture);

#define			glLineWidth    mglLineWidth
void			mglLineWidth(GLfloat width);

#define			glLinkProgram    mglLinkProgram
void			mglLinkProgram(GLuint program);

#define			glPixelStorei    mglPixelStorei
void			mglPixelStorei(GLenum pname, GLint param);

#define			glPolygonOffset    mglPolygonOffset
void			mglPolygonOffset(GLfloat factor, GLfloat units);

#define			glReadPixels    mglReadPixels
void			mglReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);

#define			glReleaseShaderCompiler    mglReleaseShaderCompiler
void			mglReleaseShaderCompiler(void);

#define			glRenderbufferStorage    mglRenderbufferStorage
void			mglRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

#define			glSampleCoverage    mglSampleCoverage
void			mglSampleCoverage(GLclampf value, GLboolean invert);

#define			glScissor    mglScissor
void			mglScissor(GLint x, GLint y, GLsizei width, GLsizei height);

#define			glShaderBinary    mglShaderBinary
void			mglShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);

#define			glShaderSource    mglShaderSource
void			mglShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);

#define			glStencilFunc    mglStencilFunc
void			mglStencilFunc(GLenum func, GLint ref, GLuint mask);

#define			glStencilFuncSeparate    mglStencilFuncSeparate
void			mglStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);

#define			glStencilMask    mglStencilMask
void			mglStencilMask(GLuint mask);

#define			glStencilMaskSeparate    mglStencilMaskSeparate
void			mglStencilMaskSeparate(GLenum face, GLuint mask);

#define			glStencilOp    mglStencilOp
void			mglStencilOp(GLenum fail, GLenum zfail, GLenum zpass);

#define			glStencilOpSeparate    mglStencilOpSeparate
void			mglStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);

#define			glTexImage2D    mglTexImage2D
void			mglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);

#define			glTexParameterf    mglTexParameterf
void			mglTexParameterf(GLenum target, GLenum pname, GLfloat param);

#define			glTexParameterfv    mglTexParameterfv
void			mglTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);

#define			glTexParameteri    mglTexParameteri
void			mglTexParameteri(GLenum target, GLenum pname, GLint param);

#define			glTexParameteriv    mglTexParameteriv
void			mglTexParameteriv(GLenum target, GLenum pname, const GLint* params);

#define			glTexSubImage2D    mglTexSubImage2D
void			mglTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);

#define			glUniform1f    mglUniform1f
void			mglUniform1f(GLint location, GLfloat x);

#define			glUniform1fv    mglUniform1fv
void			mglUniform1fv(GLint location, GLsizei count, const GLfloat* v);

#define			glUniform1i    mglUniform1i
void			mglUniform1i(GLint location, GLint x);

#define			glUniform1iv    mglUniform1iv
void			mglUniform1iv(GLint location, GLsizei count, const GLint* v);

#define			glUniform2f    mglUniform2f
void			mglUniform2f(GLint location, GLfloat x, GLfloat y);

#define			glUniform2fv    mglUniform2fv
void			mglUniform2fv(GLint location, GLsizei count, const GLfloat* v);

#define			glUniform2i    mglUniform2i
void			mglUniform2i(GLint location, GLint x, GLint y);

#define			glUniform2iv    mglUniform2iv
void			mglUniform2iv(GLint location, GLsizei count, const GLint* v);

#define			glUniform3f    mglUniform3f
void			mglUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);

#define			glUniform3fv    mglUniform3fv
void			mglUniform3fv(GLint location, GLsizei count, const GLfloat* v);

#define			glUniform3i    mglUniform3i
void			mglUniform3i(GLint location, GLint x, GLint y, GLint z);

#define			glUniform3iv    mglUniform3iv
void			mglUniform3iv(GLint location, GLsizei count, const GLint* v);

#define			glUniform4f    mglUniform4f
void			mglUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

#define			glUniform4fv    mglUniform4fv
void			mglUniform4fv(GLint location, GLsizei count, const GLfloat* v);

#define			glUniform4i    mglUniform4i
void			mglUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);

#define			glUniform4iv    mglUniform4iv
void			mglUniform4iv(GLint location, GLsizei count, const GLint* v);

#define			glUniformMatrix2fv    mglUniformMatrix2fv
void			mglUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define			glUniformMatrix3fv    mglUniformMatrix3fv
void			mglUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define			glUniformMatrix4fv    mglUniformMatrix4fv
void			mglUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define			glUseProgram    mglUseProgram
void			mglUseProgram(GLuint program);

#define			glValidateProgram    mglValidateProgram
void			mglValidateProgram(GLuint program);

#define			glVertexAttrib1f    mglVertexAttrib1f
void			mglVertexAttrib1f(GLuint indx, GLfloat x);

#define			glVertexAttrib1fv    mglVertexAttrib1fv
void			mglVertexAttrib1fv(GLuint indx, const GLfloat* values);

#define			glVertexAttrib2f    mglVertexAttrib2f
void			mglVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);

#define			glVertexAttrib2fv    mglVertexAttrib2fv
void			mglVertexAttrib2fv(GLuint indx, const GLfloat* values);

#define			glVertexAttrib3f    mglVertexAttrib3f
void			mglVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);

#define			glVertexAttrib3fv    mglVertexAttrib3fv
void			mglVertexAttrib3fv(GLuint indx, const GLfloat* values);

#define			glVertexAttrib4f    mglVertexAttrib4f
void			mglVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

#define			glVertexAttrib4fv    mglVertexAttrib4fv
void			mglVertexAttrib4fv(GLuint indx, const GLfloat* values);

#define			glVertexAttribPointer    mglVertexAttribPointer
void			mglVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);

#define			glViewport    mglViewport
void			mglViewport(GLint x, GLint y, GLsizei width, GLsizei height);


#endif	// MGL_SUPPORT_OPENGL_ON_METAL

#ifdef __cplusplus
}
#endif	//  __cplusplus
