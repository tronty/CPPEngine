#if defined(GLES3_2) || defined(GLES2)
#ifndef __RendererGLES_H__
#define __RendererGLES_H__ 1

/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*
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

#include <Framework3/importgl.h>
#include <Framework3/RendererGLDS.h>
struct RendererGLES: public RendererHLSLCg
{
public:
#ifdef _MSC_VER
	union
	{
		HDC hdc;
		HDC hDc;
		HDC m_hDC;
	};
	union
	{
	HGLRC hGlrc;
	HGLRC hglrc;
};
#elif 0
	GLXContext ctx;
#endif
	virtual Texture* GetTexture(TextureID id)
	{
		if(id==-1)
			return 0;
		if(id>=textures.size())
			return 0;
		return &textures[id];
	}
	virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);


	virtual unsigned int drawPrimitivesVrtl(	const Primitives primitives,
						const unsigned int firstv, const unsigned int countv,
						const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize);

	virtual unsigned int GetLevelCount(TextureID id);
	virtual void GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc);
	virtual void GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc);
	virtual void GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc);

    BYTE* LockTexture(TextureID id, int m, int& pitch);
    void UnlockTexture(TextureID id, int m);
    BYTE* LockVertexBuffer(VertexBufferID id);
    void UnLockVertexBuffer(VertexBufferID id);
    BYTE* LockIndexBuffer(IndexBufferID id);
    void UnLockIndexBuffer(IndexBufferID id);
#if 0
	BYTE* GetVertexBufferProcessorData(ProcessorID ID)
	{return (BYTE *)vertexBufferProcessors[ID].m_pLockedData;}
	BYTE* GetIndexBufferProcessorData(ProcessorID ID)
	{return (BYTE *)indexBufferProcessors[ID].m_pLockedData;}
	BYTE* GetTextureProcessorData(ProcessorID ID, UINT i)
	{return (BYTE *)textureProcessors[ID].m_pLockedRectsBits[i];}
	UINT GetTextureProcessorPitch(ProcessorID ID, UINT i)
	{return textureProcessors[ID].m_pLockedRectsPitch[i];}
#endif
	virtual float GetMaxPointSpriteSize()=0;
	virtual void BeginPointSpriteRendering()=0;
	virtual void EndPointSpriteRendering()=0;

	void applyTextures(){}
void applyConstants(){}
unsigned int DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags){return 0;}
unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags){return 0;}
void changeShader(const ShaderID shader){}
void setShaderConstantRaw(const char *name, const void *data, const int size){}
ShaderID addHLSLShader(const char *vsText0, const char *gsText, const char *fsText0,
		const char *vsName, const char *psName,
		const int vsLine, const int gsLine, const int fsLine,
		const char *header, const char *extra, const char *fileName, const char **attributeNames, const int nAttributes, const unsigned int flags){return -1;}

	RendererGLES(bool fullScreen=false,
		int argc=0, char *argv[]=0);
	virtual ~RendererGLES(){
	}

	void	CreateTextureFromFile_Serial(  CHAR* szFileName,
						TextureID* ppTexture,
						//, void* pContext
						CPackedFile*		    g_PackFile,
						UINT g_SkipMips
						);
void	CreateVertexBuffer_Serial(  VertexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,



						void* pData

//					    void* pContext
						);
void	CreateIndexBuffer_Serial(  IndexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
//					    void* pContext

						);

	BYTE *fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l){return 0;}
	BYTE *fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l){return 0;}

void setGLdefaults();
		////FontID defaultFont;
	////FontID GetDefaultFont(){return defaultFont;}
	////virtual VArray <TexFont*>& GetFonts(){return fonts;}
	////virtual TexFont* GetFont(unsigned int i){return fonts.GetAt(i);}

	D3DXFROMWINEVECTOR4 scaleBias2D;
	D3DXFROMWINEVECTOR4& GetscaleBias2D(){return scaleBias2D;}

	virtual int GetTextureWidth(TextureID id);
	virtual int GetTextureHeight(TextureID id);

	virtual void viewport(int x, int y, int viewportWidth, int viewportHeight);

int texturePitchVrtl(const TextureID id,int level);
BYTE * lockTexture(const TextureID id,int level, int& pitch);
BYTE * lockVertexBuffer(const VertexBufferID id);
BYTE * lockIndexBuffer(const IndexBufferID id);
BYTE * unlockTextureVrtl(const TextureID id,int level);
BYTE * unlockVertexBuffer(const VertexBufferID id);
BYTE * unlockIndexBuffer(const IndexBufferID id);
private:
		int deleteVertexBuffer(const VertexBufferID vertexBuffer);
		int deleteIndexBuffer(const IndexBufferID indexBuffer);
public:
    ProcessorID addTextureProcessor( TextureID& ppTexture, int g_SkipMips );
    HRESULT ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes );
    HRESULT LockTextureDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceTexture(ProcessorID pProcessor);
    HRESULT UnLockTextureDeviceObject(ProcessorID pProcessor);

    ProcessorID addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData);
    HRESULT ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes );
    HRESULT LockVertexBufferDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceVertexBuffer(ProcessorID pProcessor);
    HRESULT UnLockVertexBufferDeviceObject(ProcessorID pProcessor);

    ProcessorID addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData);
    HRESULT ProcessIndexBuffer(ProcessorID pProcessor, void*pLocalData, SIZE_T Bytes );
    HRESULT LockIndexBufferDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceIndexBuffer(ProcessorID pProcessor);
    HRESULT UnLockIndexBufferDeviceObject(ProcessorID pProcessor);
	virtual BlendStateID getBlendStateVrtl(const TextureID texture)
	{
		if(texture>=textures.size())
			return -1;
		if(textures[texture].format==FORMAT_RGB8)
			return IRenderer::GetRendererInstance()->GetblendOneOne();
		else
			return IRenderer::GetRendererInstance()->GetblendSrcAlpha();
	}
	virtual unsigned int GetTextureCount(){return textures.size();}
	virtual FORMAT GetTextureFormat(const TextureID texture)
	{
		if(texture>=textures.size())
			return FORMAT_NONE;
		return textures[texture].format;
	}

	////VArray <TexFont*> fonts;

	VArray <TextureGL> textures;
	VArray <VertexBufferGL> vertexBuffers;
	VArray <IndexBufferGL> indexBuffers;
protected:
	VArray <VertexFormatGL> vertexFormats;
	VArray <SamplerStateGL> samplerStates;
	VArray <BlendStateGL> blendStates;
	VArray <AlphaStateGL> alphaStates;
	VArray <DepthStateGL> depthStates;
	VArray <RasterizerStateGL> rasterizerStates;
#if 0
	VArray <CTextureProcessorGL> textureProcessors;
	VArray <CVertexBufferProcessorGL> vertexBufferProcessors;
	VArray <CIndexBufferProcessorGL> indexBufferProcessors;
#endif

public:
#if 1
virtual void buildPerspectiveMatrix(float fieldOfView,
                                   float aspectRatio,
                                   float zNear, float zFar)//,D3DXFROMWINEMATRIX& m)
{
buildPerspectiveMatrixGL(fieldOfView,
                                   aspectRatio,
                                   zNear, zFar,(*(D3DXFROMWINEMATRIX*)pp));
	m_fieldOfView=fieldOfView;
    m_aspectRatio=aspectRatio;
    m_zNear=zNear;
    m_zFar=zFar;
//	p=m;

}
        virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
                              float centerx, float centery, float centerz,
                              float upx, float upy, float upz)//,D3DXFROMWINEMATRIX& m)
        {
#if 1
buildLookAtMatrixGL(eyex, eyey, eyez,
                              centerx, centery, centerz,
                              upx, upy, upz,(*(D3DXFROMWINEMATRIX*)vv));
#else
	D3DXFROMWINEVECTOR3 c1=D3DXFROMWINEVECTOR3(centerx-eyex,centery-eyey,centerz-eyez);
	mv=identity4();
if(_isinf(c1.x)) return;
if(_isinf(c1.y)) return;
if(_isinf(c1.z)) return;
	float radius=0.0f; float phi=0.0f; float theta=0.0f;
	float f=c1.x*c1.x+c1.y*c1.y+c1.z*c1.z;
	if(f<=0.0000001f) return;
	radius=sqrt(f);
	phi = asinf(c1.z / (radius));
	if(0.0<=c1.y)
		theta = acosf(c1.x / ((radius)* cosf((phi))));
	else if(c1.y<0.0f)
		theta = -acosf(c1.x /((radius)*cosf((phi))));
	mv=
	rotateXY(phi,theta)
	*translate(-D3DXFROMWINEVECTOR3(eyex, eyey, eyez));
#endif
	m_eye=D3DXFROMWINEVECTOR3(eyex, eyey, eyez);
	m_center=D3DXFROMWINEVECTOR3(centerx, centery, centerz);
    m_up=D3DXFROMWINEVECTOR3(upx, upy, upz);
//mv=m;
        }
#endif
	        virtual bool supportsR2VB();
        virtual bool supportsHalf();
        virtual bool supportsPS3();
        virtual bool supportsATI1N();
        void setTextureSlice(const char *textureName, const TextureID texture, const int slice);

        TextureID addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags);

        TextureID addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
        TextureID addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
        void setRenderTargetSize(const TextureID renderTarget, const int width, const int height);
        bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
        bool generateMipMaps(const TextureID renderTarget);

	int removeTexture(const TextureID texture);
	UINT32 GetAvailableTextureMem();
#if 0
        char vendor[256];
        float textureLod[MAX_TEXTUREUNIT];
        VertexFormatID activeVertexFormat[MAX_VERTEXSTREAM];
        GLuint fbo;
        GLenum drawBuffers[MAX_MRTS];

        GLenum currentFrontFace;
        GLenum currentSrcFactor;
        GLenum currentDstFactor;
        GLenum currentBlendMode;
        int currentMask;
        bool currentBlendEnable;
        int currentDepthFunc;
        bool currentDepthTestEnable;
        bool currentDepthWriteEnable;
        bool currentMultiSampleEnable;
        bool currentScissorEnable;
        int currentCullMode;
        int currentFillMode;
        GLuint currentVBO;
#endif
		virtual void resetVrtl(const unsigned int flags);
		virtual void resetToDefaults();
        void SaveImg(Image3& img,const char* fn);

        virtual void changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices);
        virtual void changeToMainFramebuffer();

//        void applyTextures();

        void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
        void applySamplerStates();
/*      ShaderID addHLSLShader(const char *vsText, const char *gsText, const char *fsText,
			const char *vsName, const char *psName, const int vsLine, const int gsLine, const int fsLine,
                const char *header, const char *extra, const char *fileName, const char **attributeNames, const int nAttributes, const unsigned int flags);
*/
		VertexFormatID addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader);
        VertexBufferID addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old);//, const IDataProcessor* p);
        IndexBufferID addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old);//, const IDataProcessor* p);

        BufferID addBufferVrtl(const STX_BUFFER_DESC      *pDesc, const STX_SUBRESOURCE_DATA *pInitialData);

        SamplerStateID addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color);
        BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage);
        DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest,
                const uint8 stencilMask, const int stencilFunc, const int stencilFail, const int depthFail, const int stencilPass);
        RasterizerStateID addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor);

//        void changeShader(const ShaderID shader);
        void changeVertexFormat(const VertexFormatID vertexFormat);
        

//      void changeSamplerState(const unsigned int samplerUnit, const SamplerStateID samplerState);
        void changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask);
        void changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef);
        void changeRasterizerState(const RasterizerStateID rasterizerState);

//        void setShaderConstantRaw(const char *name, const void *data, const int size);
//        void applyConstants();
	virtual float getTextWidth(const FontID font, const char *str, int length) const;

    AlphaStateID addAlphaState(
		const int func,
		const float ref
		);
	void changeAlphaState(const AlphaStateID alphaState);

    virtual void SaveScreenShotVrtl(const char* fn);
	virtual void captureScreenshot(Image3& img);
	virtual void Init0(void);

void grabScreenshot(Image3& img,int w,int h);
	virtual void setup2DMode(const float left, const float right, const float top, const float bottom);
/*
        virtual unsigned int DrawPrimitive(Primitives PrimitiveType,
                UINT StartVertex,
                UINT PrimitiveCount, const unsigned int flags);
    virtual unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,
                INT BaseVertexIndex,
                UINT MinVertexIndex,
                UINT NumVertices,
                UINT startIndex,
                UINT primCount, const unsigned int flags);
*/
	virtual void BeginSceneVrtl(bool aB){};
    virtual void EndSceneVrtl(bool aB){OnEndScene();}
	virtual void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors);
    virtual void PresentVrtl(bool aB);
	virtual void SwapBuffersGL(void);


protected:
        void changeFrontFace(const GLenum frontFace);
//      void setupFilter(const Texture &tex, const Filter filter, const unsigned int flags);
        void setupSampler(GLenum glTarget, const SamplerStateGL &ss, const bool bVTF_R2VB=false);
public:
        GLuint fbo;
        GLenum drawBuffers[MAX_MRTS];
        GLenum currentFrontFace;

        GLenum currentSrcFactorRGB;
        GLenum currentDstFactorRGB;
        GLenum currentSrcFactorAlpha;
        GLenum currentDstFactorAlpha;
        GLenum currentBlendModeRGB;
        GLenum currentBlendModeAlpha;

        GLuint currentVBO;
};
#endif
#endif
