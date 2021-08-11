/*
 * glext.h
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
#import "../../../orig/OpenGLES/ES2/glext.h"


#pragma mark Additional OpenGL constants supported by MoltenGL

	// Mapped buffers under Metal are readable and writable
#define GL_READ_ONLY_ARB                                     0x88B8
#define GL_WRITE_ONLY_ARB                                    0x88B9
#define GL_READ_WRITE_ARB                                    0x88BA


#if MGL_SUPPORT_OPENGL_ON_METAL

#pragma mark Extension functions

#define		glCopyTextureLevelsAPPLE    mglCopyTextureLevelsAPPLE
GLvoid		mglCopyTextureLevelsAPPLE(GLuint destinationTexture, GLuint sourceTexture, GLint sourceBaseLevel, GLsizei sourceLevelCount);

#define		glRenderbufferStorageMultisampleAPPLE    mglRenderbufferStorageMultisampleAPPLE
GLvoid		mglRenderbufferStorageMultisampleAPPLE(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

#define		glResolveMultisampleFramebufferAPPLE    mglResolveMultisampleFramebufferAPPLE
GLvoid		mglResolveMultisampleFramebufferAPPLE(void);

#define		glFenceSyncAPPLE    mglFenceSyncAPPLE
GLsync		mglFenceSyncAPPLE(GLenum condition, GLbitfield flags);

#define		glIsSyncAPPLE    mglIsSyncAPPLE
GLboolean	mglIsSyncAPPLE(GLsync sync);

#define		glDeleteSyncAPPLE    mglDeleteSyncAPPLE
void		mglDeleteSyncAPPLE(GLsync sync);

#define		glClientWaitSyncAPPLE    mglClientWaitSyncAPPLE
GLenum		mglClientWaitSyncAPPLE(GLsync sync, GLbitfield flags, GLuint64 timeout);

#define		glWaitSyncAPPLE    mglWaitSyncAPPLE
void		mglWaitSyncAPPLE(GLsync sync, GLbitfield flags, GLuint64 timeout);

#define		glGetInteger64vAPPLE    mglGetInteger64vAPPLE
void		mglGetInteger64vAPPLE(GLenum pname, GLint64* params);

#define		glGetSyncivAPPLE    mglGetSyncivAPPLE
void		mglGetSyncivAPPLE(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values);

#define		glLabelObjectEXT    mglLabelObjectEXT
GLvoid		mglLabelObjectEXT(GLenum type, GLuint object, GLsizei length, const GLchar* label);

#define		glGetObjectLabelEXT    mglGetObjectLabelEXT
GLvoid		mglGetObjectLabelEXT(GLenum type, GLuint object, GLsizei bufSize, GLsizei* length, GLchar* label);

#define		glInsertEventMarkerEXT    mglInsertEventMarkerEXT
GLvoid		mglInsertEventMarkerEXT(GLsizei length, const GLchar* marker);

#define		glPushGroupMarkerEXT    mglPushGroupMarkerEXT
GLvoid		mglPushGroupMarkerEXT(GLsizei length, const GLchar* marker);

#define		glPopGroupMarkerEXT    mglPopGroupMarkerEXT
GLvoid		mglPopGroupMarkerEXT(void);

#define		glDiscardFramebufferEXT    mglDiscardFramebufferEXT
GLvoid 		mglDiscardFramebufferEXT(GLenum target, GLsizei numAttachments, const GLenum* attachments);

#define		glDrawArraysInstancedEXT    mglDrawArraysInstancedEXT
GLvoid		mglDrawArraysInstancedEXT(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);

#define		glDrawElementsInstancedEXT    mglDrawElementsInstancedEXT
GLvoid		mglDrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount);

#define		glVertexAttribDivisorEXT    mglVertexAttribDivisorEXT
GLvoid		mglVertexAttribDivisorEXT(GLuint index, GLuint divisor);

#define		glMapBufferRangeEXT    mglMapBufferRangeEXT
GLvoid*		mglMapBufferRangeEXT(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

#define		glFlushMappedBufferRangeEXT    mglFlushMappedBufferRangeEXT
GLvoid		mglFlushMappedBufferRangeEXT(GLenum target, GLintptr offset, GLsizeiptr length);

#define		glGenQueriesEXT    mglGenQueriesEXT
GLvoid		mglGenQueriesEXT(GLsizei n, GLuint* ids);

#define		glDeleteQueriesEXT    mglDeleteQueriesEXT
GLvoid		mglDeleteQueriesEXT(GLsizei n, const GLuint* ids);

#define		glIsQueryEXT    mglIsQueryEXT
GLboolean	mglIsQueryEXT(GLuint id);

#define		glBeginQueryEXT    mglBeginQueryEXT
GLvoid		mglBeginQueryEXT(GLenum target, GLuint id);

#define		glEndQueryEXT    mglEndQueryEXT
GLvoid		mglEndQueryEXT(GLenum target);

#define		glGetQueryivEXT    mglGetQueryivEXT
GLvoid		mglGetQueryivEXT(GLenum target, GLenum pname, GLint* params);

#define		glGetQueryObjectuivEXT    mglGetQueryObjectuivEXT
GLvoid		mglGetQueryObjectuivEXT(GLuint id, GLenum pname, GLuint* params);

#define		glUseProgramStagesEXT    mglUseProgramStagesEXT
GLvoid		mglUseProgramStagesEXT(GLuint pipeline, GLbitfield stages, GLuint program);

#define		glActiveShaderProgramEXT    mglActiveShaderProgramEXT
GLvoid		mglActiveShaderProgramEXT(GLuint pipeline, GLuint program);

#define		glCreateShaderProgramvEXT    mglCreateShaderProgramvEXT
GLuint		mglCreateShaderProgramvEXT(GLenum type, GLsizei count, const GLchar* const* strings);

#define		glBindProgramPipelineEXT    mglBindProgramPipelineEXT
GLvoid		mglBindProgramPipelineEXT(GLuint pipeline);

#define		glDeleteProgramPipelinesEXT    mglDeleteProgramPipelinesEXT
GLvoid		mglDeleteProgramPipelinesEXT(GLsizei n, const GLuint* pipelines);

#define		glGenProgramPipelinesEXT    mglGenProgramPipelinesEXT
GLvoid		mglGenProgramPipelinesEXT(GLsizei n, GLuint* pipelines);

#define		glIsProgramPipelineEXT    mglIsProgramPipelineEXT
GLboolean	mglIsProgramPipelineEXT(GLuint pipeline);

#define		glProgramParameteriEXT    mglProgramParameteriEXT
GLvoid		mglProgramParameteriEXT(GLuint program, GLenum pname, GLint value);

#define		glGetProgramPipelineivEXT    mglGetProgramPipelineivEXT
GLvoid		mglGetProgramPipelineivEXT(GLuint pipeline, GLenum pname, GLint* params);

#define		glValidateProgramPipelineEXT    mglValidateProgramPipelineEXT
GLvoid		mglValidateProgramPipelineEXT(GLuint pipeline);

#define		glGetProgramPipelineInfoLogEXT    mglGetProgramPipelineInfoLogEXT
GLvoid		mglGetProgramPipelineInfoLogEXT(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

#define		glProgramUniform1iEXT    mglProgramUniform1iEXT
GLvoid		mglProgramUniform1iEXT(GLuint program, GLint location, GLint x);

#define		glProgramUniform2iEXT    mglProgramUniform2iEXT
GLvoid		mglProgramUniform2iEXT(GLuint program, GLint location, GLint x, GLint y);

#define		glProgramUniform3iEXT    mglProgramUniform3iEXT
GLvoid		mglProgramUniform3iEXT(GLuint program, GLint location, GLint x, GLint y, GLint z);

#define		glProgramUniform4iEXT    mglProgramUniform4iEXT
GLvoid		mglProgramUniform4iEXT(GLuint program, GLint location, GLint x, GLint y, GLint z, GLint w);

#define		glProgramUniform1fEXT    mglProgramUniform1fEXT
GLvoid		mglProgramUniform1fEXT(GLuint program, GLint location, GLfloat x);

#define		glProgramUniform2fEXT    mglProgramUniform2fEXT
GLvoid		mglProgramUniform2fEXT(GLuint program, GLint location, GLfloat x, GLfloat y);

#define		glProgramUniform3fEXT    mglProgramUniform3fEXT
GLvoid		mglProgramUniform3fEXT(GLuint program, GLint location, GLfloat x, GLfloat y, GLfloat z);

#define		glProgramUniform4fEXT    mglProgramUniform4fEXT
GLvoid		mglProgramUniform4fEXT(GLuint program, GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

#define		glProgramUniform1ivEXT    mglProgramUniform1ivEXT
GLvoid		mglProgramUniform1ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value);

#define		glProgramUniform2ivEXT    mglProgramUniform2ivEXT
GLvoid		mglProgramUniform2ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value);

#define		glProgramUniform3ivEXT    mglProgramUniform3ivEXT
GLvoid		mglProgramUniform3ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value);

#define		glProgramUniform4ivEXT    mglProgramUniform4ivEXT
GLvoid		mglProgramUniform4ivEXT(GLuint program, GLint location, GLsizei count, const GLint* value);

#define		glProgramUniform1fvEXT    mglProgramUniform1fvEXT
GLvoid		mglProgramUniform1fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value);

#define		glProgramUniform2fvEXT    mglProgramUniform2fvEXT
GLvoid		mglProgramUniform2fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value);

#define		glProgramUniform3fvEXT    mglProgramUniform3fvEXT
GLvoid		mglProgramUniform3fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value);

#define		glProgramUniform4fvEXT    mglProgramUniform4fvEXT
GLvoid		mglProgramUniform4fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat* value);

#define		glProgramUniformMatrix2fvEXT    mglProgramUniformMatrix2fvEXT
GLvoid		mglProgramUniformMatrix2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define		glProgramUniformMatrix3fvEXT    mglProgramUniformMatrix3fvEXT
GLvoid		mglProgramUniformMatrix3fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define		glProgramUniformMatrix4fvEXT    mglProgramUniformMatrix4fvEXT
GLvoid		mglProgramUniformMatrix4fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

#define		glGetBufferPointervOES    mglGetBufferPointervOES
GLvoid 		mglGetBufferPointervOES (GLenum target, GLenum pname, GLvoid** params);

#define		glMapBufferOES    mglMapBufferOES
GLvoid* 	mglMapBufferOES (GLenum target, GLenum access);

#define		glUnmapBufferOES    mglUnmapBufferOES
GLboolean 	mglUnmapBufferOES (GLenum target);

#define		glBindVertexArrayOES    mglBindVertexArrayOES
GLvoid		mglBindVertexArrayOES(GLuint array);

#define		glDeleteVertexArraysOES    mglDeleteVertexArraysOES
GLvoid		mglDeleteVertexArraysOES(GLsizei n, const GLuint* arrays);

#define		glGenVertexArraysOES    mglGenVertexArraysOES
GLvoid		mglGenVertexArraysOES(GLsizei n, GLuint* arrays);

#define		glIsVertexArrayOES    mglIsVertexArrayOES
GLboolean	mglIsVertexArrayOES(GLuint array);

	
#endif	// MGL_SUPPORT_OPENGL_ON_METAL
	
#ifdef __cplusplus
}
#endif	//  __cplusplus
