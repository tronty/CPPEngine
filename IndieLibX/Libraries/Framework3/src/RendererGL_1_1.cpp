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
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/RendererGL_1_1.h>
#ifdef __RendererGL_1_1_H__
#include <STX/LuaUtil.h>
using namespace LuaUtil;

#define USEXML 1

#ifdef USEXML 
extern TiXmlElement* _IRenderer_BodyElement;
#else
extern LuaScript _IRenderer_script;
#endif

#if !defined(GLCG1_1)
#define LOG_PRINT(...)
#define LOG_FNLN
#define STX_PRINT(...)
#define STX_FNLN
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#define LOG_FNLN_X
#define LOG_PRINT_X
#endif

#ifdef __APPLE__
#define LOG_FNLN_OSX
#define LOG_PRINT_OSX
#else
#define LOG_FNLN_OSX
#define LOG_PRINT_OSX
#endif

		BYTE* IndexBufferGL::Lock()
	{
		BYTE* m_pLockedData=0;
            // Lock
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboIB);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB,
				nIndices*indexSize,//???
				0, GL_DYNAMIC_DRAW_ARB);
				checkGlError("");
			m_pLockedData = (BYTE *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY);
				checkGlError("");
		return m_pLockedData;
	}
	void IndexBufferGL::Unlock()
	{
 // Unlock
        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
				checkGlError("");
	}
	BYTE* VertexBufferGL::Lock()
	{
		BYTE* m_pLockedData=0;
            // Lock
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboVB);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, 0, GL_DYNAMIC_DRAW_ARB);
				checkGlError("");
			m_pLockedData = (BYTE *)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY);
				checkGlError("");
		return m_pLockedData;
	}
	void VertexBufferGL::Unlock()
	{
// Unlock
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
				checkGlError("");
    }
	BYTE* TextureGL::Lock(int m, int& pitch)//int& slicePitch;
	{


//		int slicePitch;
		Image3 img;
		img.create(format,width, height,depth,mipMapLevels);

#if 0
				unsigned int nMipMaps = img->getMipMapCount();
				unsigned int nSlices = img->isCube()? 6 : 1;
				unsigned int arraySize = img->getArraySize();
				//static D3D10_SUBRESOURCE_DATA texData[1024];
				//D3D10_SUBRESOURCE_DATA *dest = texData;
				//for (unsigned int n = 0; n < arraySize; n++)
				{
				//	for (unsigned int k = 0; k < nSlices; k++)
					{
						for (unsigned int i =
m//0
; i <
m+1//nMipMaps
; i++)
								  {
							if (isCompressedFormat(format))
							{
								pitch = ((img->getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
								slicePitch = pitch * ((img->getHeight(i) + 3) >> 2);
							}
							else
							{
								pitch = img->getWidth(i) * getBytesPerPixel(format);
								slicePitch = pitch * img->getHeight(i);
							}
/*
							dest->pSysMem = img->getPixels(i, n) + k * slicePitch;
							dest->SysMemPitch = pitch;
							dest->SysMemSlicePitch = slicePitch;
							dest++;
*/
						}
					}
				}
#endif

BYTE* m_pLockedData= 0;

#if !defined( _MSC_VER) && !defined(__APPLE__)
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, glTexID);
				checkGlError("");
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,
				img.getMipMappedSize(0, mipMapLevels),//???,
				0, GL_DYNAMIC_DRAW_ARB);
				checkGlError("");
		m_pLockedData= ((BYTE *)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY))+(img.getPixels(m)-img.getPixels(0));//img->getDataOffset(m,format);
				checkGlError("");
#endif
		pitch=img.getPitch(m);
		return m_pLockedData;
	}
	void TextureGL::Unlock(int m)
	{
		#if !defined( _MSC_VER) && !defined(__APPLE__)
glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT);
				checkGlError("");
//glTexImage2D    (GLenum target, GLint level, GLint internalformat,         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
//glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,GLenum format, GLenum type, const GLvoid *pixels);
//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
//glTexImage2D((textures[renderTarget]).glTarget, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
//glTexSubImage2D(GL_TEXTURE_2D, tex.glTarget,i,0,0,tex.width, tex.height,srcFormat, srcType ,0);
#endif
	}

void RendererGL_1_1::BeginSceneVrtl(bool aB)
{
	OnBeginScene();
#if defined(SDL_2_0_5_BUILD)
	//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("STX_Service::GetSDLWindow()=%x\n", STX_Service::GetSDLWindow());
            SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
            //SDL_GL_GetDrawableSize(STX_Service::GetSDLWindow(), &w, &h);
#endif
}

		void RendererGL_1_1::SwapBuffersGL(void)
		{
	LOG_FNLN;
//LOG_PRINT_NONE("SDL_GL_SwapBuffers:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
#if defined(SDL_2_0_5_BUILD)
	//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("STX_Service::GetSDLWindow()=%x\n", STX_Service::GetSDLWindow());
	SDL_GL_SwapWindow(STX_Service::GetSDLWindow());
#elif defined(SDL_1_2_15_BUILD)
	SDL_GL_SwapBuffers();
#elif defined(_MSC_VER)
	SwapBuffers( hDc );
#else
	if(!m_dpy) m_dpy=STX_Service::GetWindowInstance()->m_dpy;
	if(!m_win) m_win=STX_Service::GetWindowInstance()->m_win;
	//LOG_PRINT_NONE("m_dpy=%d\n", m_dpy);
	//LOG_PRINT_NONE("m_win=%d\n", m_win);
	glXSwapBuffers( m_dpy, m_win );
#endif
		LOG_FNLN;
		LOG_PRINT("SwapBuffersGL:\n");
		//stx_exit(0);
		}

#ifndef GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif
	RendererGL_1_1::~RendererGL_1_1(){}
	RendererGL_1_1::RendererGL_1_1(
			bool fullScreen
			,int argc, char *argv[]
		): RendererHLSLCg(argc, argv)
{
{
	#if 1
	int major=1;
	int minor=1;
	const char *version = (const char *) glGetString(GL_VERSION);
	if (version)
	{
		major = atoi(version);
		minor = atoi(stx_strchr(version, '.') + 1);
	}
	version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (version)
	{
		SHADING_LANGUAGE_VERSION_MAJOR = atoi(version);
		SHADING_LANGUAGE_VERSION_MINOR = atoi(stx_strchr(version, '.') + 1);
	}
	#endif

	#if 1
	printf("GL_VERSION: %d.%d\n", major, minor);
	printf("GL_SHADING_LANGUAGE_VERSION: %d.%d\n", SHADING_LANGUAGE_VERSION_MAJOR, SHADING_LANGUAGE_VERSION_MINOR);
	#endif
	#if 0
	GLint N;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_TEXTURE_IMAGE_UNITS=%d\n", N);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS=%d\n", N);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS=%d\n", N);
#ifndef __APPLE__
	glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS=%d\n", N);
	glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS=%d\n", N);
	glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS=%d\n", N);
	glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &N);
	STX_PRINT("GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS=%d\n", N);
#endif
	GLfloat F;
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &F);//  1.0
	STX_PRINT("GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT=%f\n", F);
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &F);//  2.0
	STX_PRINT("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT=%f\n", F);
	#endif
}
	// GL cannot use palettes anyway, so convert early.
	//ilEnable (IL_CONV_PAL);

	// Gets rid of dithering on some nVidia-based cards.
	//???ilutEnable (ILUT_OPENGL_CONV);

	//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("STX_Service::GetSDLWindow()=%x\n", STX_Service::GetSDLWindow());
}

void RendererGL_1_1::setGLdefaults()
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
	BM_ADD		    = GL_FUNC_ADD_EXT;
	BM_SUBTRACT	    = GL_FUNC_SUBTRACT_EXT;
	BM_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT_EXT;
	BM_MIN		    = GL_MIN_EXT;
	BM_MAX		    = GL_MAX_EXT;
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
	SOLID = GL_FILL;
	WIREFRAME = GL_LINE;
		
        fbo=0;
        currentVBO=0;

	glEnable(GL_DEPTH_TEST);
				checkGlError("");
	glDepthFunc(GL_LEQUAL);
				checkGlError("");
	glFrontFace(GL_CW);
				checkGlError("");
	glPixelStorei(GL_PACK_ALIGNMENT,   1);
				checkGlError("");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				checkGlError("");
				
				
				
				
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

void RendererGL_1_1::setup2DMode(const float left, const float right, const float top, const float bottom){
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

const GLenum glPrimRendererGL_1_1[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,
}
;
const char* glPrimRendererGL_1_1_[] =
{
	"GL_TRIANGLES",
	"GL_TRIANGLE_FAN",
	"GL_TRIANGLE_STRIP",
	"GL_QUADS",
	"GL_LINES",
	"GL_LINE_STRIP",
	"GL_LINE_LOOP",
	"GL_POINTS",
};
bool RendererGL_1_1::supportsR2VB()
{
	return false;
}

bool RendererGL_1_1::supportsHalf()
{
	return bsupportsHalf;
}
bool RendererGL_1_1::supportsPS3()
{
	return false;
}
bool RendererGL_1_1::supportsATI1N()
{
	return false;
}

void RendererGL_1_1::changeFrontFace(const GLenum frontFace)
{
#if defined(LINUX)
	if (frontFace != currentFrontFace)
#endif
	{
		//LOG_FNLN;
		//LOG_PRINT("changeFrontFace(%x)\n", frontFace);
		glFrontFace(currentFrontFace = frontFace);
	}
}
void RendererGL_1_1::setupSampler(GLenum glTarget, const SamplerStateGL &ss, const bool bVTF_R2VB)
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
	glTexParameteri(glTarget, GL_GENERATE_MIPMAP, GL_TRUE);
	//glTexImage2D(glTarget, 0, GL_LUMINANCE32F_ARB, width, height, 0, GL_LUMINANCE, GL_FLOAT, data);
	GLfloat value, max_anisotropy = 2.0f; // 8.0f; /* don't exceed this value...*/
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, & value);
	value = (value > max_anisotropy) ? max_anisotropy : value;
	glTexParameterf(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
}
else
{
	// Set requested wrapping modes
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, ss.wrapS);
	if (glTarget != GL_TEXTURE_1D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, ss.wrapT);
	if (glTarget == GL_TEXTURE_3D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, ss.wrapR);

	// Set requested filter modes
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, ss.magFilter);
	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, ss.minFilter);

	// Setup anisotropic filtering
	if (ss.aniso > 1 && /*glTarget == GL_TEXTURE_2D && */GL_EXT_texture_filter_anisotropic_supported){
		glTexParameteri(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, ss.aniso);
	}
}
}

void RendererGL_1_1::setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
	//ASSERT(0);
}

		void RendererGL_1_1::resetToDefaults()
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
		void RendererGL_1_1::resetVrtl(const unsigned int aflags)
		{
			//LOG_FNLN_NONE;
				reset0(aflags);

	#if 1
	unsigned int flags=aflags;
	#else
	unsigned int flags=getResetFlag();
	#endif
	#if 0
	if((selectedShader==-1) && (selectedVertexFormat==-1))
		flags=0;
	#endif
	#if 1	
		if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}
	//setResetFlag(0);
	#endif

		}

		void RendererGL_1_1::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
		{
LOG_FNLN;
			IRenderer::OnClear();
//
#if 1//!defined(SDL_2_0_5_BUILD)
#if (!defined(QT_BUILD))
			if(color)
			{
#if 0//defined(QT_BUILD)
	qglClearColor(colors.x,colors.y,colors.z,colors.w);
#elif 0//defined(SDL_2_0_5_BUILD)
	glClearColor(colors.x,colors.y,colors.z,colors.w);
				checkGlError("");
#else
				glClearColor(colors.x,colors.y,colors.z,colors.w);
				checkGlError("");
#endif

			}
			if(depth)
			{
				glClearDepth(1.0f);
				checkGlError("");
			}
			int f=0;
			if(color) f|=GL_COLOR_BUFFER_BIT;
			if(depth) f|=GL_DEPTH_BUFFER_BIT;
			glClear(f);
				checkGlError("");
#endif
#endif
		}
		void RendererGL_1_1::PresentVrtl(bool aB)
		{
//
		//LOG_FNLN_NONE;
		bool b=true;//???((aB&&(m_pInput!=0)) || ((!aB)&&(m_pInput==0)));
if(b)
{

			//LOG_FNLN_NONE;
		IRenderer::OnPresent();
			//LOG_FNLN_NONE;
		SwapBuffersGL();

	bool m_bDebugInit=false;
	
#if 0
#ifndef USEXML
	if(!m_bLuaScript)
	{
		std::string DebugInit = _IRenderer_script.get<std::string>("DebugInit");
		if(DebugInit=="True")
			m_bDebugInit=true;
	}
#else
	{
	if(_IRenderer_BodyElement){
		std::string DebugInit = _IRenderer_BodyElement->FirstChild("DebugInit")->ToElement()->Attribute("Value");
		if(DebugInit=="True")
			m_bDebugInit=true;
	}
	}
#endif
		if(m_bDebugInit)
			stx_exit(0);
#endif
		}
		//stx_exit(0);
		}

		void RendererGL_1_1::captureScreenshot(Image3& img)
		{
			int w=IRenderer::GetRendererInstance()->GetViewportWidth();
			int h=IRenderer::GetRendererInstance()->GetViewportHeight();
			img.create(FORMAT_RGB8, w, h, 1, 1);
			glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (BYTE *)img.getPixels());
				checkGlError("");
			img.flipY();
		}
		void RendererGL_1_1::SaveImg(Image3& img,const char* fn)
		{
		int l=0;
        l=int(stx_strrchr(fn,'.')-fn);
		std::string fns(fn);
		fns.resize(l);
					char bufpng[256];
                    stx_snprintf(bufpng,256,"%s.png",fns.c_str());
					img.saveImageLibImage(bufpng,false);
		}

		void RendererGL_1_1::SaveScreenShotVrtl(const char* fn)
		{
		//printf("%s:%s:%d%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		if(fn==0)return;

		{
		//printf("%s:%s:%d%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
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
                stx_snprintf(buf,256,"../%s","ScreenShot.jpg");
                stx_snprintf(gifbuf,256,"../%s","ScreenShot.gif");
			}
		////printf("%s:%s:%dRendererGL_1_1::SaveScreenShotVrtl2:\n");
			captureScreenshot(img);
			img.flipX();
			//printf("%s:%s:%d%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			//printf("%s:%s:%dimg.saveImageLibImage:%s\n", buf);
			img.saveImageLibImage(buf,false);
			RendererHLSLCg::OpenILToGIF(img,gifbuf);
		//printf("RendererGL_1_1::SaveScreenShotVrtl3:\n");
		}
		}


		void RendererGL_1_1::changeVertexFormat(const VertexFormatID vertexFormat)
		{
		LOG_PRINT("changeVertexFormat(%d)\n", vertexFormat);
#ifdef __LOG_glDrawElements__
	LOG_FNLN;
#endif	
	if (vertexFormat == VF_NONE)
		return;
#if 1//defined(LINUX)
			if (vertexFormat != currentVertexFormat)
#endif
			{

	if(1){
	if(m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("changeVertexFormat(%d)\n", vertexFormat);
		LOG_PRINT("vertexFormat(%x)\n", vertexFormat);
		LOG_PRINT("currentVertexFormat(%x)\n", currentVertexFormat);
	}}

				static VertexFormatGL zero;
				VertexFormatGL curr = zero, sel = zero;
				if (currentVertexFormat != VF_NONE) curr = vertexFormats[currentVertexFormat];
				if (vertexFormat != VF_NONE) sel = vertexFormats[vertexFormat];
				if ( sel.vertex.size && !curr.vertex.size)
				{
					glEnableClientState (GL_VERTEX_ARRAY);
		LOG_FNLN;
				checkGlError("");
		LOG_FNLN;
if(1){
	if(m_bDebug){LOG_PRINT("glEnableClientState(GL_VERTEX_ARRAY)\n");}
	}
				}
				if (!sel.vertex.size &&  curr.vertex.size)
				{
					glDisableClientState(GL_VERTEX_ARRAY);
				checkGlError("");
	if(1){
	if(m_bDebug){LOG_PRINT("glDisableClientState(GL_VERTEX_ARRAY)\n");}
	}
				}
				if ( sel.normal.size && !curr.normal.size)
 				{
				glEnableClientState(GL_NORMAL_ARRAY);
				checkGlError("");if(1){
	if(m_bDebug){LOG_PRINT("glEnableClientState(GL_NORMAL_ARRAY)\n");}
	}
				}
				if (!sel.normal.size &&  curr.normal.size) {glDisableClientState(GL_NORMAL_ARRAY);
				checkGlError("");if(1){
	if(m_bDebug){LOG_PRINT("glDisableClientState(GL_NORMAL_ARRAY)\n");}
	}
}
				for (int i = 0; i < MAX_GENERIC; i++)
				{
					if ( sel.generic[i].size && !curr.generic[i].size) {glEnableVertexAttribArrayARB(i);
				checkGlError("");
	if(1){
	if(m_bDebug){LOG_PRINT("glEnableVertexAttribArrayARB(%d)\n", i);}
	}}
					if (!sel.generic[i].size &&  curr.generic[i].size) {glDisableVertexAttribArrayARB(i);
				checkGlError("");
	if(1){
	if(m_bDebug){LOG_PRINT("glDisableVertexAttribArrayARB(%d)\n", i);}
	}}
				}
				for (int i = 0; i < MAX_TEXCOORD; i++)
				{
					if ((sel.texCoord[i].size > 0) ^ (curr.texCoord[i].size > 0))
					{
						glClientActiveTexture(GL_TEXTURE0 + i);
		LOG_FNLN;
				checkGlError("");
		LOG_FNLN;




	if(1){
	if(m_bDebug){LOG_PRINT("glClientActiveTexture(%d)\n", i);}
	}
						if (sel.texCoord[i].size > 0)
						{
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				checkGlError("");




	if(1){
	if(m_bDebug){LOG_PRINT("glEnableClientState(GL_TEXTURE_COORD_ARRAY)\n");}
	}
						}
						else
						{
							glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				checkGlError("");




	if(1){
	if(m_bDebug){LOG_PRINT("glDisableClientState(GL_TEXTURE_COORD_ARRAY)\n");}
	}

						}
					}
				}
				currentVertexFormat = vertexFormat;
			}
		}

		void RendererGL_1_1::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){
	// stencilRef currently not used

#if defined(LINUX)
			if (depthState != currentDepthState)
#endif
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
#if defined(LINUX)
				if (depthStates[depthState].depthWrite != currentDepthWriteEnable)
#endif
				{
					glDepthMask((currentDepthWriteEnable = depthStates[depthState].depthWrite)? GL_TRUE : GL_FALSE);
				}
#if defined(LINUX)
				if (depthStates[depthState].depthFunc != currentDepthFunc)
#endif
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
		void RendererGL_1_1::changeRasterizerState(const RasterizerStateID rasterizerState)
		{
#if defined(LINUX)
			if (rasterizerState != currentRasterizerState)
#endif
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
#if defined(LINUX)
				if (state.cullMode != currentCullMode)
#endif
				{
					if (state.cullMode == CULL_NONE)
					{
						glDisable(GL_CULL_FACE);
				checkGlError("");
					}

					else
					{
						if (currentCullMode == CULL_NONE) {glEnable(GL_CULL_FACE);
				checkGlError("");}
						glCullFace(state.cullMode);
				checkGlError("");
					}
					currentCullMode = state.cullMode;
				}
#if defined(LINUX)
				if (state.fillMode != currentFillMode)
#endif
				{
					glPolygonMode(GL_FRONT_AND_BACK, currentFillMode = state.fillMode);
				checkGlError("");
				}
#if defined(LINUX)
				if (state.multiSample != currentMultiSampleEnable)
#endif
				{
					if (state.multiSample)
					{
						glEnable(GL_MULTISAMPLE);
				checkGlError("");
					}
					else
					{
						glDisable(GL_MULTISAMPLE);
				checkGlError("");
					}
					currentMultiSampleEnable = state.multiSample;
				}
#if defined(LINUX)
				if (state.scissor != currentScissorEnable)
#endif
				{
					if (state.scissor)
					{
						glEnable(GL_SCISSOR_TEST);
				checkGlError("");
					}
					else
					{
						glDisable(GL_SCISSOR_TEST);
				checkGlError("");
					}
					currentScissorEnable = state.scissor;
				}
				currentRasterizerState = rasterizerState;
			}
		}

		VertexFormatID RendererGL_1_1::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader)
		{
#if 1
    if (shader < 0)
    {
        STX_PRINT("Title: %s\n", m_title.c_str());
        STX_PRINT("RendererGL_1_1::addVertexFormatVrtl:848\n");
        STX_PRINT("ShaderID shader < 0\n");
        STX_PRINT("shd: %x\n", shader);
        STX_PRINT("vf: %x\n", vertexFormats.size());
        DBG_HALT;
    }
#endif
#if defined(USE_HLSL_SHADERS)
			VertexFormatGL vertexFormat;
			stx_memset(&vertexFormat, 0, sizeof(vertexFormat));
			unsigned int nGeneric  = 0;
			unsigned int nTexCoord = 0;

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				if (formatDesc[i].type == TYPE_VERTEX)
				{
					nGeneric = 1;
					break;
				}
			}

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;
				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					case TYPE_TANGENT:
					case TYPE_BINORMAL:
					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;
					nGeneric++;
					break;
					case TYPE_VERTEX:
					vertexFormat.vertex.stream = stream;
					vertexFormat.vertex.size   = formatDesc[i].size;
					vertexFormat.vertex.offset = vertexFormat.vertexSize[stream];
					vertexFormat.vertex.format = formatDesc[i].format;
					break;
					case TYPE_NORMAL:
					vertexFormat.normal.stream = stream;
					vertexFormat.normal.size   = formatDesc[i].size;
					vertexFormat.normal.offset = vertexFormat.vertexSize[stream];
					vertexFormat.normal.format = formatDesc[i].format;
					break;
					case TYPE_TEXCOORD:
					vertexFormat.texCoord[nTexCoord].stream = stream;
					vertexFormat.texCoord[nTexCoord].size	 = formatDesc[i].size;
					vertexFormat.texCoord[nTexCoord].offset = vertexFormat.vertexSize[stream];
					vertexFormat.texCoord[nTexCoord].format = formatDesc[i].format;
					nTexCoord++;
					break;
				}
				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
			}

			vertexFormat.maxGeneric = nGeneric;
			vertexFormat.maxTexCoord = nTexCoord;
			vertexFormats.push_back(vertexFormat);

			return vertexFormats.size()-1;
#else

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
					case TYPE_TANGENT:
					case TYPE_BINORMAL:

					case TYPE_VERTEX:
					case TYPE_NORMAL:
					case TYPE_TEXCOORD:
					if(formatDesc[i].type==TYPE_TEXCOORD) nTexCoord++;


					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;
					nGeneric++;
					break;
				}

				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
			}

			vertexFormat.shader=shader;

			vertexFormat.maxGeneric = nGeneric;

#ifdef USE_HLSL_SHADERS
			vertexFormat.maxTexCoord = nTexCoord;
#endif

			vertexFormats.push_back(vertexFormat);

			return vertexFormats.size()-1;

#endif

		}
GLenum usagesGL_1_1[] =
		{
			GL_STATIC_DRAW,
			GL_DYNAMIC_DRAW,
			GL_STREAM_DRAW,
		}
		;
const char* usagesGL_1_1_[] =
		{
			"GL_STATIC_DRAW",
			"GL_DYNAMIC_DRAW",
			"GL_STREAM_DRAW",
		}
		;
		VertexBufferID RendererGL_1_1::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID aold)//, const IDataProcessor* p)
		{
//LOG_FNLN_NONE;
//////
if(aold>-1)
{
//LOG_FNLN_NONE;
//////
	glDeleteBuffers(1,  &vertexBuffers[aold].vboVB);
				checkGlError("");
//LOG_FNLN_NONE;
//
//////
	vertexBuffers[aold].vboVB=0;
	glGenBuffersARB(1, &vertexBuffers[aold].vboVB);
				checkGlError("");
//LOG_FNLN_NONE;

	vertexBuffers[aold].size=size;
//LOG_FNLN_NONE;
//////
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffers[aold].vboVB);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[%d].vboVB:%d)\n", aold, vertexBuffers[aold].vboVB);
	}
//LOG_FNLN_NONE;

}
//LOG_FNLN_NONE;
//////
VertexBufferGL vb;
//LOG_FNLN_NONE;
if(aold==-1)
{
//LOG_FNLN_NONE;
	vb.size = size;
	GLuint vbb=0;
//////
//LOG_FNLN_NONE;
	glGenBuffersARB
//ARB
(1, &vbb);
				checkGlError("");
//LOG_FNLN_NONE;

	vb.vboVB=vbb;
	vertexBuffers.push_back(vb);
//LOG_FNLN_NONE;
//////

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbb);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ARRAY_BUFFER, vbb:%d)\n", vbb);
	}
//LOG_FNLN_NONE;

}
//////
//LOG_FNLN_NONE;
	if(data) {
//LOG_FNLN_NONE;
//////
vb.data=(BYTE*)data;
//LOG_FNLN_NONE;
glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, data, usagesGL_1_1[bufferAccess]);
				checkGlError("");
//LOG_FNLN_NONE;

}
//LOG_FNLN_NONE;
//////
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ARRAY_BUFFER, %d)\n", 0);
	}
//LOG_FNLN_NONE;

//////
/*
LOG_PRINT_NONE("vbb=%d\n",vbb);
LOG_PRINT_NONE("size=%d\n",size);
LOG_PRINT_NONE("data=%d\n",data);
*/


			if(aold==-1)
			{
//LOG_FNLN_NONE;

				//LOG_PRINT_NONE("vbid=%d\n",vertexBuffers.size()-1);
				return vertexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<vertexBuffers.size()))
{
//LOG_FNLN_NONE;
//LOG_PRINT_NONE("vbid=%d\n",old);
return aold;
}
#if 0
			else if(0)
			{
				VertexBufferID tmpVertexBuffers[MAX_VERTEXSTREAM];
				intptr tmpOffsets[MAX_VERTEXSTREAM];
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					tmpVertexBuffers[i]=currentVertexBuffers[i];
					tmpOffsets[i]=currentOffsets[i];
				}
				//int vf=currentVertexFormat;
				int vf=selectedVertexFormat;
				setVertexFormat(-1);
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					if(-1!=currentVertexBuffers[i])
					{
						glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentVBO = 0);
				checkGlError("");

						currentVertexBuffers[i] = -1;
					}
				}
				deleteVertexBuffer(old);//???
				vertexBuffers[old]=vb;
				setVertexFormat(vf);
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					setVertexBuffer(i,tmpVertexBuffers[i],tmpOffsets[i]);
				}
				return old;
			}
#endif
			else
{
//LOG_FNLN_NONE;
//LOG_PRINT_NONE("vbid=%d\n",-1);

			//DBG_HALT;
			return -1;
}
//LOG_FNLN_NONE;
		}
BufferID RendererGL_1_1::addBufferVrtl(const STX_BUFFER_DESC* pDesc, const STX_SUBRESOURCE_DATA* pInitialData)
{
    LOG_FNLN;
    LOG_PRINT("IMPLEMENT!\n");
    return -1;
}
		IndexBufferID RendererGL_1_1::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID aold)//, const IDataProcessor* p)
		{
			unsigned int size = nIndices * indexSize;
//////
if(aold>-1)
{
//////
	glDeleteBuffers(1,  &indexBuffers[aold].vboIB);
				checkGlError("");
//
//////
	indexBuffers[aold].vboIB=0;
	glGenBuffersARB(1, &indexBuffers[aold].vboIB);
				checkGlError("");

//////
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffers[aold].vboIB);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[%d].vboIB:%d)\n", aold, indexBuffers[aold].vboIB);
	}

//////
	indexBuffers[aold].nIndices = nIndices;
	indexBuffers[aold].indexSize = indexSize;
}
//////
	IndexBufferGL ib;
if(aold==-1)
{
	GLuint vbb=0;
	ib.nIndices = nIndices;
	ib.indexSize = indexSize;
//////
	glGenBuffersARB(1, &vbb);
				checkGlError("");

	ib.vboIB=vbb;
//////
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbb);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, %vbb:%d)\n", vbb);
	}

//////
	indexBuffers.push_back(ib);
}
//////
//if(old>-1)
	//indexBuffers[old].size=size;

	if(data) {
//////
ib.data=(BYTE*)data;
#if 0
printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
printf("size=%d\n", size);
printf("data=%x\n", data);
printf("usagesGL_1_1_[%d]=%s\n", bufferAccess, usagesGL_1_1_[bufferAccess]);
#endif
glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size, data, usagesGL_1_1[bufferAccess]);
				checkGlError("");

}
//////
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				checkGlError("");

	if(0)//(m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, %d)\n", 0);
	}

//////
			if(aold==-1)
			{

				//LOG_PRINT_NONE("vbid=%d\n",vertexBuffers.size()-1);
				return indexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<indexBuffers.size()))
{
return aold;
}
else
	return -1;
/*

			glGenBuffersARB(1, &ib.vboIB);
				checkGlError("");
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ib.vboIB);
				checkGlError("");
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size, data, usagesGL_1_1[bufferAccess]);
				checkGlError("");
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				checkGlError("");
			if(old==-1)
			{
				indexBuffers.push_back(ib);
				return indexBuffers.size()-1;
			}
			else
			{
				deleteIndexBuffer(old);
				indexBuffers[old]=ib;
				return old;
			}
			return -1;
*/
		}
		GLint minFilters[] =
		{
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
		}
		;
		SamplerStateID RendererGL_1_1::addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color)
		{
			if (compareFunc != 0 || border_color != 0) return SS_NONE;
			SamplerStateGL samplerState;
			samplerState.minFilter = minFilters[filter];
			samplerState.magFilter = (filter == NEAREST)? GL_NEAREST : GL_LINEAR;
			samplerState.wrapS = (s == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapT = (t == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapR = (r == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.aniso = hasAniso(filter)? min((unsigned int) maxAnisotropic, maxAniso) : 1;
			samplerState.lod = lod;
			samplerStates.push_back(samplerState);
			return samplerStates.size()-1;
		}
		DepthStateID RendererGL_1_1::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest,
		const uint8 stencilMask, const int stencilFunc, const int stencilFail, const int depthFail, const int stencilPass)
		{


			//LOG_FNLN_NONE;
/*			
LOG_PRINT_NONE("depthStates.size()=%x", depthStates.size());
if(depthTest) LOG_PRINT_NONE("depthTest true;");
else LOG_PRINT_NONE("depthTest false;");
if(depthWrite) LOG_PRINT_NONE("depthWrite true;");
else LOG_PRINT_NONE("depthWrite false;");
*/

			DepthStateGL depthState;
			depthState.depthTest  = depthTest;
			depthState.depthWrite = depthWrite;
			depthState.depthFunc  = depthFunc;
			depthStates.push_back(depthState);
			return depthStates.size()-1;
		}
		RasterizerStateID RendererGL_1_1::addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor)
		{
			RasterizerStateGL rasterizerState;
			rasterizerState.cullMode = cullMode;
			rasterizerState.fillMode = fillMode;
			rasterizerState.multiSample = multiSample;
			rasterizerState.scissor = scissor;
			rasterizerStates.push_back(rasterizerState);
			return rasterizerStates.size()-1;
		}
		static const GLint internalFormats[] =
		{

			0,
			GL_INTENSITY8,
			GL_LUMINANCE8_ALPHA8,
			GL_RGB8,
			GL_RGBA8,
			GL_INTENSITY16,
			GL_LUMINANCE16_ALPHA16,
			GL_RGB16,
			GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			GL_INTENSITY_FLOAT16_ATI,
			GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			GL_RGB_FLOAT16_ATI,
			GL_RGBA_FLOAT16_ATI,
			GL_INTENSITY_FLOAT32_ATI,
			GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			GL_RGB_FLOAT32_ATI,
			GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, GL_RGB5,
			GL_RGBA4,
			GL_RGB10_A2,
			GL_DEPTH_COMPONENT16,
			GL_DEPTH_COMPONENT24,
			GL_DEPTH24_STENCIL8_EXT,
			0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI,
#else
0,
#endif
		}
		;
		static const GLenum srcFormats[] =
		{
			0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes[] =
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
			0, 0, 0, GL_UNSIGNED_SHORT_5_6_5,
			GL_UNSIGNED_SHORT_4_4_4_4_REV,
			GL_UNSIGNED_INT_2_10_10_10_REV,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
			GL_UNSIGNED_INT_24_8_EXT,
			0, 0,
			0,
			0,
			0,
			0,
		}
		;

		static const char* internalFormats_[] =
		{

			0,
			"GL_INTENSITY8",
			"GL_LUMINANCE8_ALPHA8",
			"GL_RGB8",
			"GL_RGBA8",
			"GL_INTENSITY16",
			"GL_LUMINANCE16_ALPHA16",
			"GL_RGB16",
			"GL_RGBA16",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			"GL_INTENSITY_FLOAT16_ATI",
			"GL_LUMINANCE_ALPHA_FLOAT16_ATI",
			"GL_RGB_FLOAT16_ATI",
			"GL_RGBA_FLOAT16_ATI",
			"GL_INTENSITY_FLOAT32_ATI",
			"GL_LUMINANCE_ALPHA_FLOAT32_ATI",
			"GL_RGB_FLOAT32_ATI",
			"GL_RGBA_FLOAT32_ATI",
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "GL_RGB5",
			"GL_RGBA4",
			"GL_RGB10_A2",
			"GL_DEPTH_COMPONENT16",
			"GL_DEPTH_COMPONENT24",
			"GL_DEPTH24_STENCIL8_EXT",
			0, "GL_COMPRESSED_RGB_S3TC_DXT1_EXT",
			"GL_COMPRESSED_RGBA_S3TC_DXT3_EXT",
			"GL_COMPRESSED_RGBA_S3TC_DXT5_EXT",
			0,
#ifdef _MSC_VER
"GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI",
#else
0,
#endif
		}
		;
		static const char* srcFormats_[] =
		{
			0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
		}
		;
		static const char* srcTypes_[] =
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
			0, 0, 0, "GL_UNSIGNED_SHORT_5_6_5",
			"GL_UNSIGNED_SHORT_4_4_4_4_REV",
			"GL_UNSIGNED_INT_2_10_10_10_REV",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_INT",
			"GL_UNSIGNED_INT_24_8_EXT",
			0, 0,
			0,
			0,
			0,
			0,
		}
		;



		void RendererGL_1_1::setSamplerState(const char *samplerName, const SamplerStateID samplerState)
		{
		}
		void RendererGL_1_1::applySamplerStates()
		{
		}


int RendererGL_1_1::GetTextureWidth(TextureID id)
{
	if(id<textures.size())
		return textures[id].width;
	return 0;
}
int RendererGL_1_1::GetTextureHeight(TextureID id)
{
	if(id<textures.size())
		return textures[id].height;
	return 0;
}
#if 1

// Bits 26-31 reserved for destination format specification.
#define FMT(x) (x << 26)
#define GETFMT(flags) (FORMAT) (flags >> 26)
#define GL_RG_ATI                           0x883C
#define GL_TEXTURE_RECTANGLE_EXT            0x84F5
bool GL_ATI_texture_compression_3dc_Flag = false;
//GL_ATI_texture_compression_3dc_Flag = isExtensionSupported("GL_ATI_texture_compression_3dc");
bool GL_EXT_texture_compression_s3tc_Flag   = false;
//GL_EXT_texture_compression_s3tc_Flag   = isExtensionSupported("GL_EXT_texture_compression_s3tc");
bool GL_ARB_texture_compression_Flag  = false;

#if 1
#define checkGlError { \
    GLint errGL = glGetError(); \
    if (errGL != GL_NO_ERROR) { \
        STX_PRINT("GL error:\nfile=%s:\nfn=%s:\nline=%d:\n 0x%08x\n", __FILE__, __FUNCTION__, __LINE__, errGL); \
    } \
}

#define glerror { \
    GLint errGL = glGetError(); \
    if (errGL != GL_NO_ERROR) { \
        STX_PRINT("GL error:\nfile=%s:\nfn=%s:\nline=%d:\n 0x%08x\n", __FILE__, __FUNCTION__, __LINE__, errGL); \
    } \
}
#endif
		TextureID RendererGL_1_1::addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
		{
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
	bool m_bGL_TEXTURE_RECTANGLE_EXT=false;
	
if(0)
{
	STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	ubyte* p=img->getPixels();
	STX_PRINT("p[0]:%x\n", *(p+0));
	STX_PRINT("p[1]:%x\n", *(p+1));
	STX_PRINT("p[2]:%x\n", *(p+2));
	STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
}

if(flags!=0xFFFF)
{
	// Turn a 2D texture formatted as a cross into a proper cubemap if requested
	if (img->is2D() && (flags & CUBEMAP)) img->crossToCube();
	if(!m_bLuaScript)
	{
		
		std::string GL_TEXTURE_RECTANGLE_EXT_ = "False";
		#if 0//ndef USEXML
		GL_TEXTURE_RECTANGLE_EXT_ = _IRenderer_script.get<std::string>("GL_TEXTURE_RECTANGLE_EXT");
		#elif 0
		if(_IRenderer_BodyElement)
			GL_TEXTURE_RECTANGLE_EXT_ = _IRenderer_BodyElement->FirstChild("GL_TEXTURE_RECTANGLE_EXT")->ToElement()->Attribute("Value");
		#endif
		if(GL_TEXTURE_RECTANGLE_EXT_=="True")
			m_bGL_TEXTURE_RECTANGLE_EXT=true;
	}
	if(m_bGL_TEXTURE_RECTANGLE_EXT)
	{
	if (flags & RECTANGLE){
		// Texture rectangles doesn't support mipmaps or repeat mode
		if (img->getMipMapCount()) return TEXTURE_NONE;
		flags |= (CLAMP_S | CLAMP_T);
	}
	}

	// Generate mipmaps if they are missing and the filter requires mipmaps
	if (img->getMipMapCount() <= 1 && img->getMipMapCount()){
		if (isPlainFormat(img->getFormat()))
		if (!img->generateMipMaps()){
			LOG_PRINT_NONE("Couldn't generate mipmaps\n");
			return TEXTURE_NONE;
		}
	}
}
	// Turn a 2D texture into a normal map if requested
	FORMAT destFormat = GETFMT(flags);
if(flags!=0xFFFF)
{
	if (img->is2D() && (flags & NORMALMAP)){
		if (!img->toNormalMap(destFormat? destFormat : FORMAT_RGBA8)){
			LOG_PRINT_NONE("Couldn't generate normalmap\n");
			return TEXTURE_NONE;
		}
	}

	// Not using GL_CLAMP_TO_EDGE with cubemaps makes little sense
	if (img->isCube()) flags |= (CLAMP_S | CLAMP_T);

	// If the target hardware doesn't support the compressed texture format, just decompress it to a compatible format
}
	FORMAT format = img->getFormat();
if(flags!=0xFFFF)
{
	if (isCompressedFormat(format)){
		if (!GL_ARB_texture_compression_Flag || (format <= FORMAT_DXT5 && !GL_EXT_texture_compression_s3tc_Flag) || (format == FORMAT_ATI2N && !GL_ATI_texture_compression_3dc_Flag)){
			img->decompress();
			format = img->getFormat();
		}
	}
}
	tex.flags  = flags;
	tex.width  = img->getWidth();
	tex.height = img->getHeight();
	// tex.name = "XXX";//samplerStates[samplerState].name;

	if(m_bGL_TEXTURE_RECTANGLE_EXT)
		tex.glTarget = img->isCube()? GL_TEXTURE_CUBE_MAP : img->is3D()? GL_TEXTURE_3D : img->is2D()? (flags & RECTANGLE? GL_TEXTURE_RECTANGLE_EXT : GL_TEXTURE_2D) : GL_TEXTURE_1D;
	else
		tex.glTarget = img->isCube()? GL_TEXTURE_CUBE_MAP : img->is3D()? GL_TEXTURE_3D : img->is2D()? GL_TEXTURE_2D : GL_TEXTURE_1D;
	// Generate a texture
	glGenTextures(1, &tex.glTexID);
	glBindTexture(tex.glTarget, tex.glTexID);

	// Upload it all
	GLenum srcFormat, srcType = srcTypes[format];
	if (isDepthFormat(format)){
		srcFormat = GL_DEPTH_COMPONENT;
	} else if (format == FORMAT_R16f || format == FORMAT_R32f){
		srcFormat = GL_RED;
	} else if (format == FORMAT_RG16f || format == FORMAT_RG32f){
		srcFormat = GL_RG_ATI;
	} else {
		srcFormat = srcFormats[getChannelCount(format)];
	}
	GLint internalFormat = internalFormats[destFormat? destFormat : format];

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

	// ??? setupTextureFilter(tex, filter, flags);

	ubyte *src;
	int mipMapLevel = 0;

	bool m_bTexImage=false;
if(flags!=0xFFFF)
{
	if(!m_bLuaScript)
	{
		std::string TexImage = "False";
		#if 0//ndef USEXML
		TexImage = _IRenderer_script.get<std::string>("TexImage");
		#elif 0
		if(_IRenderer_BodyElement)
			TexImage = _IRenderer_BodyElement->FirstChild("TexImage")->ToElement()->Attribute("Value");
		#endif
		if(TexImage=="True")
			m_bTexImage=true;
	}
}

if(0)
{
	STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	ubyte* p=img->getPixels();
	STX_PRINT("p[0]:%x\n", *(p+0));
	STX_PRINT("p[1]:%x\n", *(p+1));
	STX_PRINT("p[2]:%x\n", *(p+2));
	STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
}

if(0)
{ /*
	GLuint _textureID = 0;

    glGenTextures( 1, & _textureID );
    glBindTexture( GL_TEXTURE_2D, _textureID );
*/
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, img->getWidth(), img->getHeight(), 
                  0, GL_RGB, GL_UNSIGNED_BYTE, img->getPixels() );
	//STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			return textures.add(tex);
}
#if 0
	STX_PRINT("srcTypes_[%x]=%s\n", format, srcTypes_[format]);
	STX_PRINT("srcFormats_[%x]=%s\n", format, srcFormats_[getChannelCount(format)]);
	STX_PRINT("getChannelCount(%x)=%d\n", format, getChannelCount(format));
	STX_PRINT("internalFormats_[%x]=%s\n", (destFormat? destFormat : format), internalFormats_[destFormat? destFormat : format]);
#endif

if(!m_bTexImage)
{
	while ((src = img->getPixels(mipMapLevel)) != 0){
		if (img->isCube()){
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++){
				if (isCompressedFormat(format)){
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, src + i * size);
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
				}
			}
		} else if (img->is3D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);}
			else{
				glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, src);}
		} else if (img->is2D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
	//STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			} else {
				glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src);
	//STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			}
		} else {
			glTexImage1D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, src);
		}
		mipMapLevel++;
		break;
	}
} else {
	while ((src = img->getPixels(mipMapLevel)) != 0){
	#if 0
	STX_PRINT("img->getPixels(%d)=%x\n", mipMapLevel, img->getPixels(mipMapLevel));
	STX_PRINT("img->getWidth(%d)=%x\n", mipMapLevel, img->getWidth(mipMapLevel));
	STX_PRINT("img->getHeight(%d)=%x\n", mipMapLevel, img->getHeight(mipMapLevel));
	#endif
		if (img->isCube()){
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++){
				if (isCompressedFormat(format)){
					glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, src + i * size);
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
				}
			}
		} else if (img->is3D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage3DARB(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
			} else {
				glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, src);
			}
		} else if (img->is2D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage2DARB(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
	//STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			} else {
				glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src);
	//STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
			}
		} else {
			glTexImage1D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, src);
		}
		mipMapLevel++;
		break;
	}
}
#if 0
	TextureID thisTex = insertTexture(tex);
	TextureID currTex = currentTextures[currentImageUnit];
	if (currTex != TEXTURE_NONE && (textures[currTex].glTarget == tex.glTarget)){
		// Restore texture binding
		//glBindTexture(textures[currTex].glTarget, textures[currTex].glTexID);
		currentTextures[currentImageUnit] = thisTex;
	}
	return thisTex;
#else
	LOG_PRINT("tex.glTexID=%x\n", tex.glTexID);
			return textures.add(tex);
#endif
}
#endif
		TextureID RendererGL_1_1::addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
		{
			if (isCompressedFormat(format) || arraySize > 1) return false;
			if (mipMapCount > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = (flags & CUBEMAP)? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
			tex.flags  = flags;
			tex.format = format;
			glGenTextures(1, &tex.glTexID);
				checkGlError("");
			glBindTexture(tex.glTarget, tex.glTexID);
				checkGlError("");
			if((format>=FORMAT_R16F) && (FORMAT_RGBA32F<=format))
				setupSampler(tex.glTarget, samplerStates[samplerState], true);
			else
				setupSampler(tex.glTarget, samplerStates[samplerState]);
			tex.samplerState = samplerState;
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);

			return rt;
		}
		TextureID RendererGL_1_1::addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
		{
			if (arraySize > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = GL_RENDERBUFFER_EXT;
			tex.format = format;
			glGenRenderbuffersEXT(1, &tex.glDepthID);
				checkGlError("");
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			return rt;
		}
		void RendererGL_1_1::setRenderTargetSize(const TextureID renderTarget, const int width, const int height)
		{
			(textures[renderTarget]).width	= width;
			(textures[renderTarget]).height = height;
			if ((textures[renderTarget]).glTarget == GL_RENDERBUFFER_EXT)
			{
				FORMAT format = (textures[renderTarget]).format;
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, (textures[renderTarget]).glDepthID);
				checkGlError("");
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormats[format], width, height);
				checkGlError("");
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
				checkGlError("");
			}
			else
			{
				FORMAT format = (textures[renderTarget]).format;
				GLint internalFormat = internalFormats[format];
				GLenum srcFormat = srcFormats[getChannelCount(format)];
				GLenum srcType = srcTypes[format];
				if (isDepthFormat(format))
				{
					if (isStencilFormat(format))
					{

						srcFormat = GL_DEPTH_STENCIL_EXT;
					}
					else
					{
						srcFormat = GL_DEPTH_COMPONENT;


					}
				}

				if (isFloatFormat(format)) srcType = GL_FLOAT;
				glBindTexture((textures[renderTarget]).glTarget, (textures[renderTarget]).glTexID);
				checkGlError("");
				if ((textures[renderTarget]).flags & CUBEMAP)
				{
					for (int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; i++)
					glTexImage2D(i, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
				checkGlError("");
				}
				else
				{
					glTexImage2D((textures[renderTarget]).glTarget, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
				checkGlError("");
				}
			}
		}
		bool RendererGL_1_1::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
		{
			if (depth > 1 || arraySize > 1 || mipMapCount > 1) return false;
			if (width != (textures[renderTarget]).width || height != (textures[renderTarget]).height)
			{
				setRenderTargetSize(renderTarget, width, height);
			}
			return true;
		}
		bool RendererGL_1_1::generateMipMaps(const TextureID renderTarget)
		{
			return false;
		}
		void RendererGL_1_1::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices)
		{
			//return; // ???
			//apply(); // ???
			if (fbo == 0)
			{
				glGenFramebuffersEXT(1, &fbo);
				checkGlError("");
			}
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
				checkGlError("");
			return; // ???
			for (unsigned int i = 0; i < nRenderTargets; i++)
			{
				TextureID colorRT = colorRTs[i];
				if ((textures[colorRT]).flags & CUBEMAP)
				{
#if defined(LINUX)
					if (colorRT != currentColorRT[i] || currentColorRTSlice[i] != slices[i])
#endif
					{
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + slices[i], (textures[colorRT]).glTexID, 0);
				checkGlError("");
						currentColorRTSlice[i] = slices[i];
					}
				}
				else
				{
#if defined(LINUX)
					if (colorRT != currentColorRT[i])
#endif
					{
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, (textures[colorRT]).glTexID, 0);
				checkGlError("");
						currentColorRT[i] = colorRT;
					}
				}
			}
			if (nRenderTargets != nCurrentRenderTargets)
			{
				for (unsigned int i = nRenderTargets; i < nCurrentRenderTargets; i++)
				{
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0, 0);
				checkGlError("");
					currentColorRT[i] = TEXTURE_NONE;
					currentColorRTSlice[i] = NO_SLICE;
				}
				if (nRenderTargets == 0)
				{				
					#if 1//ndef __APPLE__
					glDrawBuffer(GL_NONE);
				checkGlError("");
					glReadBuffer(GL_NONE);
				checkGlError("");
					#endif
				}
				else
				{				
					#if 1//ndef __APPLE__
					glDrawBuffersARB(nRenderTargets, drawBuffers);
				checkGlError("");
					glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
				checkGlError("");
					#endif
				}
				nCurrentRenderTargets = nRenderTargets;
			}
#if defined(LINUX)
			if (depthRT != currentDepthRT)
#endif
			{
				if (depthRT >= 0 && (textures[depthRT]).glTarget != GL_RENDERBUFFER_EXT)
				{
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
				checkGlError("");
					if (isStencilFormat((textures[depthRT]).format))
					{
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
				checkGlError("");
					}
					else
					{
						glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
				checkGlError("");
					}
				}
				else
				{
					glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, (depthRT < 0)? 0 : (textures[depthRT]).glTexID);
				checkGlError("");
					if (depthRT >= 0 && isStencilFormat((textures[depthRT]).format))
					{
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, (textures[depthRT]).glTexID);
				checkGlError("");
					}
					else
					{
						glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
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
				glViewport(0, 0, (textures[colorRTs[0]]).width, (textures[colorRTs[0]]).height);
				checkGlError("");
			}
			else if(depthRT >= 0)
			{
				changeFrontFace(GL_CW);
				glViewport(0, 0, (textures[depthRT]).width, (textures[depthRT]).height);
				checkGlError("");
			}
			reset(0);
		}
		void RendererGL_1_1::changeToMainFramebuffer()
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				checkGlError("");
			return; // ???
			glViewport(m_viewport.X,m_viewport.Y, m_viewport.Width, m_viewport.Height);
				checkGlError("");
			changeFrontFace(GL_CW);
		}
		void RendererGL_1_1::Init0(void)
		{
	if(defaultFont == -1)
	{
		//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
	}

					}

UINT32 RendererGL_1_1::GetAvailableTextureMem( )
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

		int RendererGL_1_1::removeTexture(const TextureID texture)
		{
			if(texture>=textures.size()) return -1;
			if ((textures[texture]).glTarget)
			{
				if ((textures[texture]).glTarget == GL_RENDERBUFFER_EXT)
				{
					glDeleteRenderbuffersEXT(1, &(textures[texture]).glDepthID);
				checkGlError("");
				}
				else
				{
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

		int RendererGL_1_1::deleteVertexBuffer(const VertexBufferID vertexBuffer)
		{
			if(vertexBuffer>=vertexBuffers.size()) return -1;
			if ((vertexBuffers[vertexBuffer]).vboVB)
			{
				glDeleteBuffersARB(1, &(vertexBuffers[vertexBuffer]).vboVB);
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
		int RendererGL_1_1::deleteIndexBuffer(const IndexBufferID indexBuffer)
		{
			if(indexBuffer>=indexBuffers.size()) return -1;
			if ((indexBuffers[indexBuffer]).vboIB)
			{
				glDeleteBuffersARB(1, &(indexBuffers[indexBuffer]).vboIB);
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
int RendererGL_1_1::texturePitchVrtl(const TextureID id,int level){return 0;}
BYTE *RendererGL_1_1::lockTexture(const TextureID id,int level, int& pitch){return 0;}
BYTE *RendererGL_1_1::lockVertexBuffer(const VertexBufferID id){
		return 0;}
BYTE *RendererGL_1_1::lockIndexBuffer(const IndexBufferID id){
		return 0;}
BYTE *RendererGL_1_1::unlockTextureVrtl(const TextureID id,int level){return 0;}
BYTE *RendererGL_1_1::unlockVertexBuffer(const VertexBufferID id){
		return 0;}
BYTE *RendererGL_1_1::unlockIndexBuffer(const IndexBufferID id){
		return 0;}

#if 0
HRESULT RendererGL_1_1::ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return textureProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGL_1_1::LockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGL_1_1::opyToResourceTexture(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGL_1_1::UnLockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererGL_1_1::ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return vertexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGL_1_1::LockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGL_1_1::opyToResourceVertexBuffer(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGL_1_1::UnLockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererGL_1_1::ProcessIndexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return indexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererGL_1_1::LockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererGL_1_1::opyToResourceIndexBuffer(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererGL_1_1::UnLockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].UnLockDeviceObject();
}

ProcessorID RendererGL_1_1::addTextureProcessor( TextureID& ppTexture, int g_SkipMips )
	{
		static int id=-1;
			CTextureProcessorGL tp(this,ppTexture);//,g_pResourceReuseCache, g_SkipMips );

			id=textureProcessors.add(tp);
		return id;
	}

ProcessorID RendererGL_1_1::addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData)

	{
		static int id=-1;
			CVertexBufferProcessorGL tl(this,ppBuffer);//, iSizeBytes, pData);
			id=vertexBufferProcessors.add(tl);
		return id;
	}

ProcessorID RendererGL_1_1::addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData)
	{
		static int id=-1;
			CIndexBufferProcessorGL tl(this,ppBuffer);//, iSizeBytes,4/*???*/, pData);
			id=indexBufferProcessors.add(tl);
		return id;
	}

#endif
void RendererGL_1_1::viewport(int x, int y, int viewportWidth, int viewportHeight){
        //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glViewport(x, y, viewportWidth, viewportHeight);
       // changeFrontFace(GL_CW);
  m_viewport.X=x;
  m_viewport.Y=y;
  m_viewport.Width=viewportWidth;
  m_viewport.Height=viewportHeight;

}
#if 0

void	RendererGL_1_1::CreateTextureFromFile_Serial(  CHAR* szFileName,
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
    if( FAILED( RendererGL_1_1::HLSLCg::LoadTexture(pLoader) ) )
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

void	RendererGL_1_1::CreateVertexBuffer_Serial(
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

void	RendererGL_1_1::CreateIndexBuffer_Serial(
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
float	RendererGL_1_1::GetMaxPointSpriteSize()
{
//
    // If you want to know the std::max size that a point sprite can be set
    // to, do this.
	//

    // Query for the std::max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
				checkGlError("");
    glPointSize( maxSize );
				checkGlError("");
	return maxSize;
}

void	RendererGL_1_1::BeginPointSpriteRendering()
{
	//
	// Enabling GL_DEPTH_TEST and setting glDepthMask to GL_FALSE makes the
    // Z-Buffer read-only, which helps remove graphical artifacts generated
    // from  rendering a list of particles that haven't been sorted by
    // distance to the eye.
	//
    // Enabling GL_BLEND and setting glBlendFunc to GL_DST_ALPHA with GL_ONE
    // allows particles, which overlap, to alpha blend with each other
    // correctly.
	//

    glEnable( GL_DEPTH_TEST );
				checkGlError("");
    glDepthMask( GL_FALSE );
				checkGlError("");


	glEnable( GL_BLEND );
				checkGlError("");
    glBlendFunc( GL_DST_ALPHA, GL_ONE );
				checkGlError("");

    //
	// Set up the OpenGL state machine for using point sprites...
	//

    // This is how will our point sprite's size will be modified by
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
				checkGlError("");

    // Query for the std::max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
				checkGlError("");

    // Clamp size to 100.0f or the sprites could get a little too big on some
    // of the newer graphic cards. My ATI card at home supports a std::max point
    // size of 1024.0f!
    if( maxSize > 100.0f )
        maxSize = 100.0f;

    glPointSize( maxSize );//in OTL 10 ???
				checkGlError("");

    // The alpha of a point is calculated to allow the fading of points
    // instead of shrinking them past a defined threshold size. The threshold
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );
				checkGlError("");

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
				checkGlError("");
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );
				checkGlError("");

    // Specify point sprite texture coordinate replacement mode for each
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
				checkGlError("");

    //
	// Render point sprites...
	//

    glEnable( GL_POINT_SPRITE_ARB );
				checkGlError("");
}

void	RendererGL_1_1::EndPointSpriteRendering()
{
    //
    // Reset OpenGL states...
	//

    glDepthMask( GL_TRUE );
				checkGlError("");
    glDisable( GL_BLEND );
				checkGlError("");

	glDisable( GL_POINT_SPRITE_ARB );
				checkGlError("");
}
    BYTE* RendererGL_1_1::LockTexture(TextureID id, int m, int& pitch)
    {
		return textures[id].Lock(m,pitch);
	}
    void RendererGL_1_1::UnlockTexture(TextureID id, int m)
    {
		textures[id].Unlock(m);
	}
    BYTE* RendererGL_1_1::LockVertexBuffer(VertexBufferID id)
    {
		
		return vertexBuffers[id].Lock();
	}
    void RendererGL_1_1::UnLockVertexBuffer(VertexBufferID id)
    {
		
		vertexBuffers[id].Unlock();
	}
    BYTE* RendererGL_1_1::LockIndexBuffer(IndexBufferID id)
    {
		//
		return indexBuffers[id].Lock();
	}
    void RendererGL_1_1::UnLockIndexBuffer(IndexBufferID id)
    {
		//
		indexBuffers[id].Unlock();
	}

	void RendererGL_1_1::GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc)
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
	unsigned int RendererGL_1_1::GetLevelCount(TextureID id)
	{
		return textures[id].mipMapLevels;
	}
	void RendererGL_1_1::GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc)
	{
		sDesc->Size=vertexBuffers[id].size;
	}
	void RendererGL_1_1::GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc)
	{
		sDesc->indexSize=indexBuffers[id].indexSize;
		sDesc->nIndices=indexBuffers[id].nIndices;
		sDesc->Format=indexBuffers[id].indexSize;
	}

		void RendererGL_1_1::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask)
        {
#if defined(LINUX)
        if (blendState != currentBlendState)
#endif
{
            if (blendState == BS_NONE || !blendStates[blendState].blendEnable){
                if (currentBlendEnable){
                    glDisable(GL_BLEND);
				checkGlError("");
                    currentBlendEnable = false;
                }
            } else {
                if (blendStates[blendState].blendEnable){
                    if (!currentBlendEnable){
                        glEnable(GL_BLEND);
				checkGlError("");
                        currentBlendEnable = true;
                    }

#if defined(LINUX)
                    if (blendStates[blendState].srcFactorRGB   != currentSrcFactorRGB ||
                        blendStates[blendState].dstFactorRGB   != currentDstFactorRGB ||
                        blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha ||
                        blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha)

#endif
			{

                        glBlendFuncSeparate(
                            currentSrcFactorRGB   = blendStates[blendState].srcFactorRGB,
                            currentDstFactorRGB   = blendStates[blendState].dstFactorRGB,
                            currentSrcFactorAlpha = blendStates[blendState].srcFactorAlpha,
                            currentDstFactorAlpha = blendStates[blendState].dstFactorAlpha);
				checkGlError("");
                    }

#if defined(LINUX)
                    if (blendStates[blendState].blendModeRGB   != currentBlendModeRGB ||
                        blendStates[blendState].blendModeAlpha != currentBlendModeAlpha)
#endif
{
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

#if defined(LINUX)
            if (mask != currentMask)
#endif
{
                glColorMask((mask & RED) & 1, ((mask & GREEN) >> 1) & 1, ((mask & BLUE) >> 2) & 1, ((mask & ALPHA) >> 3) & 1);
				checkGlError("");

                currentMask = mask;
            }

#if defined(LINUX)
            if (alphaToCoverage != currentAlphaToCoverageEnable)
#endif
            {
                if (alphaToCoverage)
                    {glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				checkGlError("");}
                else
                    {glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				checkGlError("");}

                currentAlphaToCoverageEnable = alphaToCoverage;
            }

            currentBlendState = blendState;
        }
    }

void RendererGL_1_1::changeAlphaState(const AlphaStateID blendState)
{
#if defined(LINUX)
	if (blendState != currentAlphaState)
#endif
	{
		if (blendState == -1 || !(alphaStates[blendState]).blendEnable)
		{
			if (currentAlphaEnable)
			{
				glDisable(GL_ALPHA_TEST);
				checkGlError("");
    			currentAlphaEnable = false;
			}
		}
		else
		{
			if ((alphaStates[blendState]).blendEnable)
			{
				if (!currentAlphaEnable)
				{
					glEnable(GL_ALPHA_TEST);
				checkGlError("");
    				currentAlphaEnable = true;
    //  reference = clamp(referenceArg, 0.0f, 1.0f);
    glAlphaFunc(GL_GEQUAL, 0.5f);//???

				}
			}
		}
		currentAlphaState = blendState;

}
}

		BlendStateID RendererGL_1_1::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage)
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

AlphaStateID RendererGL_1_1::addAlphaState(
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
float RendererGL_1_1::getTextWidth(const FontID font, const char *str, int length) const {
	if (font < 0) return 0;
    if (length < 0) length = (int) stx_strlen(str);

	Character *chars = fonts[font].chars;

	float len = 0;
	for (int i = 0; i < length; i++){
		len += chars[(unsigned char) str[i]].ratio;
	}

	return len;
}

unsigned int RendererGL_1_1::drawArrays(const Primitives PrimitiveType, const int firstVertex, const int nVertices)
{
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	if(!(selectedVertexBuffers[0]>-1))
		return 0;
	apply();

	if(1){
	{
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("glDrawArrays:\n");
		printf("PrimitiveType=%x\n", PrimitiveType);
		printf("glPrimRendererGL_1_1[%d]=%x\n", PrimitiveType, glPrimRendererGL_1_1[PrimitiveType]);
		printf("firstVertex=%d\n", firstVertex);
		printf("nVertices=%d\n", nVertices);
	}}
	LOG_PRINT("glDrawArrays\n");
	glDrawArrays(glPrimRendererGL_1_1[PrimitiveType], firstVertex, nVertices);
				checkGlError("");
	//disApplyTextures();
	reset();
	nDrawCalls++;
	return 0;
}

unsigned int RendererGL_1_1::drawElements(const Primitives PrimitiveType, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
		return 0;
	apply();
	unsigned int indexSize = indexBuffers[selectedIndexBuffer].indexSize;

	if(1){
	{
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("glDrawElements:\n");
		printf("PrimitiveType=%x\n", PrimitiveType);
		printf("glPrimRendererGL_1_1[PrimitiveType]=%x\n", glPrimRendererGL_1_1[PrimitiveType]);
		printf("nIndices=%d\n", nIndices);
		printf("indexSize=%d\n", indexSize);
		printf("firstIndex=%d\n", firstIndex);
		printf("BUFFER_OFFSET(indexSize * firstIndex)=%d\n", BUFFER_OFFSET(indexSize * firstIndex));
	}}

	LOG_PRINT("glDrawElements\n");
	glDrawElements(glPrimRendererGL_1_1[PrimitiveType], nIndices, indexSize == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, BUFFER_OFFSET(indexSize * firstIndex));
	checkGlError("");
	//disApplyTextures();
	reset();
	nDrawCalls++;
	return 0;
}


		void RendererGL_1_1::changeIndexBuffer(const IndexBufferID indexBuffer)
		{
#ifdef __LOG_glDrawElements__
	LOG_FNLN;
#endif	
	if (indexBuffer == IB_NONE)
		return;
#if 0//defined(LINUX)
			if (indexBuffer != currentIndexBuffer)
#endif
			{

	if(1){
	if(1)//m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("indexBuffers.size()=%d\n", indexBuffers.size());
		LOG_PRINT("indexBuffer=%d\n", indexBuffer);
	}}
				if (indexBuffer == IB_NONE)
				{
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				checkGlError("");
				}
				else
				{
					if((indexBuffer>-1)&&(indexBuffers.size()>indexBuffer))
					{
						glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, (indexBuffers[indexBuffer]).vboIB);
						checkGlError("");
					}
				}
				currentIndexBuffer = indexBuffer;
			}
		}
unsigned int RendererGL_1_1::drawPrimitivesVrtl(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vsize,
				const ubyte* ibdata, const unsigned int isize)
{
	VertexFormatID vf=-1;
	if(currentVertexFormat>-1)
		vf=currentVertexFormat;
	else if(selectedVertexFormat>-1)
		vf=selectedVertexFormat;
	LOG_PRINT_NONE("vf=%x\n", vf);	
	apply();

	const intptr offset=0;
	char *base = (char *) vbdata;
	VertexFormatGL cvf = vertexFormats[vf];
	unsigned int stream=0;
	unsigned int vertexSize = cvf.vertexSize[stream];

			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			};
	//glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*)&g_vertices_[0] + vx_loc);
	glVertexPointer(cvf.vertex.size, glTypes[cvf.vertex.format], vertexSize, base + cvf.vertex.offset);
	checkGlError("",__FUNCTION__, __LINE__);
	//LOG_PRINT_NONE("glVertexPointer(%d, %x, %d, %x)\n", cvf.vertex.size, glTypes[cvf.vertex.format], vertexSize, base + cvf.vertex.offset);
	if(cvf.normal.size){
	//glNormalPointer(glTypes[cvf.normal.format], vertexSize, base + cvf.normal.offset);
	checkGlError("",__FUNCTION__, __LINE__);
	//LOG_PRINT_NONE("glNormalPointer(%x, %d, %x)\n", glTypes[cvf.normal.format], vertexSize, base + cvf.normal.offset);
	}
	unsigned int i=0;
	glTexCoordPointer(cvf.texCoord[i].size, glTypes[cvf.texCoord[i].format], vertexSize, base + cvf.texCoord[i].offset);
	//LOG_PRINT_NONE("glTexCoordPointer(%d, %x, %d, %x)\n", cvf.texCoord[i].size, glTypes[cvf.texCoord[i].format], vertexSize, base + cvf.texCoord[i].offset);
	checkGlError("",__FUNCTION__, __LINE__);
	if(ibdata)
	{
		if(isize==2)//((isize==2)? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT)
			glDrawElements(glPrimRendererGL_1_1[primitives], counti, GL_UNSIGNED_SHORT, (GLvoid*)(((unsigned short*)ibdata)+firsti));
		else
			glDrawElements(glPrimRendererGL_1_1[primitives], counti, GL_UNSIGNED_INT, (GLvoid*)(((unsigned int*)ibdata)+firsti));
	}
	else
		glDrawArrays(glPrimRendererGL_1_1[primitives], 0, countv);
						 //firstv, countv);
	reset(RESET_ALL);
	GLint err = glGetError();
	return 0;
}
#endif

