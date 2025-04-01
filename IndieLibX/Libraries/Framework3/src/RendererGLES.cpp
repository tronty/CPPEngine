/* * * * * * * * * * * * * Author's note * * * * * * * * * * * **
*  |_|	   |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|	*
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _	*
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_	*
*  |_|	   |_| |_|_ _ _|_| |_|	   |_| |_|_ _ _|_|  _ _ _ _|_|	*
*  |_|	   |_|	 |_|_|_|   |_|	   |_|	 |_|_|_|   |_|_|_|_|	*
*								*
*		      http://www.humus.name			*
*								 *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.		   *
*								 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/RendererGLES.h>
#ifdef __RendererGLES_H__

#if defined(GLES3_2)
#define glCompressedTexImage3DOES
#define glMapBufferOES
#define glUnmapBufferOES
#define GL_WRITE_ONLY_OES 0x0000
#define GL_MIN_EXT 0x0000
#define GL_MAX_EXT 0x0000
#define GL_TEXTURE_3D_OES 0x0000
#define GL_TEXTURE_WRAP_R_OES 0x0000
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x0000
#define GL_DEPTH_COMPONENT24_OES 0x0000
#define GL_DEPTH24_STENCIL8_OES 0x0000
#define GL_UNSIGNED_INT_24_8_OES 0x0000
#define GL_DEPTH_STENCIL_OES 0x0000
#endif

#define LOG_FNLN_X
#define LOG_PRINT_X

		void RendererGLES::SwapBuffersGL(void)
		{
			LOG_PRINT_X("SDL_GL_SwapBuffers:\n");
#if defined(SDL_2_0_5_BUILD)// && !defined(ANDROID)
	//SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
	SDL_GL_SwapWindow(STX_Service::GetSDLWindow());
#elif defined(SDL_1_2_15_BUILD)
	SDL_GL_SwapBuffers();
#elif defined(_MSC_VER) && !defined(WIN_PHONE_APP)
	SwapBuffers( hDc );
#elif !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR) && !defined(WIN_PHONE_APP)
	if(!m_dpy) m_dpy=STX_Service::GetWindowInstance()->m_dpy;
	if(!m_win) m_win=STX_Service::GetWindowInstance()->m_win;
	//LOG_PRINT_X("m_dpy=%d\n", m_dpy);
	//LOG_PRINT_X("m_win=%d\n", m_win);
	glXSwapBuffers( m_dpy, m_win );
#endif
		}

#define BUFSIZE 80
#ifndef GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif

		BYTE* IndexBufferGL::Lock()
	{
		BYTE* m_pLockedData=0;
#if 1//defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			glBindBuffer(GL_ARRAY_BUFFER, vboIB);
   checkGlError("");

			LOG_PRINT_X("glBufferData:\n");
			glBufferData(GL_ARRAY_BUFFER,
				nIndices*indexSize,//???
				0, GL_DYNAMIC_DRAW);
   checkGlError("");
			LOG_PRINT_X("glMapBuffer:\n");
			m_pLockedData = (BYTE *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
   checkGlError("");
		#endif
		return m_pLockedData;
	}
	void IndexBufferGL::Unlock()
	{
 // Unlock
#if 1//!defined(SDL_1_2_15_BUILD)
        glUnmapBuffer(GL_ARRAY_BUFFER);
   checkGlError("");
#endif
	}
	BYTE* VertexBufferGL::Lock()
	{
		BYTE* m_pLockedData=0;
#if 1//defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			glBindBuffer(GL_ARRAY_BUFFER, vboVB);
   checkGlError("");
			LOG_PRINT_X("glBufferData:\n");
			glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
   checkGlError("");
			LOG_PRINT_X("glMapBuffer:\n");
			m_pLockedData = (BYTE *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
   checkGlError("");
#endif
		return m_pLockedData;
	}
	void VertexBufferGL::Unlock()
	{
// Unlock
#if 1//!defined(SDL_1_2_15_BUILD)
		LOG_PRINT_X("glUnmapBuffer:\n");
		glUnmapBuffer(GL_ARRAY_BUFFER);
   checkGlError("");
#endif
    }
	BYTE* TextureGL::Lock(int m, int& pitch)//int& slicePitch;
	{


//		int slicePitch;
		Image3 img;
		img.create(format,width, height,depth,mipMapLevels);

BYTE* m_pLockedData= 0;

#if !defined( _MSC_VER) && !defined(__APPLE__) && !defined(_LINUX) && !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
		LOG_PRINT_X("glBindBuffer:\n");
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, glTexID);
   checkGlError("");
		LOG_PRINT_X("glBufferData:\n");
		glBufferData(GL_PIXEL_UNPACK_BUFFER_EXT,
				img.getMipMappedSize(0, mipMapLevels),//???,
				0, GL_DYNAMIC_DRAW);
   checkGlError("");
		LOG_PRINT_X("glMapBuffer:\n");
		m_pLockedData= ((BYTE *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY))+(img.getPixels(m)-img.getPixels(0));
   checkGlError("");//img->getDataOffset(m,format);
#endif
		pitch=img.getPitch(m);
		return m_pLockedData;
	}
	void TextureGL::Unlock(int m)
	{
		#if !defined( _MSC_VER) && !defined(__APPLE__) && !defined(_LINUX) && !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			LOG_PRINT_X("glUnmapBuffer:\n");
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT);
   checkGlError("");
//glTexImage2D    (GLenum target, GLint level, GLint internalformat,         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
//glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,GLenum format, GLenum type, const GLvoid *pixels);
//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
//glTexImage2D((textures[renderTarget]).glTarget, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
//glTexSubImage2D(GL_TEXTURE_2D, tex.glTarget,i,0,0,tex.width, tex.height,srcFormat, srcType ,0);
#endif
	}


	RendererGLES::RendererGLES(
			bool fullScreen
			,int argc, char *argv[]
		): RendererHLSLCg(argc, argv)
{
{
	const char *version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (version)
	{
		SHADING_LANGUAGE_VERSION_MAJOR=atoi(version);
		SHADING_LANGUAGE_VERSION_MINOR=atoi(stx_strchr(version, '.') + 1);
	}
}

	// GL cannot use palettes anyway, so convert early.
	//ilEnable (IL_CONV_PAL);

	// Gets rid of dithering on some nVidia-based cards.
	//???ilutEnable (ILUT_OPENGL_CONV);
}

void RendererGLES::setGLdefaults()
{
	ZERO		     = GL_ZERO;
	ONE		     = GL_ONE;
	SRC_COLOR	     = GL_SRC_COLOR;
	ONE_MINUS_SRC_COLOR  = GL_ONE_MINUS_SRC_COLOR;
	DST_COLOR	     = GL_DST_COLOR;
	ONE_MINUS_DST_COLOR  = GL_ONE_MINUS_DST_COLOR;
	SRC_ALPHA	     = GL_SRC_ALPHA;
	ONE_MINUS_SRC_ALPHA  = GL_ONE_MINUS_SRC_ALPHA;
	DST_ALPHA	     = GL_DST_ALPHA;
	ONE_MINUS_DST_ALPHA  = GL_ONE_MINUS_DST_ALPHA;
	SRC_ALPHA_SATURATE   = GL_SRC_ALPHA_SATURATE;
	BM_ADD		    = GL_FUNC_ADD;
	BM_SUBTRACT	    = GL_FUNC_SUBTRACT;
	BM_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT;
#if !defined(_MSC_VER)
	BM_MIN		    = GL_MIN_EXT;
	BM_MAX		    = GL_MAX_EXT;
#endif
	NEVER	 = GL_NEVER;
	LESS	 = GL_LESS;
	EQUAL	 = GL_EQUAL;
	LEQUAL	 = GL_LEQUAL;
	GREATER  = GL_GREATER;
	NOTEQUAL = GL_NOTEQUAL;
	GEQUAL	 = GL_GEQUAL;
	ALWAYS	 = GL_ALWAYS;
	KEEP	 = GL_KEEP;
	SET_ZERO = GL_ZERO;
	REPLACE  = GL_REPLACE;
	INVERT	 = GL_INVERT;
	INCR	 = GL_INCR_WRAP;
	DECR	 = GL_DECR_WRAP;
	INCR_SAT = GL_INCR;
	DECR_SAT = GL_DECR;
	CULL_NONE  = 0;
	CULL_BACK  = GL_BACK;
	CULL_FRONT = GL_FRONT;
	SOLID = 0;//???GL_FILL;
	WIREFRAME = 0;//???GL_LINE;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glPixelStorei(GL_PACK_ALIGNMENT,   1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
/*
	SetVersion((const char *) glGetString(GL_VERSION));
	SetVendor((const char *) glGetString(GL_VENDOR));
	SetRenderer((const char *) glGetString(GL_RENDERER));
	SetAntialiasing(false);
	SetMaxTextureSize(4096);
	SetVertexShaderVersion(3);
	SetPixelShaderVersion(3);
*/
}

void RendererGLES::setup2DMode(const float left, const float right, const float top, const float bottom){
/*
	glMatrixMode(GL_PROJECTION);
				checkGlError("");
	glLoadIdentity();
				checkGlError("");
	glOrtho(left, right, bottom, top, -1, 1);
				checkGlError("");

	glMatrixMode(GL_MODELVIEW);
				checkGlError("");
	glLoadIdentity();
				checkGlError("");
*/
	scaleBias2D.x = 2.0f / (right - left);
	scaleBias2D.y = 2.0f / (top - bottom);
	scaleBias2D.z = -1.0f;
	scaleBias2D.w =  1.0f;

}
const GLenum glPrim_GLES[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	0,//???GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,


	GL_LINE_LOOP,
	GL_POINTS,
}
;
bool RendererGLES::supportsR2VB()
{
	return false;
}

bool RendererGLES::supportsHalf()
{
	return bsupportsHalf;
}
bool RendererGLES::supportsPS3()
{
	return false;
}
bool RendererGLES::supportsATI1N()
{
	return false;
}

void RendererGLES::changeFrontFace(const GLenum frontFace)
{
	//if (frontFace != currentFrontFace)
	{
		LOG_PRINT_X("glFrontFace:\n");
		glFrontFace(currentFrontFace = frontFace);
   checkGlError("");
	}
}
void RendererGLES::setupSampler(GLenum glTarget, const SamplerStateGL &ss, const bool bVTF_R2VB)
{
if(bVTF_R2VB)
{
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Linear filter might cause a fallback to software rendering so we are using GL_NEAREST
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Linear filter might cause a fallback to software rendering so we are using GL_NEAREST_MIPMAP_NEAREST
	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//Make sure your GPU support mipmaps with floating point textures
	// ??? glTexParameteri(glTarget, GL_GENERATE_MIPMAP, GL_TRUE);
	//glTexImage2D(glTarget, 0, GL_LUMINANCE32F_ARB, width, height, 0, GL_LUMINANCE, GL_FLOAT, data);
	GLfloat value, max_anisotropy = 2.0f; // 8.0f; /* don't exceed this value...*/
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, & value);
	value = (value > max_anisotropy) ? max_anisotropy : value;
	glTexParameterf(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
}
else
{
	LOG_PRINT_X("glTexParameteri:\n");
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, ss.wrapS);
   checkGlError("");
	//if (glTarget != GL_TEXTURE_1D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, ss.wrapT);
	#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
	if (glTarget == GL_TEXTURE_3D) {
	LOG_PRINT_X("glTexParameteri:\n");
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, ss.wrapR);
   	checkGlError("");
	}
	#elif !defined(_LINUX) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
	if (glTarget == GL_TEXTURE_3D_OES) {
	LOG_PRINT_X("glTexParameteri:\n");
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_R_OES, ss.wrapR);
   	checkGlError("");
	}
	#endif
	LOG_PRINT_X("glTexParameteri:\n");
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, ss.magFilter);
   	checkGlError("");

	LOG_PRINT_X("glTexParameteri:\n");
	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, ss.minFilter);
   	checkGlError("");
	if (ss.aniso > 1 && GL_EXT_texture_filter_anisotropic_supported)
	{
		LOG_PRINT_X("glTexParameteri:\n");
		glTexParameteri(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, ss.aniso);
   		checkGlError("");
	}
	}
}

void RendererGLES::setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
	//ASSERT(0);
}

		void RendererGLES::resetToDefaults()
		{
			reset0ToDefaults();
			for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
			{
				currentTextures[i] = TEXTURE_NONE;
			}
			for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
			{
				currentSamplerStates[i] = SS_NONE;
			}
			currentSrcFactorRGB = currentSrcFactorAlpha = ONE;
			currentDstFactorRGB = currentDstFactorAlpha = ZERO;
			currentBlendModeRGB = currentBlendModeAlpha = BM_ADD;
			currentMask = ALL;
			currentBlendEnable = false;
			currentAlphaToCoverageEnable = false;
			currentDepthFunc = LEQUAL;
	currentDepthTestEnable = true;
	currentDepthWriteEnable = true;
			currentCullMode = CULL_NONE;
			currentFillMode = SOLID;
			currentMultiSampleEnable = true;
			currentScissorEnable = false;
		}
		void RendererGLES::resetVrtl(const unsigned int flags)
		{
			reset0(flags);
			if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}

		}




		void RendererGLES::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
		{
			LOG_PRINT_X("Clear:\n");

#if defined(SDL_2_0_5_BUILD)
	SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
#endif

//LOG_FNLN_X;
#if !defined(SDL_2_0_5_BUILD)
#if (!defined(QT_BUILD))
			if(color)
			{
LOG_FNLN_X;
#if 0//defined(QT_BUILD)
	qglClearColor(colors.x,colors.y,colors.z,colors.w);
   checkGlError("");
#elif 0//defined(SDL_2_0_5_BUILD)
	LOG_PRINT_X("glClearColor:\n");
	glClearColor(colors.x,colors.y,colors.z,colors.w);
   	checkGlError("");
#elif !defined(__APPLE__)
LOG_FNLN_X;
	LOG_PRINT_X("glClearColor:\n");
	glClearColor(colors.x,colors.y,colors.z,colors.w);
LOG_FNLN_X;
   	checkGlError("");
#endif
LOG_FNLN_X;

			}
LOG_FNLN_X;
			if(depth)
			{
LOG_FNLN_X;
#if !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glClearDepth:\n");
				glClearDepth(1.0f);
   				checkGlError("");
#endif
LOG_FNLN_X;
			}
LOG_FNLN_X;
			int f=0;
LOG_FNLN_X;
			if(color) f|=GL_COLOR_BUFFER_BIT;
LOG_FNLN_X;
			if(depth) f|=GL_DEPTH_BUFFER_BIT;
LOG_FNLN_X;
			LOG_PRINT_X("glClear:\n");
			glClear(f);
   			checkGlError("");

#endif
#endif
LOG_FNLN_X;
		}
		void RendererGLES::PresentVrtl(bool aB)
		{
//LOG_FNLN_X;
		bool b=true;//???((aB&&(m_pInput!=0)) || ((!aB)&&(m_pInput==0)));
if(b)
{

			IRenderer::OnPresent();

			SwapBuffersGL();

		}
		}

		void RendererGLES::captureScreenshot(Image3& img)
		{
			int w=IRenderer::GetRendererInstance()->GetViewportWidth();
			int h=IRenderer::GetRendererInstance()->GetViewportHeight();
			img.create(FORMAT_RGB8, w, h, 1, 1);
			LOG_PRINT_X("glReadPixels:\n");
			glReadPixels(0, 0, w,h, GL_RGB, GL_UNSIGNED_BYTE, (BYTE *)img.getPixels());
   checkGlError("");
			img.flipY();
		}
		void RendererGLES::SaveImg(Image3& img,const char* fn)
		{
		int l=0;
        l=int(stx_strrchr(fn,'.')-fn);
		std::string fns(fn);
		fns.resize(l);
					char bufpng[256];
                    stx_snprintf(bufpng,256,"%s.png",fns.c_str());
					img.saveImageLibImage(bufpng,false);
		}

		void RendererGLES::SaveScreenShotVrtl(const char* fn)
		{
		if(fn==0)return;

		{
			Image3 img;
			char buf[256];
			char gifbuf[256];
			if(fn!=0)
			{
                stx_snprintf(buf,256,"../%s.jpg",fn);
                stx_snprintf(gifbuf,256,"../%s.gif",fn);

			}
			else
			{
                stx_snprintf(buf,256,"../%s.jpg","ScreenShot.jpg");
                stx_snprintf(gifbuf,256,"../%s.jpg","ScreenShot.gif");
			}
			captureScreenshot(img);
			//img.flipX();
			img.saveImageLibImage(buf,false);
			RendererHLSLCg::OpenILToGIF(img,gifbuf);
		}
		}


		void RendererGLES::changeVertexFormat(const VertexFormatID vertexFormat)
		{
#if 1//defined(USE_HLSL_SHADERS)
			//if (vertexFormat != currentVertexFormat)
			{
				int iii=0;
				static VertexFormatGL zero;
				VertexFormatGL curr = zero, sel = zero;
				if (currentVertexFormat != VF_NONE) curr = vertexFormats[currentVertexFormat];
				if (vertexFormat != VF_NONE) sel = vertexFormats[vertexFormat];
				if ( sel.vertex.size && !curr.vertex.size)
				{
					//glEnableClientState (GL_VERTEX_ARRAY);
					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;
				}
				if (!sel.vertex.size &&  curr.vertex.size)
				{
					//glDisableClientState(GL_VERTEX_ARRAY);
					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;
				}
				if ( sel.normal.size && !curr.normal.size){ //glEnableClientState (GL_NORMAL_ARRAY);

					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				if (!sel.normal.size &&  curr.normal.size){ //glDisableClientState(GL_NORMAL_ARRAY);

					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				for (int i = 0; i < MAX_GENERIC; i++)
				{
					if ( sel.generic[i].size && !curr.generic[i].size)
					{
					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
					if (!sel.generic[i].size &&  curr.generic[i].size){
					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				}
				for (int i = 0; i < MAX_TEXCOORD; i++)
				{
					if ((sel.texCoord[i].size > 0) ^ (curr.texCoord[i].size > 0))
					{
						//glClientActiveTexture(GL_TEXTURE0 + i);
						if (sel.texCoord[i].size > 0)
						{
							//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							//LOG_FNLN_X;
							LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
							glEnableVertexAttribArray(iii);
				checkGlError("");
							iii++;
						}
						else
						{
							//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
							//LOG_FNLN_X;
							LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
							glDisableVertexAttribArray(iii);
				checkGlError("");
							iii++;
						}
					}
				}
				currentVertexFormat = vertexFormat;
			}
#endif
		}

		void RendererGLES::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){
	// stencilRef currently not used

	//if (depthState != currentDepthState)
	{
		if (depthState == DS_NONE){
			if (!currentDepthTestEnable){
				glEnable(GL_DEPTH_TEST);
				currentDepthTestEnable = true;
			}

			if (!currentDepthWriteEnable){
				glDepthMask(GL_TRUE);
				currentDepthWriteEnable = true;
			}

			if (currentDepthFunc != LEQUAL){
				glDepthFunc(currentDepthFunc = LEQUAL);
			}
		} else {
			if (depthStates[depthState].depthTest){
				if (!currentDepthTestEnable){
					glEnable(GL_DEPTH_TEST);
					currentDepthTestEnable = true;
				}
				//if (depthStates[depthState].depthWrite != currentDepthWriteEnable)
				{
					glDepthMask((currentDepthWriteEnable = depthStates[depthState].depthWrite)? GL_TRUE : GL_FALSE);
				}
				//if (depthStates[depthState].depthFunc != currentDepthFunc)
				{
					glDepthFunc(currentDepthFunc = depthStates[depthState].depthFunc);
				}
			} else {
				if (currentDepthTestEnable){
					glDisable(GL_DEPTH_TEST);
					currentDepthTestEnable = false;
				}
			}
		}

		currentDepthState = depthState;
	}
}
		void RendererGLES::changeRasterizerState(const RasterizerStateID rasterizerState)
		{
			//if (rasterizerState != currentRasterizerState)
			{
				RasterizerStateGL state;
				if (rasterizerState == RS_NONE)
				{
					state.cullMode = CULL_NONE;
					state.fillMode = SOLID;
					state.multiSample = true;
					state.scissor = false;
				}
				else
				{
					state = (rasterizerStates[rasterizerState]);
				}
				//if (state.cullMode != currentCullMode)
				{
					if (state.cullMode == CULL_NONE)
					{
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_CULL_FACE);
   checkGlError("");
					}

					else
					{
						if (currentCullMode == CULL_NONE) {glEnable(GL_CULL_FACE);
   checkGlError("");}
						LOG_PRINT_X("glCullFace:\n");
						glCullFace(state.cullMode);
   checkGlError("");
					}
					currentCullMode = state.cullMode;
				}
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
					glPolygonMode(GL_FRONT_AND_BACK, currentFillMode = state.fillMode);
   checkGlError("");
				}
				//if (state.multiSample != currentMultiSampleEnable)
				{
					if (state.multiSample)
					{
						LOG_PRINT_X("glEnable:\n");
						glEnable(GL_MULTISAMPLE);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_MULTISAMPLE);
   checkGlError("");
					}
					currentMultiSampleEnable = state.multiSample;
				}
#endif
				//if (state.scissor != currentScissorEnable)
				{
					if (state.scissor)
					{
						LOG_PRINT_X("glEnable:\n");
						glEnable(GL_SCISSOR_TEST);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_SCISSOR_TEST);
   checkGlError("");
					}
					currentScissorEnable = state.scissor;
				}
				currentRasterizerState = rasterizerState;
			}
		}




		VertexFormatID RendererGLES::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader)
		{
#if !defined(__APPLE__)
			VertexFormatGL vertexFormat;
			stx_memset(&vertexFormat, 0, sizeof(vertexFormat));
			unsigned int nGeneric  = 0;
			unsigned int nTexCoord = 0;

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;

				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
						LOG_PRINT_X("TYPE_GENERIC:\n");
					break;
					case TYPE_TANGENT:
						LOG_PRINT_X("TYPE_TANGENT:\n");
					break;
					case TYPE_BINORMAL:
						LOG_PRINT_X("TYPE_BINORMAL:\n");
					break;
					case TYPE_VERTEX:
						LOG_PRINT_X("TYPE_VERTEX:\n");
					break;
					case TYPE_NORMAL:
						LOG_PRINT_X("TYPE_NORMAL:\n");
					break;
					case TYPE_TEXCOORD:
						LOG_PRINT_X("TYPE_TEXCOORD:\n");
					break;
				};

				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					case TYPE_TANGENT:
					case TYPE_BINORMAL:

					case TYPE_VERTEX:
					//if(vertexFormat.pBase==0) vertexFormat.pBase=&vertexBuffers[vertexFormat.vertex.stream].data[0];
					case TYPE_NORMAL:
					case TYPE_TEXCOORD:
					if(formatDesc[i].type==TYPE_TEXCOORD) nTexCoord++;


					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;

LOG_PRINT_X("nGeneric=%d\n", nGeneric);
LOG_PRINT_X("stream  =%d\n", stream);
LOG_PRINT_X("size    =%d\n", formatDesc[i].size);
LOG_PRINT_X("offset  =%d\n", vertexFormat.vertexSize[stream]);
LOG_PRINT_X("format  =%d\n", formatDesc[i].format);

					nGeneric++;
					break;
				}
				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
				LOG_PRINT_X("vertexSize=%d\n", vertexFormat.vertexSize[stream]);
			}
			vertexFormat.shader=shader;
			vertexFormat.maxGeneric = nGeneric;
			vertexFormat.maxTexCoord = nTexCoord;
			vertexFormats.push_back(vertexFormat);
			return vertexFormats.size()-1;
#else
	return -1;
#endif
		}
GLenum usages_GLES[] =
		{
			GL_STATIC_DRAW,
			GL_DYNAMIC_DRAW,
			GL_STREAM_DRAW,
		}
		;
		//STXArray<GLenum,3> GLES::usages(_GLES::usages);
		VertexBufferID RendererGLES::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID aold)//, const IDataProcessor* p)
		{
//LOG_FNLN_X;
LOG_PRINT_X("size=%d\n", size);
LOG_PRINT_X("data=%x\n", data);
LOG_PRINT_X("aold=%d\n", aold);
//////
if(aold>-1)
{
//////
	//LOG_FNLN_X;
	LOG_PRINT_X("glDeleteBuffers:\n");
	glDeleteBuffers(1,  &vertexBuffers[aold].vboVB);
   	checkGlError("");
//
//////
	vertexBuffers[aold].vboVB=0;
	vertexBuffers[aold].data=(BYTE*)data;
	//LOG_FNLN_X;
	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers(1, &vertexBuffers[aold].vboVB);
   	checkGlError("");

	vertexBuffers[aold].size=size;
//////

	//LOG_FNLN_X;
	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[aold].vboVB);
  	 checkGlError("");


}
	VertexBufferGL vb;
//////
if(aold==-1)
{
	vb.size = size;
/*
LOG_PRINT_X("(void*)&vb.data[0]=%x\n",(void*)&vb.data[0]);
LOG_PRINT_X("data=%x\n",data);
LOG_PRINT_X("size=%x\n",size);
*/
	GLuint vbb;
//////
	//LOG_FNLN_X;
	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers
//ARB
(1, &vbb);
   checkGlError("");

	vb.vboVB=vbb;
	vb.data=(BYTE*)data;
	vertexBuffers.push_back(vb);
//////


	//LOG_FNLN_X;
	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, vbb);
   	checkGlError("");


}
//////
	if(data>=0) {
/////
	//vb.data.resize(size);
	//stx_memcpy((void*)&vb.data[0],data,size);//ABC123
	//LOG_FNLN_X;
	LOG_PRINT_X("glBufferData: size=%d\n", size);
	LOG_PRINT_X("glBufferData: data=%x\n", data);
	glBufferData(GL_ARRAY_BUFFER, size, data, usages_GLES[bufferAccess]);
   	checkGlError("");

}
//////

	//LOG_FNLN_X;
	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
   	checkGlError("");

			if(aold==-1)
			{

				return vertexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<vertexBuffers.size()))
{

return aold;
}

			else
				return -1;

		}
BufferID RendererGLES::addBufferVrtl(const STX_BUFFER_DESC* pDesc, const STX_SUBRESOURCE_DATA* pInitialData)
{
    LOG_FNLN;
    LOG_PRINT("IMPLEMENT!\n");
    return -1;
}
		IndexBufferID RendererGLES::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID aold)//, const IDataProcessor* p)
		{
LOG_FNLN_X;
LOG_PRINT_X("nIndices=%d\n", nIndices);
LOG_PRINT_X("indexSize=%d\n", indexSize);
LOG_PRINT_X("data=%x\n", data);
LOG_PRINT_X("aold=%d\n", aold);

			unsigned int size = nIndices * indexSize;
//////
if(aold>-1)
{
//////
	LOG_PRINT_X("glDeleteBuffers:\n");
	glDeleteBuffers(1,  &indexBuffers[aold].vboIB);
   	checkGlError("");
//
//////
	indexBuffers[aold].vboIB=0;
	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers(1, &indexBuffers[aold].vboIB);
   	checkGlError("");

//////

	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[aold].vboIB);
   	checkGlError("");


//////
	indexBuffers[aold].nIndices = nIndices;
	indexBuffers[aold].indexSize = indexSize;
	indexBuffers[aold].data=(BYTE*)data;
}
	IndexBufferGL ib;
//////
if(aold==-1)
{
	GLuint vbb;

	ib.nIndices = nIndices;
	ib.indexSize = indexSize;;
	ib.data=(BYTE*)data;
//////
	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers(1, &vbb);
   	checkGlError("");

	ib.vboIB=vbb;
//////

	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbb);
   	checkGlError("");


//////
	indexBuffers.push_back(ib);
}
//////
//if(old>-1)
	//indexBuffers[old].size=size;

	if(data>=0) {
//////
	//ib.data.resize(size);
	//stx_memcpy((void*)&ib.data[0],data,size);//ABC123
	//LOG_FNLN_X;
	LOG_PRINT_X("glBufferData: size=%d\n", size);
	LOG_PRINT_X("glBufferData: data=%d\n", data);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usages_GLES[bufferAccess]);
   	checkGlError("");

}
//////

	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   	checkGlError("");


//////
			if(aold==-1)
			{

				//LOG_PRINT_X("vbid=%d\n",vertexBuffers.size()-1);
				return indexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<indexBuffers.size()))
{
return aold;
}
else
	return -1;

		}
		GLint minFilters_GLES[] =
		{
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
		}
		;
		SamplerStateID RendererGLES::addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color)
		{
			if (compareFunc != 0 || border_color != 0) return SS_NONE;
			SamplerStateGL samplerState;
			samplerState.minFilter = minFilters_GLES[filter];
			samplerState.magFilter = (filter == NEAREST)? GL_NEAREST : GL_LINEAR;
			samplerState.wrapS = (s == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapT = (t == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapR = (r == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.aniso = hasAniso(filter)? min((unsigned int) maxAnisotropic, maxAniso) : 1;
			samplerState.lod = lod;
			samplerStates.push_back(samplerState);
			return samplerStates.size()-1;
		}
		DepthStateID RendererGLES::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest,
		const uint8 stencilMask, const int stencilFunc, const int stencilFail, const int depthFail, const int stencilPass)
		{
			DepthStateGL depthState;
			depthState.depthTest  = depthTest;
			depthState.depthWrite = depthWrite;
			depthState.depthFunc  = depthFunc;
			depthStates.push_back(depthState);
			return depthStates.size()-1;
		}
		RasterizerStateID RendererGLES::addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor)
		{
			RasterizerStateGL rasterizerState;
			rasterizerState.cullMode = cullMode;
			rasterizerState.fillMode = fillMode;
			rasterizerState.multiSample = multiSample;
			rasterizerState.scissor = scissor;
			rasterizerStates.push_back(rasterizerState);
			return rasterizerStates.size()-1;
		}
		static const GLint internalFormats_GLES[] =
		{

			0,
			0,//???GL_INTENSITY8,
			GL_LUMINANCE_ALPHA,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			GL_RGB8,
			GL_RGBA8,
#elif defined(SDL_2_0_5_BUILD)// || defined(USE_GLES_SHADERS)
			GL_RGB8_OES,
			GL_RGBA8_OES,
#elif defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
			GL_RGB,
			GL_RGBA,
#else
			GL_RGB8,
			GL_RGBA8,
#endif
			0,//???GL_INTENSITY16,
			0,//???GL_LUMINANCE16_ALPHA16,
			0,//???GL_RGB16,
			0,//???GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,//???GL_INTENSITY_FLOAT16_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			0,//???GL_RGB_FLOAT16_ATI,
			0,//???GL_RGBA_FLOAT16_ATI,
			0,//???GL_INTENSITY_FLOAT32_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			0,//???GL_RGB_FLOAT32_ATI,
			0,//???GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//???GL_RGB5,
			GL_RGBA4,
			0,//???GL_RGB10_A2,
			0,//???GL_DEPTH_COMPONENT16,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			GL_DEPTH_COMPONENT24,
			GL_DEPTH24_STENCIL8,
#elif !defined(_LINUX) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			GL_DEPTH_COMPONENT24_OES,
			GL_DEPTH24_STENCIL8_OES,
#else
			0,
			0,
#endif
			0,
#if !defined(_MSC_VER) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
#else
			0,
#endif
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI,
#else
0,
#endif
		}
		;
		static const GLenum srcFormats_GLES[] =
		{
			0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes_GLES[] =
		{
			0,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0, 0, 0, 0,//???GL_UNSIGNED_SHORT_5_6_5,
			0,//???GL_UNSIGNED_SHORT_4_4_4_4_REV,
			0,//???GL_UNSIGNED_INT_2_10_10_10_REV,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
			GL_UNSIGNED_INT_24_8,
#elif !defined(_LINUX) && !defined(__APPLE__)
			GL_UNSIGNED_INT_24_8_OES,
#else
			0,
#endif
			0, 0,
			0,
			0,
			0,
			0,
		}
		;

		static const char* internalFormats_GLES_[] =
		{

			0,
			0,//???"GL_INTENSITY8",
			"GL_LUMINANCE_ALPHA",
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			"GL_RGB8",
			"GL_RGBA8",
#elif defined(SDL_2_0_5_BUILD)// || defined(USE_GLES_SHADERS)
			"GL_RGB8_OES",
			"GL_RGBA8_OES",
#elif defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
			"GL_RGB",
			"GL_RGBA",
#else
			"GL_RGB8",
			"GL_RGBA8",
#endif
			0,//???GL_INTENSITY16,
			0,//???GL_LUMINANCE16_ALPHA16,
			0,//???GL_RGB16,
			0,//???GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,//???GL_INTENSITY_FLOAT16_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			0,//???GL_RGB_FLOAT16_ATI,
			0,//???GL_RGBA_FLOAT16_ATI,
			0,//???GL_INTENSITY_FLOAT32_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			0,//???GL_RGB_FLOAT32_ATI,
			0,//???GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//???GL_RGB5,
			"GL_RGBA4",
			0,//???GL_RGB10_A2,
			0,//???GL_DEPTH_COMPONENT16,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			"GL_DEPTH_COMPONENT24",
			"GL_DEPTH24_STENCIL8",
#elif !defined(_LINUX) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			"GL_DEPTH_COMPONENT24_OES",
			"GL_DEPTH24_STENCIL8_OES",
#else
			0,
			0,
#endif
			0,
#if !defined(_MSC_VER) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			"GL_COMPRESSED_RGB_S3TC_DXT1_EXT",
#else
			0,
#endif
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
"GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI",
#else
0,
#endif
		}
		;
		static const char* srcFormats_GLES_[] =
		{
			0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
		}
		;
		static const char* srcTypes_GLES_[] =
		{
			0,
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			"GL_FLOAT",
			"GL_FLOAT",
			"GL_FLOAT",
			"GL_FLOAT",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0, 0, 0, 0,//???GL_UNSIGNED_SHORT_5_6_5,
			0,//???GL_UNSIGNED_SHORT_4_4_4_4_REV,
			0,//???GL_UNSIGNED_INT_2_10_10_10_REV,
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_INT",
#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
			"GL_UNSIGNED_INT_24_8",
#elif !defined(_LINUX) && !defined(__APPLE__)
			"GL_UNSIGNED_INT_24_8_OES",
#else
			0,
#endif
			0, 0,
			0,
			0,
			0,
			0,
		}
		;

		void RendererGLES::setSamplerState(const char *samplerName, const SamplerStateID samplerState)
		{
		}
		void RendererGLES::applySamplerStates()
		{
		}


int RendererGLES::GetTextureWidth(TextureID id)
{
	if(id<textures.size())
		return textures[id].width;
	return 0;
}
int RendererGLES::GetTextureHeight(TextureID id)
{
	if(id<textures.size())
		return textures[id].height;
	return 0;
}

		TextureID RendererGLES::addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
		{
			//ASSERT(samplerState != SS_NONE);

			TextureGL tex;

			stx_memset(&tex, 0, sizeof(TextureGL));

	//img->BindImage();
	tex.ss=samplerState;
	tex.width=img->getWidth();
	tex.height=img->getHeight();
	tex.depth=img->getDepth();

	if(useMips&&img->getMipMapCount()<2)
		img->createMipMaps();

	tex.mipMapLevels=img->getMipMapCount();
	const FORMAT format = img->getFormat();
	tex.format=format;

			GLenum srcFormat = srcFormats_GLES[getChannelCount(format)];
			GLenum srcType = srcTypes_GLES[format];
			GLint internalFormat = internalFormats_GLES[format];

			const char* srcFormat_ = srcFormats_GLES_[getChannelCount(format)];
			const char* srcType_ = srcTypes_GLES_[format];
			const char* internalFormat_ = internalFormats_GLES_[format];

			if ((flags & HALF_FLOAT) != 0 && format >= FORMAT_I32f && format <= FORMAT_RGBA32f)
			{
				internalFormat = internalFormats_GLES[format - (FORMAT_I32f - FORMAT_I16f)];
			}

			tex.lod = (samplerStates[samplerState]).lod;
			//tex.glTarget = img->isCube()? GL_TEXTURE_CUBE_MAP : img->is3D()? GL_TEXTURE_3D : img->is2D()? GL_TEXTURE_2D : 0;//???GL_TEXTURE_1D;
			tex.glTarget = img->is2D()? GL_TEXTURE_2D : 0;//???GL_TEXTURE_1D;
			LOG_PRINT_X("glGenTextures:\n");

			glGenTextures(1, &tex.glTexID);
   checkGlError("");

			//LOG_PRINT_X("tex.glTexID=%d\n",tex.glTexID);
			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, tex.glTexID);
   checkGlError("");

	if((format>=FORMAT_R16F) && (FORMAT_RGBA32F<=format))
	{
		setupSampler(tex.glTarget, samplerStates[samplerState], true);
		#if 0
            glTexImage2D(tex.glTarget, 0, GL_LUMINANCE32F_ARB, img->getWidth(), img->getHeight(), 0, GL_LUMINANCE, GL_FLOAT, img->getPixels(0));
        #else
		    glTexImage2D(tex.glTarget, 0, internalFormat, img->getWidth(0), img->getHeight(0), 0, srcFormat, srcType, img->getPixels(0));
        #endif
		return textures.add(tex);
	}
	else
		setupSampler(tex.glTarget, samplerStates[samplerState]);

			tex.samplerState = samplerState;

			const ubyte *src;
			int mipMapLevel = 0;
////LOG_FNLN_X

			while ((src = (useMips? img->getPixels(mipMapLevel) : img->getPixels())) != 0)
			{
				if (img->isCube())
				{
					int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
					for (unsigned int i = 0; i < 6; i++)
					{
						if (isCompressedFormat(format))
						{
							glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, (BYTE *)(src + i * size));
   checkGlError("");
						}
						else
						{
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src + i * size));
   checkGlError("");
						}
					}
				}
				else if (img->is3D())
				{
					if (isCompressedFormat(format))
					{
#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
						glCompressedTexImage3DOES(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), (BYTE *)(src));
   checkGlError("");
#endif
					}
					else
					{

#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
						glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src));
   checkGlError("");
#endif
					}
				}
				else if (img->is2D())
				{
					if (isCompressedFormat(format))
					{
						glCompressedTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), (BYTE *)(src));
   checkGlError("");
					}
					else
					{
						glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src));
   checkGlError("");
					}
				}
				if(!useMips)
					break;
				mipMapLevel++;
			}
////LOG_FNLN_X

			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, 0);
				checkGlError("");

////LOG_FNLN_X
			return textures.add(tex);
		}
		TextureID RendererGLES::addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
		{
			if (isCompressedFormat(format) || arraySize > 1) return false;
			if (mipMapCount > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = (flags & CUBEMAP)? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
			tex.flags  = flags;
			tex.format = format;
			LOG_PRINT_X("glGenTextures:\n");
			glGenTextures(1, &tex.glTexID);
   checkGlError("");
			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, tex.glTexID);
   checkGlError("");
			setupSampler(tex.glTarget, samplerStates[samplerState]);
			tex.samplerState = samplerState;
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, 0);
   checkGlError("");
			return rt;
		}
		TextureID RendererGLES::addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
		{
			if (arraySize > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = GL_RENDERBUFFER;
			tex.format = format;
			LOG_PRINT_X("glGenRenderbuffers:\n");
			glGenRenderbuffers(1, &tex.glDepthID);
   checkGlError("");
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			return rt;
		}
		void RendererGLES::setRenderTargetSize(const TextureID renderTarget, const int width, const int height)
		{
			(textures[renderTarget]).width	= width;
			(textures[renderTarget]).height = height;
			if ((textures[renderTarget]).glTarget == GL_RENDERBUFFER)
			{
				FORMAT format = (textures[renderTarget]).format;
				LOG_PRINT_X("glBindRenderbuffer:\n");
				glBindRenderbuffer(GL_RENDERBUFFER, (textures[renderTarget]).glDepthID);
   checkGlError("");
				LOG_PRINT_X("glRenderbufferStorage:\n");
				glRenderbufferStorage(GL_RENDERBUFFER, internalFormats_GLES[format], width, height);
   checkGlError("");
				LOG_PRINT_X("glBindRenderbuffer:\n");
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
   checkGlError("");
			}
			else
			{
				FORMAT format = (textures[renderTarget]).format;
				GLint internalFormat = internalFormats_GLES[format];
				GLenum srcFormat = srcFormats_GLES[getChannelCount(format)];
				GLenum srcType = srcTypes_GLES[format];

				const char* internalFormat_ = internalFormats_GLES_[format];
				const char* srcFormat_ = srcFormats_GLES_[getChannelCount(format)];
				const char* srcType_ = srcTypes_GLES_[format];

				if (isDepthFormat(format))
				{
					if (isStencilFormat(format))
					{
						#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
						srcFormat = GL_DEPTH_STENCIL;
                        			#elif !defined(_LINUX) && !defined(__APPLE__)
						srcFormat = GL_DEPTH_STENCIL_OES;
						#else
						srcFormat = 0;
						#endif
					}
					else
					{
						srcFormat = GL_DEPTH_COMPONENT;


					}
				}

				if (isFloatFormat(format)) srcType = GL_FLOAT;
				LOG_PRINT_X("glBindTexture:\n");
				glBindTexture((textures[renderTarget]).glTarget, (textures[renderTarget]).glTexID);
   checkGlError("");
				if ((textures[renderTarget]).flags & CUBEMAP)
				{
					for (int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; i++){
//LOG_FNLN_X;
LOG_PRINT_X("glTexImage2D:\n");
LOG_PRINT_X("glTarget=%d\n", i);
LOG_PRINT_X("mipMapLevel=%d\n", 0);
LOG_PRINT_X("internalFormat=%s\n", internalFormat_);
LOG_PRINT_X("WIDTH=%d\n", width);
LOG_PRINT_X("HEIGHT=%d\n", height);
LOG_PRINT_X("srcFormat=%s\n", srcFormat_);
LOG_PRINT_X("srcType=%s\n", srcType_);
LOG_PRINT_X("data=%x\n", 0);
					glTexImage2D(i, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
   checkGlError("");}
				}
				else
				{
//LOG_FNLN_X;
LOG_PRINT_X("glTexImage2D:\n");
LOG_PRINT_X("glTarget=%d\n", (textures[renderTarget]).glTarget);
LOG_PRINT_X("mipMapLevel=%d\n", 0);
LOG_PRINT_X("internalFormat=%s\n", internalFormat_);
LOG_PRINT_X("WIDTH=%d\n", width);
LOG_PRINT_X("HEIGHT=%d\n", height);
LOG_PRINT_X("srcFormat=%s\n", srcFormat_);
LOG_PRINT_X("srcType=%s\n", srcType_);
LOG_PRINT_X("data=%x\n", 0);
					glTexImage2D((textures[renderTarget]).glTarget, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
   checkGlError("");
				}
				LOG_PRINT_X("glBindTexture:\n");
				glBindTexture((textures[renderTarget]).glTarget, 0);
   checkGlError("");
			}
		}
		bool RendererGLES::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
		{
			if (depth > 1 || arraySize > 1 || mipMapCount > 1) return false;
			if (width != (textures[renderTarget]).width || height != (textures[renderTarget]).height)
			{
				setRenderTargetSize(renderTarget, width, height);
			}
			return true;
		}
		bool RendererGLES::generateMipMaps(const TextureID renderTarget)
		{
			return false;
		}
		void RendererGLES::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices)
		{
			apply();
			if (fbo == 0) glGenFramebuffers(1, &fbo);
			LOG_PRINT_X("glBindFramebuffer:\n");
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   checkGlError("");
			for (unsigned int i = 0; i < nRenderTargets; i++)
			{
				TextureID colorRT = colorRTs[i];
				if ((textures[colorRT]).flags & CUBEMAP)
				{
					//if (colorRT != currentColorRT[i] || currentColorRTSlice[i] != slices[i])
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + slices[i], (textures[colorRT]).glTexID, 0);
   checkGlError("");
						currentColorRTSlice[i] = slices[i];
					}
				}
				else
				{
					//if (colorRT != currentColorRT[i])
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, (textures[colorRT]).glTexID, 0);
   checkGlError("");
						currentColorRT[i] = colorRT;
					}
				}
			}
			if (nRenderTargets != nCurrentRenderTargets)
			{
				for (unsigned int i = nRenderTargets; i < nCurrentRenderTargets; i++)
				{
					LOG_PRINT_X("glFramebufferTexture2D:\n");
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
   checkGlError("");
					currentColorRT[i] = TEXTURE_NONE;
					currentColorRTSlice[i] = NO_SLICE;
				}
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
					glDrawBuffer(GL_NONE);
   checkGlError("");
					LOG_PRINT_X("glDrawBuffer:\n");
					glReadBuffer(GL_NONE);
   checkGlError("");
				}
				else
				{
					LOG_PRINT_X("glDrawBuffers:\n");
					glDrawBuffers(nRenderTargets, drawBuffers);
   checkGlError("");
					LOG_PRINT_X("glReadBuffer:\n");
					glReadBuffer(GL_COLOR_ATTACHMENT0);
   checkGlError("");
				}
#endif
				nCurrentRenderTargets = nRenderTargets;
			}
			//if (depthRT != currentDepthRT)
			{
				if (depthRT >= 0 && (textures[depthRT]).glTarget != GL_RENDERBUFFER)
				{
					LOG_PRINT_X("glFramebufferTexture2D:\n");
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
   checkGlError("");
					if (isStencilFormat((textures[depthRT]).format))
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
   checkGlError("");
					}
				}
				else
				{
					LOG_PRINT_X("glFramebufferRenderbuffer:\n");
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (depthRT < 0)? 0 : (textures[depthRT]).glTexID);
   checkGlError("");
					if (depthRT >= 0 && isStencilFormat((textures[depthRT]).format))
					{
						LOG_PRINT_X("glFramebufferRenderbuffer:\n");
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (textures[depthRT]).glTexID);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glFramebufferRenderbuffer:\n");
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
   checkGlError("");
					}
				}
				currentDepthRT = depthRT;
			}
			if (nRenderTargets > 0)
			{
				if ((textures[colorRTs[0]]).flags & CUBEMAP)
				{
					changeFrontFace(GL_CCW);
				}
				else
				{
					changeFrontFace(GL_CW);
				}
				LOG_PRINT_X("glViewport:\n");
				glViewport(0, 0, (textures[colorRTs[0]]).width, (textures[colorRTs[0]]).height);
   checkGlError("");
			}
			else if(depthRT >= 0)
			{
				changeFrontFace(GL_CW);
				LOG_PRINT_X("glViewport:\n");
				glViewport(0, 0, (textures[depthRT]).width, (textures[depthRT]).height);
   checkGlError("");
			}
			reset(0);
		}
		void RendererGLES::changeToMainFramebuffer()
		{
			LOG_PRINT_X("glBindFramebuffer:\n");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
   checkGlError("");
			LOG_PRINT_X("glViewport:\n");
			glViewport(m_viewport.X,m_viewport.Y, m_viewport.Width, m_viewport.Height);
   checkGlError("");
			changeFrontFace(GL_CW);
		}
		void RendererGLES::Init0(void)
		{
	if(defaultFont == -1)
	{
		//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
	}

					}

UINT32 RendererGLES::GetAvailableTextureMem( )
{
/*
        GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
        GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
        GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
        GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
        GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

        VBO_FREE_MEMORY_ATI                     0x87FB
        TEXTURE_FREE_MEMORY_ATI                 0x87FC
        RENDERBUFFER_FREE_MEMORY_ATI            0x87FD

int total, current;
 glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);
 glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &current);

 Local param:Int[4]
 param[0]=0; param[1]=0; param[2]=0; param[3]=0;
 glGetIntegerv (TEXTURE_FREE_MEMORY_ATI, Varptr (param[0]))

// param[0] :

// param[1] :

// param[2] : Total auxiliary memory free.

// param[3] : Largest auxiliary free block.
*/
	return 0;
}

		int RendererGLES::removeTexture(const TextureID texture)
		{
			if(texture>=textures.size()) return -1;
			if ((textures[texture]).glTarget)
			{
				if ((textures[texture]).glTarget == GL_RENDERBUFFER)
				{
					LOG_PRINT_X("glDeleteRenderbuffers:\n");
					glDeleteRenderbuffers(1, &(textures[texture]).glDepthID);
   checkGlError("");
				}
				else
				{
					LOG_PRINT_X("glDeleteTextures:\n");
					glDeleteTextures(1, &(textures[texture]).glTexID);
   checkGlError("");
				}
				(textures[texture]).glTarget = 0;
			}
	int r=0;
	for(unsigned int i=0;i<textures.size();i++)
	{
		if(textures[i].glTarget) r++;
	}
	return r;

		}

		int RendererGLES::deleteVertexBuffer(const VertexBufferID vertexBuffer)
		{
			if(vertexBuffer>=vertexBuffers.size()) return -1;
			if ((vertexBuffers[vertexBuffer]).vboVB)
			{
				LOG_PRINT_X("glDeleteBuffers:\n");
				glDeleteBuffers(1, &(vertexBuffers[vertexBuffer]).vboVB);
   checkGlError("");

				(vertexBuffers[vertexBuffer]).vboVB = 0;
			}
	int r=0;
	for(unsigned int i=0;i<vertexBuffers.size();i++)
	{
		if(vertexBuffers[i].vboVB) r++;
	}
	return r;
		}
		int RendererGLES::deleteIndexBuffer(const IndexBufferID indexBuffer)
		{
			if(indexBuffer>=indexBuffers.size()) return -1;
			if ((indexBuffers[indexBuffer]).vboIB)
			{
				LOG_PRINT_X("glDeleteBuffers:\n");
				glDeleteBuffers(1, &(indexBuffers[indexBuffer]).vboIB);
   checkGlError("");

				(indexBuffers[indexBuffer]).vboIB = 0;
			}
				int r=0;
	for(unsigned int i=0;i<indexBuffers.size();i++)
	{
		if(indexBuffers[i].vboIB) r++;
	}
	return r;

		}
int RendererGLES::texturePitchVrtl(const TextureID id,int level){return 0;}
BYTE *RendererGLES::lockTexture(const TextureID id,int level, int& pitch){return 0;}
BYTE *RendererGLES::lockVertexBuffer(const VertexBufferID id){LOG_FNLN_X;
		return 0;}
BYTE *RendererGLES::lockIndexBuffer(const IndexBufferID id){LOG_FNLN_X;
		return 0;}
BYTE *RendererGLES::unlockTextureVrtl(const TextureID id,int level){return 0;}
BYTE *RendererGLES::unlockVertexBuffer(const VertexBufferID id){LOG_FNLN_X;
		return 0;}
BYTE *RendererGLES::unlockIndexBuffer(const IndexBufferID id){LOG_FNLN_X;
		return 0;}

#if 0
HRESULT RendererGLES::ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return textureProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGLES::LockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGLES::opyToResourceTexture(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGLES::UnLockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererGLES::ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return vertexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGLES::LockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGLES::opyToResourceVertexBuffer(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGLES::UnLockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererGLES::ProcessIndexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return indexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGLES::LockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGLES::opyToResourceIndexBuffer(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGLES::UnLockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].UnLockDeviceObject();
}

ProcessorID RendererGLES::addTextureProcessor( TextureID& ppTexture, int g_SkipMips )
	{
		static int id=-1;
			CTextureProcessorGL tp(this,ppTexture);//,g_pResourceReuseCache, g_SkipMips );

			id=textureProcessors.add(tp);
		return id;
	}

ProcessorID RendererGLES::addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData)

	{
		static int id=-1;
			CVertexBufferProcessorGL tl(this,ppBuffer);//, iSizeBytes, pData);
			id=vertexBufferProcessors.add(tl);
		return id;
	}

ProcessorID RendererGLES::addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData)
	{
		static int id=-1;
			CIndexBufferProcessorGL tl(this,ppBuffer);//, iSizeBytes,4/*???*/, pData);
			id=indexBufferProcessors.add(tl);
		return id;
	}

#endif
void RendererGLES::viewport(int x, int y, int viewportWidth, int viewportHeight){
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(x, y, viewportWidth, viewportHeight);
       // changeFrontFace(GL_CW);
  m_viewport.X=x;
  m_viewport.Y=y;
  m_viewport.Width=viewportWidth;
  m_viewport.Height=viewportHeight;

}
#if 0

void	RendererGLES::CreateTextureFromFile_Serial(  CHAR* szFileName,
						TextureID* ppTexture,
						//, void* pContext
						CPackedFile*		    g_PackFile,
						UINT g_SkipMips
						)
{
    LoaderID pLoader=addTextureLoader( szFileName, g_PackFile );
    ProcessorID pProcessor=addTextureProcessor( *ppTexture,// g_pResourceReuseCache
		 g_SkipMips );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( RendererGLES::HLSLCg::LoadTexture(pLoader) ) )
	goto Error;
    if( FAILED( DecompressTexture( pLoader,&pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessTexture( pLoader,pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockTextureDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceTexture(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockTextureDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}

void	RendererGLES::CreateVertexBuffer_Serial(
						VertexBufferID& ppBuffer,
						UINT iSizeBytes,
					    //__DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
					    //void* pContext
						)
{
    LoaderID pLoader=addVertexBufferLoader();
    ProcessorID pProcessor=addVertexBufferProcessor( ppBuffer, iSizeBytes,  //ibFormat,
								   pData );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( LoadVertexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( DecompressVertexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessVertexBuffer(pProcessor, pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceVertexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}

void	RendererGLES::CreateIndexBuffer_Serial(
						IndexBufferID& ppBuffer,
						UINT iSizeBytes,
					    //__DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
					    //void* pContext
						)
{
    LoaderID pLoader=addIndexBufferLoader();
    ProcessorID pProcessor=addIndexBufferProcessor( ppBuffer, iSizeBytes,  //ibFormat,
								   pData );

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( LoadIndexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( DecompressIndexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessIndexBuffer(pProcessor, pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceIndexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}
#endif
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * **
*  |_|	   |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|	*
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _	*
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_	*
*  |_|	   |_| |_|_ _ _|_| |_|	   |_| |_|_ _ _|_|  _ _ _ _|_|	*
*  |_|	   |_|	 |_|_|_|   |_|	   |_|	 |_|_|_|   |_|_|_|_|	*
*								*
*		      http://www.humus.name			*
*								 *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.		   *
*								 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    BYTE* RendererGLES::LockTexture(TextureID id, int m, int& pitch)
    {
		return textures[id].Lock(m,pitch);
	}
    void RendererGLES::UnlockTexture(TextureID id, int m)
    {
		textures[id].Unlock(m);
	}
    BYTE* RendererGLES::LockVertexBuffer(VertexBufferID id)
    {
		LOG_FNLN_X;
		return vertexBuffers[id].Lock();
	}
    void RendererGLES::UnLockVertexBuffer(VertexBufferID id)
    {
		LOG_FNLN_X;
		vertexBuffers[id].Unlock();
	}
    BYTE* RendererGLES::LockIndexBuffer(IndexBufferID id)
    {
		LOG_FNLN_X;
		return indexBuffers[id].Lock();
	}
    void RendererGLES::UnLockIndexBuffer(IndexBufferID id)
    {
		LOG_FNLN_X;
		indexBuffers[id].Unlock();
	}

	void RendererGLES::GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc)
	{
		sDesc->Width=textures[id].width;
		sDesc->Height=textures[id].height;
		int i=0;
		while(i<level)
		{
		sDesc->Width = sDesc->Width >> 1;
        sDesc->Height = sDesc->Height >> 1;
		i++;
		}
	}
	unsigned int RendererGLES::GetLevelCount(TextureID id)
	{
		return textures[id].mipMapLevels;
	}
	void RendererGLES::GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc)
	{
		sDesc->Size=vertexBuffers[id].size;
	}
	void RendererGLES::GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc)
	{
		//sDesc->Size=indexBuffers[id].nIndices*indexBuffers[id].indexSize;
		sDesc->indexSize=indexBuffers[id].indexSize;
		sDesc->nIndices=indexBuffers[id].nIndices;
		sDesc->Format=indexBuffers[id].indexSize;
	}

		void RendererGLES::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask)
        {
        //if (blendState != currentBlendState){
            if (blendState == BS_NONE || !blendStates[blendState].blendEnable){
                if (currentBlendEnable){
                    LOG_PRINT_X("glDisable:\n");
			glDisable(GL_BLEND);
   checkGlError("");
                    currentBlendEnable = false;
                }
            } else {
                if (blendStates[blendState].blendEnable){
                    if (!currentBlendEnable){
                        LOG_PRINT_X("glEnable:\n");
			glEnable(GL_BLEND);
   checkGlError("");
                        currentBlendEnable = true;
                    } /*
                    if (blendStates[blendState].srcFactorRGB   != currentSrcFactorRGB ||
                        blendStates[blendState].dstFactorRGB   != currentDstFactorRGB ||
                        blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha ||
                        blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha) */
												{

                        LOG_PRINT_X("glBlendFuncSeparate:\n");
			glBlendFuncSeparate(
                            currentSrcFactorRGB   = blendStates[blendState].srcFactorRGB,
                            currentDstFactorRGB   = blendStates[blendState].dstFactorRGB,
                            currentSrcFactorAlpha = blendStates[blendState].srcFactorAlpha,
                            currentDstFactorAlpha = blendStates[blendState].dstFactorAlpha);
   checkGlError("");
 } /*
                    if (blendStates[blendState].blendModeRGB   != currentBlendModeRGB ||
                        blendStates[blendState].blendModeAlpha != currentBlendModeAlpha) */
												{
                        LOG_PRINT_X("glBlendEquationSeparate:\n");
			glBlendEquationSeparate(
                            currentBlendModeRGB   = blendStates[blendState].blendModeRGB,
                            currentBlendModeAlpha = blendStates[blendState].blendModeAlpha);
   checkGlError("");
                    }
                }
            }

            int mask = ALL;
            bool alphaToCoverage = false;
            if (blendState != BS_NONE){
                mask = blendStates[blendState].mask;
                alphaToCoverage = blendStates[blendState].alphaToCoverageEnable;
            }

            //if (mask != currentMask)
						{
                LOG_PRINT_X("glColorMask:\n");
		glColorMask((mask & RED) & 1, ((mask & GREEN) >> 1) & 1, ((mask & BLUE) >> 2) & 1, ((mask & ALPHA) >> 3) & 1);
   		checkGlError("");

                currentMask = mask;
            }

            //if (alphaToCoverage != currentAlphaToCoverageEnable)
            {
                if (alphaToCoverage) {
                    LOG_PRINT_X("glEnable:\n");
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
   checkGlError("");
                } else {
                    LOG_PRINT_X("glDisable:\n");
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
   checkGlError("");
		}
                currentAlphaToCoverageEnable = alphaToCoverage;
            }

            currentBlendState = blendState;
        }
    //}

void RendererGLES::changeAlphaState(const AlphaStateID blendState)
{
	//if (blendState != currentAlphaState)
	{
		if (blendState == -1 || !(alphaStates[blendState]).blendEnable)
		{
			if (currentAlphaEnable)
			{
                #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glDisable:\n");
				glDisable(GL_ALPHA_TEST);
   checkGlError("");
				#endif
    			currentAlphaEnable = false;
			}
		}
		else
		{
			if ((alphaStates[blendState]).blendEnable)
			{
				if (!currentAlphaEnable)
				{
                #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glEnable:\n");
				glEnable(GL_ALPHA_TEST);
   checkGlError("");
				#endif
    				currentAlphaEnable = true;
    //  reference = clamp(referenceArg, 0.0f, 1.0f);
    #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
    LOG_PRINT_X("glAlphaFunc:\n");
	glAlphaFunc(GL_GEQUAL, 0.5f);//???
   checkGlError("");
	#endif
				}
			}
		}
		currentAlphaState = blendState;

}
}


		BlendStateID RendererGLES::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage)
		{
			BlendStateGL blendState;
			blendState.srcFactorRGB   = srcFactorRGB;
			blendState.dstFactorRGB   = destFactorRGB;
			blendState.srcFactorAlpha = srcFactorAlpha;
			blendState.dstFactorAlpha = destFactorAlpha;
			blendState.blendModeRGB   = blendModeRGB;
			blendState.blendModeAlpha = blendModeAlpha;
			blendState.mask = mask;
			blendState.blendEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
			blendState.alphaEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
			blendState.alphaToCoverageEnable = alphaToCoverage;
			blendStates.push_back(blendState);
			return blendStates.size()-1;
		}

AlphaStateID RendererGLES::addAlphaState(
		const int func,
		const float ref
	)
{
	AlphaStateGL blendState;
    blendState.blendEnable = true;
	blendState.alphaFunc=GEQUAL;
	alphaStates.push_back(blendState);
	return alphaStates.size()-1;
}
float RendererGLES::getTextWidth(const FontID font, const char *str, int length) const {
	if (font < 0) return 0;
    if (length < 0) length = (int) stx_strlen(str);

	Character *chars = fonts[font].chars;

	float len = 0;
	for (int i = 0; i < length; i++){
		len += chars[(unsigned char) str[i]].ratio;
	}

	return len;
}

unsigned int RendererGLES::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices){
	if(!glPrim_GLES[primitives]) return 0;
			apply();
	LOG_PRINT_X("glDrawArrays:\n");
	//glDrawArrays (GLenum mode, GLint first, GLsizei count);
	glDrawArrays(glPrim_GLES[primitives], firstVertex, nVertices);
   checkGlError("");
	reset();

	nDrawCalls++;
	return 0;
}

unsigned int RendererGLES::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices){
	if(!glPrim_GLES[primitives]) return 0;
			apply();
	unsigned int indexSize = indexBuffers[selectedIndexBuffer].indexSize;

	//glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

			unsigned short* usd=(unsigned short*) indexBuffers[selectedIndexBuffer].data;
			unsigned int  * uid=(unsigned int*)   indexBuffers[selectedIndexBuffer].data;
			if(indexSize == 2){
				glDrawElements(
					glPrim_GLES[primitives],
					nIndices,
			 		GL_UNSIGNED_SHORT,
			 		(usd+firstIndex)//???
				);
				checkGlError("");}
			else{
				glDrawElements(
					glPrim_GLES[primitives],
					nIndices,
			 		GL_UNSIGNED_INT,
			 		(uid+firstIndex)//???
				);
				checkGlError("");}
	reset();
	nDrawCalls++;
	return 0;
}
unsigned int RendererGLES::drawPrimitivesVrtl(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	LOG_FNLN;
	LOG_PRINT("IMPLEMENT\n");
	return 0;
}
#endif
