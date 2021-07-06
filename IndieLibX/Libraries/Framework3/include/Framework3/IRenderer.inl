/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _IRendererX_H_
#define _IRendererX_H_
inline void Begin(Primitives prim)
{
	IRenderer::GetRendererInstance()->Begin(prim);
}
inline void	BeginPointSpriteRendering()
{
	IRenderer::GetRendererInstance()->BeginPointSpriteRendering();
}
inline void BeginScene(bool aB)
{
	IRenderer::GetRendererInstance()->BeginScene(aB);
}
inline void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
	IRenderer::GetRendererInstance()->Clear(color, depth, colors);
}
inline void Color3f(float x,float  y, float  z)
{
	IRenderer::GetRendererInstance()->Color3f(x, y, z);
}
inline void Color3fv(const float* v)
{
	IRenderer::GetRendererInstance()->Color3fv(v);
}
inline void Color4f(float x,float  y, float  z ,float  w)
{
	IRenderer::GetRendererInstance()->Color4f(x, y, z, w);
}
inline void Color4fv(const float* v)
{
	IRenderer::GetRendererInstance()->Color4fv(v);
}
inline void DrawFPS(int x,  int y)
{
	IRenderer::GetRendererInstance()->DrawFPS(x, y);
}
inline void DrawSmallText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	va_list arglist;
	va_start(arglist, pszFormat);
	IRenderer::GetRendererInstance()->DrawSmallText(x, y, pszFormat, arglist);
	va_end(arglist);
}
inline void DrawText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	va_list arglist;
	va_start(arglist, pszFormat);
	IRenderer::GetRendererInstance()->DrawText(x, y, pszFormat, arglist);
	va_end(arglist);
}
inline void DumpRenderer()
{
	IRenderer::GetRendererInstance()->DumpRenderer();
}
inline void	EndPointSpriteRendering()
{
	IRenderer::GetRendererInstance()->EndPointSpriteRendering();
}
inline void EndScene(bool aB)
{
	IRenderer::GetRendererInstance()->EndScene(aB);
}
inline void End(TextureID id)
{
	IRenderer::GetRendererInstance()->End(id);
}
inline float GetAspect()
{
	return IRenderer::GetRendererInstance()->GetAspect();
}
inline UINT32 GetAvailableTextureMem()
{
	return IRenderer::GetRendererInstance()->GetAvailableTextureMem();
}
inline AlphaStateID GetCurrentAlphaState()
{
	return IRenderer::GetRendererInstance()->GetCurrentAlphaState();
}
inline BlendStateID GetCurrentBlendState()
{
	return IRenderer::GetRendererInstance()->GetCurrentBlendState();
}
inline DepthStateID GetCurrentDepthState()
{
	return IRenderer::GetRendererInstance()->GetCurrentDepthState();
}
inline IndexBufferID GetCurrentIndexBuffer()
{
	return IRenderer::GetRendererInstance()->GetCurrentIndexBuffer();
}
inline RasterizerStateID GetCurrentRasterizerState()
{
	return IRenderer::GetRendererInstance()->GetCurrentRasterizerState();
}
inline ShaderID GetCurrentShader()
{
	return IRenderer::GetRendererInstance()->GetCurrentShader();
}
inline VertexFormatID GetCurrentVertexFormat()
{
	return IRenderer::GetRendererInstance()->GetCurrentVertexFormat();
}
inline void GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc)
{
	IRenderer::GetRendererInstance()->GetDesc(id, sDesc);
}
inline void GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc)
{
	IRenderer::GetRendererInstance()->GetDesc(id, sDesc);
}
inline D3DXFROMWINEVECTOR3 GetEyePt()
{
	return IRenderer::GetRendererInstance()->GetEyePt();
}
inline int GetFILTER_BILINEAR_ANISO()
{
	return IRenderer::GetRendererInstance()->GetFILTER_BILINEAR_ANISO();
}
inline int GetFILTER_LINEAR_ANISO()
{
	return IRenderer::GetRendererInstance()->GetFILTER_LINEAR_ANISO();
}
inline int GetFILTER_TRILINEAR_ANISO()
{
	return IRenderer::GetRendererInstance()->GetFILTER_TRILINEAR_ANISO();
}
inline unsigned int GetLevelCount(TextureID id)
{
	return IRenderer::GetRendererInstance()->GetLevelCount(id);
}
inline void GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc)
{
	IRenderer::GetRendererInstance()->GetLevelDesc(id, level, sDesc);
}
inline D3DXFROMWINEVECTOR3 GetLookAtPt()
{
	return IRenderer::GetRendererInstance()->GetLookAtPt();
}
/*
inline D3DXFROMWINEMATRIX GetMVP()
{
	return IRenderer::GetRendererInstance()->GetMVP();
}
*/
inline float	GetMaxPointSpriteSize()
{
	return IRenderer::GetRendererInstance()->GetMaxPointSpriteSize();
}
inline float GetPitch()
{
	return IRenderer::GetRendererInstance()->GetPitch();
}
inline D3DXFROMWINEMATRIX GetProjMatrix()
{
	return IRenderer::GetRendererInstance()->GetProjMatrix();
}
inline float GetRoll()
{
	return IRenderer::GetRendererInstance()->GetRoll();
}
inline AlphaStateID GetSelectedAlphaState()
{
	return IRenderer::GetRendererInstance()->GetSelectedAlphaState();
}
inline BlendStateID GetSelectedBlendState()
{
	return IRenderer::GetRendererInstance()->GetSelectedBlendState();
}
inline DepthStateID GetSelectedDepthState()
{
	return IRenderer::GetRendererInstance()->GetSelectedDepthState();
}
inline IndexBufferID GetSelectedIndexBuffer()
{
	return IRenderer::GetRendererInstance()->GetSelectedIndexBuffer();
}
inline RasterizerStateID GetSelectedRasterizerState()
{
	return IRenderer::GetRendererInstance()->GetSelectedRasterizerState();
}
inline ShaderID GetSelectedShader()
{
	return IRenderer::GetRendererInstance()->GetSelectedShader();
}
inline VertexFormatID GetSelectedVertexFormat()
{
	return IRenderer::GetRendererInstance()->GetSelectedVertexFormat();
}
inline unsigned int GetTextureCount()
{
	return IRenderer::GetRendererInstance()->GetTextureCount();
}
inline int GetTextureHeight(TextureID id)
{
	return IRenderer::GetRendererInstance()->GetTextureHeight(id);
}
inline int GetTextureWidth(TextureID id)
{
	return IRenderer::GetRendererInstance()->GetTextureWidth(id);
}
inline void GetTransform( TS t, D3DXFROMWINEMATRIX& m)
{
	IRenderer::GetRendererInstance()->GetTransform(t, m);
}
inline void GetTransform( TS t, D3DXFROMWINEMATRIX* m)
{
	IRenderer::GetRendererInstance()->GetTransform(t, m);
}
inline D3DXFROMWINEVECTOR3 GetUpPt()
{
	return IRenderer::GetRendererInstance()->GetUpPt();
}
inline D3DXFROMWINEMATRIX GetViewMatrix()
{
	return IRenderer::GetRendererInstance()->GetViewMatrix();
}
inline D3DFROMWINEVIEWPORT9 GetViewport()
{
	return IRenderer::GetRendererInstance()->GetViewport();
}
inline int GetViewportHeight()
{
	return IRenderer::GetRendererInstance()->GetViewportHeight();
}
inline int GetViewportWidth()
{
	return IRenderer::GetRendererInstance()->GetViewportWidth();
}
inline int GetViewportX()
{
	return IRenderer::GetRendererInstance()->GetViewportX();
}
inline int GetViewportY()
{
	return IRenderer::GetRendererInstance()->GetViewportY();
}
inline float GetYaw()
{
	return IRenderer::GetRendererInstance()->GetYaw();
}
inline int Getbilinear()
{
	return IRenderer::GetRendererInstance()->Getbilinear();
}
inline int GetbilinearAniso()
{
	return IRenderer::GetRendererInstance()->GetbilinearAniso();
}
inline int GetbilinearClamp()
{
	return IRenderer::GetRendererInstance()->GetbilinearClamp();
}
inline int GetblendOneOne()
{
	return IRenderer::GetRendererInstance()->GetblendOneOne();
}
inline int GetblendOneOneReverseSubstract()
{
	return IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract();
}
inline int GetblendSrcAlpha()
{
	return IRenderer::GetRendererInstance()->GetblendSrcAlpha();
}
inline int GetcullBack()
{
	return IRenderer::GetRendererInstance()->GetcullBack();
}
inline int GetcullFront()
{
	return IRenderer::GetRendererInstance()->GetcullFront();
}
inline int GetcullNone()
{
	return IRenderer::GetRendererInstance()->GetcullNone();
}
inline FontID GetdefaultFont()
{
	return IRenderer::GetRendererInstance()->GetdefaultFont();
}
inline int GetdisableAlpha()
{
	return IRenderer::GetRendererInstance()->GetdisableAlpha();
}
inline int GetenableAlpha()
{
	return IRenderer::GetRendererInstance()->GetenableAlpha();
}
inline int GetfillSolid()
{
	return IRenderer::GetRendererInstance()->GetfillSolid();
}
inline int GetfillWireFrame()
{
	return IRenderer::GetRendererInstance()->GetfillWireFrame();
}
inline int Getlinear()
{
	return IRenderer::GetRendererInstance()->Getlinear();
}
inline int GetlinearAniso()
{
	return IRenderer::GetRendererInstance()->GetlinearAniso();
}
inline int GetlinearClamp()
{
	return IRenderer::GetRendererInstance()->GetlinearClamp();
}
inline D3DXFROMWINEMATRIX Getmv()
{
	return IRenderer::GetRendererInstance()->Getmv();
}
inline D3DXFROMWINEMATRIX Getmvp()
{
	return IRenderer::GetRendererInstance()->Getmvp();
}
inline D3DXFROMWINEMATRIX Getmvpt()
{
	return IRenderer::GetRendererInstance()->Getmvpt();
}
inline int Getnearest()
{
	return IRenderer::GetRendererInstance()->Getnearest();
}
inline int GetnearestClamp()
{
	return IRenderer::GetRendererInstance()->GetnearestClamp();
}
inline int GetnoDepthTest()
{
	return IRenderer::GetRendererInstance()->GetnoDepthTest();
}
inline int GetnoDepthWrite()
{
	return IRenderer::GetRendererInstance()->GetnoDepthWrite();
}
inline D3DXFROMWINEMATRIX Getp()
{
	return IRenderer::GetRendererInstance()->Getp();
}
inline D3DXFROMWINEMATRIX Getpmv()
{
	return IRenderer::GetRendererInstance()->Getpmv();
}
inline D3DXFROMWINEMATRIX Getpmvt()
{
	return IRenderer::GetRendererInstance()->Getpmvt();
}
inline D3DXFROMWINEMATRIX Getpp()
{
	return IRenderer::GetRendererInstance()->Getpp();
}
inline int Gettrilinear()
{
	return IRenderer::GetRendererInstance()->Gettrilinear();
}
inline int GettrilinearAniso()
{
	return IRenderer::GetRendererInstance()->GettrilinearAniso();
}
inline int GettrilinearClamp()
{
	return IRenderer::GetRendererInstance()->GettrilinearClamp();
}
inline D3DXFROMWINEMATRIX Getvv()
{
	return IRenderer::GetRendererInstance()->Getvv();
}
inline D3DXFROMWINEMATRIX Getww()
{
	return IRenderer::GetRendererInstance()->Getww();
}
inline void Init0()
{
	IRenderer::GetRendererInstance()->Init0();
}
inline void InitPlain(ShaderID& s, VertexFormatID& vf)
{
	IRenderer::GetRendererInstance()->InitPlain(s, vf);
}
inline void InitPlain3(ShaderID& s, VertexFormatID& vf)
{
	IRenderer::GetRendererInstance()->InitPlain3(s, vf);
}
inline void InitTex(ShaderID& s, VertexFormatID& vf)
{
	IRenderer::GetRendererInstance()->InitTex(s, vf);
}
inline bool IsOK()
{
	return IRenderer::GetRendererInstance()->IsOK();
}
inline BYTE LockIndexBuffer(IndexBufferID id)
{
	return IRenderer::GetRendererInstance()->LockIndexBuffer(id);
}
inline BYTE LockTexture(TextureID id, int m, int& pitch)
{
	return IRenderer::GetRendererInstance()->LockTexture(id, m, pitch);
}
inline BYTE LockVertexBuffer(VertexBufferID id)
{
	return IRenderer::GetRendererInstance()->LockVertexBuffer(id);
}
inline void NormalPointer(const int off, const void* p)
{
	IRenderer::GetRendererInstance()->NormalPointer(off, p);
}
inline void PopMatrix()
{
	IRenderer::GetRendererInstance()->PopMatrix();
}
inline void Present(bool aB)
{
	IRenderer::GetRendererInstance()->Present(aB);
}
inline void PushMatrix()
{
	IRenderer::GetRendererInstance()->PushMatrix();
}
inline void SaveScreenShot(const char* fn)
{
	IRenderer::GetRendererInstance()->SaveScreenShot(fn);
}
inline void SetFloat(const char* n, float d)
{
	IRenderer::GetRendererInstance()->SetFloat(n, d);
}
inline void SetInput(IInput* a_pInput)
{
	IRenderer::GetRendererInstance()->SetInput(a_pInput);
}
inline void SetInt(const char* n, int d)
{
	IRenderer::GetRendererInstance()->SetInt(n, d);
}
inline void SetMatrix(const char* n, const D3DXFROMWINEMATRIX * d)
{
	IRenderer::GetRendererInstance()->SetMatrix(n, d);
}
inline void SetMatrixTransposeArray(const char* n, const D3DXFROMWINEMATRIX * d,unsigned int nn)
{
	IRenderer::GetRendererInstance()->SetMatrixTransposeArray(n, d, nn);
}
inline void SetTexture(const char* n, TextureID id)
{
	IRenderer::GetRendererInstance()->SetTexture(n, id);
}
inline void SetTransform( TS t, const D3DXFROMWINEMATRIX& m)
{
	IRenderer::GetRendererInstance()->SetTransform(t, m);
}
inline void SetTransform( TS t, const D3DXFROMWINEMATRIX* m)
{
	IRenderer::GetRendererInstance()->SetTransform(t, m);
}
inline void SetVector(const char* n, const D3DXFROMWINEVECTOR4 * d)
{
	IRenderer::GetRendererInstance()->SetVector(n, d);
}
inline void SetVectorArray(const char* n, const D3DXFROMWINEVECTOR4 * d,unsigned int nn)
{
	IRenderer::GetRendererInstance()->SetVectorArray(n, d, nn);
}
inline void Setmv(const D3DXFROMWINEMATRIX& m)
{
	IRenderer::GetRendererInstance()->Setmv(m);
}
inline void Setp(const D3DXFROMWINEMATRIX& m)
{
	IRenderer::GetRendererInstance()->Setp(m);
}
inline void TexCoord2f(float x,float  y)
{
	IRenderer::GetRendererInstance()->TexCoord2f(x, y);
}
inline void TexCoordPointer(const int w,  const int off, const void* p)
{
	IRenderer::GetRendererInstance()->TexCoordPointer(w, off, p);
}
inline void Translatef(float ax, float ay, float az)
{
	IRenderer::GetRendererInstance()->Translatef(ax, ay, az);
}
inline void UnLockIndexBuffer(IndexBufferID id)
{
	IRenderer::GetRendererInstance()->UnLockIndexBuffer(id);
}
inline void UnLockVertexBuffer(VertexBufferID id)
{
	IRenderer::GetRendererInstance()->UnLockVertexBuffer(id);
}
inline void UnlockTexture(TextureID id, int m)
{
	IRenderer::GetRendererInstance()->UnlockTexture(id, m);
}
inline void Vertex2f(float x,float  y)
{
	IRenderer::GetRendererInstance()->Vertex2f(x, y);
}
inline void Vertex2fv(const float* v)
{
	IRenderer::GetRendererInstance()->Vertex2fv(v);
}
inline void Vertex2i(int x,int y)
{
	IRenderer::GetRendererInstance()->Vertex2i(x, y);
}
inline void Vertex3f(float x,float  y, float  z)
{
	IRenderer::GetRendererInstance()->Vertex3f(x, y, z);
}
inline void Vertex3fv(const float* v)
{
	IRenderer::GetRendererInstance()->Vertex3fv(v);
}
inline void Vertex3i(int x,int y, int z)
{
	IRenderer::GetRendererInstance()->Vertex3i(x, y, z);
}
inline void Vertex4f(float x,float  y, float  z ,float  w)
{
	IRenderer::GetRendererInstance()->Vertex4f(x, y, z, w);
}
inline void Vertex4fv(const float* v)
{
	IRenderer::GetRendererInstance()->Vertex4fv(v);
}
inline void Vertex4i(int x,int y, int z ,int w)
{
	IRenderer::GetRendererInstance()->Vertex4i(x, y, z, w);
}
inline void VertexPointer(const int w,  const int off, const void* p)
{
	IRenderer::GetRendererInstance()->VertexPointer(w, off, p);
}
inline int addAnimation(char* afile)
{
	return IRenderer::GetRendererInstance()->addAnimation(afile);
}
inline ShaderID addHLSLShader(const char *shaderTextp, const char *vsName, const char *psName,  const char **attributeNames, const int nAttributes, const char *extra, const unsigned int flags)
{
	return IRenderer::GetRendererInstance()->addHLSLShader(shaderTextp, vsName, psName, attributeNames, nAttributes, extra, flags);
}
inline IndexBufferID addIndexBuffer(const unsigned int nIndices,  const unsigned int indexSize,  const BufferAccess bufferAccess,  const void *data ,  IndexBufferID old)
{
	return IRenderer::GetRendererInstance()->addIndexBuffer(nIndices, indexSize, bufferAccess, data, old);
}
inline int addMesh(char* afile)
{
	return IRenderer::GetRendererInstance()->addMesh(afile);
}
inline RasterizerStateID addRasterizerState(const int cullMode,  const int fillMode ,  const bool multiSample ,  const bool scissor )
{
	return IRenderer::GetRendererInstance()->addRasterizerState(cullMode, fillMode, multiSample, scissor);
}
inline TextureID addRenderDepth(const int width, const int height, const int depthBits)
{
	return IRenderer::GetRendererInstance()->addRenderDepth(width, height, depthBits);
}
inline TextureID addRenderDepth(const int width,  const int height,  const int arraySize,  const FORMAT format,  const int msaaSamples ,  const SamplerStateID samplerState ,  unsigned int flags )
{
	return IRenderer::GetRendererInstance()->addRenderDepth(width, height, arraySize, format, msaaSamples, samplerState, flags);
}
inline TextureID addRenderTarget(const int width,  const int height,  const FORMAT format,  const SamplerStateID samplerState ,  unsigned int flags )
{
	return IRenderer::GetRendererInstance()->addRenderTarget(width, height, format, samplerState, flags);
}
inline TextureID addRenderTarget(const int width,  const int height,  const int depth,  const int mipMapCount,  const int arraySize,  const FORMAT format,  const int msaaSamples ,  const SamplerStateID samplerState ,  unsigned int flags )
{
	return IRenderer::GetRendererInstance()->addRenderTarget(width, height, depth, mipMapCount, arraySize, format, msaaSamples, samplerState, flags);
}
inline SamplerStateID addSamplerState(const Filter filter,  const AddressMode s,  const AddressMode t,  const AddressMode r,  const float lod ,  const unsigned int maxAniso ,  const int compareFunc ,  const float *border_color )
{
	return IRenderer::GetRendererInstance()->addSamplerState(filter, s, t, r, lod, maxAniso, compareFunc, border_color);
}
inline ShaderID addShaderFromFile(const char *fileName, const char *vsName, const char *psName,  const char **attributeNames, const int nAttributes, const char *extra, const unsigned int flags)
{
	return IRenderer::GetRendererInstance()->addShaderFromFile(fileName, vsName, psName, attributeNames, nAttributes, extra, flags);
}
inline int addSound(char* afile, char* aname)
{
	return IRenderer::GetRendererInstance()->addSound(afile, aname);
}
inline TextureID addTexture(int Width,  int Height,  bool MipLevels,  FORMAT format,  const SamplerStateID samplerState ,  ubyte R, ubyte G, ubyte B_, ubyte A_, const unsigned int flags )
{
	return IRenderer::GetRendererInstance()->addTexture(Width, Height, MipLevels, format, samplerState, alpha, flags);
}
inline VertexBufferID addVertexBuffer(const int size,  const BufferAccess bufferAccess,  const void *data ,  VertexBufferID old)
{
	return IRenderer::GetRendererInstance()->addVertexBuffer(size, bufferAccess, data, old);
}
inline VertexFormatID addVertexFormat(const FormatDesc *formatDesc,  const unsigned int nAttribs,  const ShaderID shader )
{
	return IRenderer::GetRendererInstance()->addVertexFormat(formatDesc, nAttribs, shader);
}
inline void apply()
{
	IRenderer::GetRendererInstance()->apply();
}
inline void applyConstants()
{
	IRenderer::GetRendererInstance()->applyConstants();
}
inline void applySamplerStates()
{
	IRenderer::GetRendererInstance()->applySamplerStates();
}
inline void applyTextures()
{
	IRenderer::GetRendererInstance()->applyTextures();
}
inline void captureScreenshot(Image3& img)
{
	IRenderer::GetRendererInstance()->captureScreenshot(img);
}
inline void changeAlphaState(const AlphaStateID alphaState)
{
	IRenderer::GetRendererInstance()->changeAlphaState(alphaState);
}
inline void changeBlendState(const BlendStateID blendState,  const unsigned int sampleMask )
{
	IRenderer::GetRendererInstance()->changeBlendState(blendState, sampleMask);
}
inline void changeDepthState(const DepthStateID depthState,  const unsigned int stencilRef )
{
	IRenderer::GetRendererInstance()->changeDepthState(depthState, stencilRef);
}
inline void changeIndexBuffer(const IndexBufferID indexBuffer)
{
	IRenderer::GetRendererInstance()->changeIndexBuffer(indexBuffer);
}
inline void changeRasterizerState(const RasterizerStateID rasterizerState)
{
	IRenderer::GetRendererInstance()->changeRasterizerState(rasterizerState);
}
inline void changeRenderTarget(const TextureID colorRT,  const TextureID depthRT ,  const int slice )
{
	IRenderer::GetRendererInstance()->changeRenderTarget(colorRT, depthRT, slice);
}
inline void changeRenderTargets(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int *faces)
{
	IRenderer::GetRendererInstance()->changeRenderTargets(colorRTs, nRenderTargets, depthRT, faces);
}
inline void changeRenderTargets(const TextureID *colorRTs,  const unsigned int nRenderTargets,  const TextureID depthRT ,  const int depthSlice ,  const int *slices )
{
	IRenderer::GetRendererInstance()->changeRenderTargets(colorRTs, nRenderTargets, depthRT, depthSlice, slices);
}
inline void changeShader(const ShaderID shader)
{
	IRenderer::GetRendererInstance()->changeShader(shader);
}
inline void changeToMainFramebuffer()
{
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}
inline void changeVertexBuffer(const void *base)
{
	IRenderer::GetRendererInstance()->changeVertexBuffer(base);
}
inline void changeVertexBuffer(const int stream,  const VertexBufferID vertexBuffer,  const intptr offset )
{
	IRenderer::GetRendererInstance()->changeVertexBuffer(stream, vertexBuffer, offset);
}
inline void changeVertexFormat(const VertexFormatID vertexFormat)
{
	IRenderer::GetRendererInstance()->changeVertexFormat(vertexFormat);
}
inline bool debugRuntime()
{
	return IRenderer::GetRendererInstance()->debugRuntime();
}
inline void deleteAnimation(char* afile)
{
	IRenderer::GetRendererInstance()->deleteAnimation(afile);
}
inline int deleteIndexBuffer(const IndexBufferID indexBuffer)
{
	return IRenderer::GetRendererInstance()->deleteIndexBuffer(indexBuffer);
}
inline void deleteMesh(char* afile)
{
	IRenderer::GetRendererInstance()->deleteMesh(afile);
}
inline void deleteSound(char* afile, char* aname)
{
	IRenderer::GetRendererInstance()->deleteSound(afile, aname);
}
inline int deleteTexture(const TextureID texture)
{
	return IRenderer::GetRendererInstance()->deleteTexture(texture);
}
inline int deleteVertexBuffer(const VertexBufferID vertexBuffer)
{
	return IRenderer::GetRendererInstance()->deleteVertexBuffer(vertexBuffer);
}
inline void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)
{
	IRenderer::GetRendererInstance()->drawArrays(primitives, firstVertex, nVertices);
}
inline void drawCircle(const float x,  const float y,  const float r,  const D3DXFROMWINEVECTOR4 &color,  const float lineWidth )
{
	IRenderer::GetRendererInstance()->drawCircle(x, y, r, color, lineWidth);
}
inline void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
	IRenderer::GetRendererInstance()->drawElements(primitives, firstIndex, nIndices, firstVertex, nVertices);
}
inline void drawFPS()
{
	IRenderer::GetRendererInstance()->drawFPS();
}
inline void drawLine(const float x0,  const float y0,  const float x1,  const float y1,  const D3DXFROMWINEVECTOR4 &color,  const float lineWidth )
{
	IRenderer::GetRendererInstance()->drawLine(x0, y0, x1, y1, color, lineWidth);
}
inline void drawPlain(const Primitives primitives, D3DXFROMWINEVECTOR2 *vertices, const unsigned int nVertices, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color)
{
	IRenderer::GetRendererInstance()->drawPlain(primitives, vertices, nVertices, blendState, depthState, color);
}
inline void drawQuad(const float x0, const float y0, const float x1, const float y1, const float s0, const float t0, const float s1, const float t1, const D3DXFROMWINEVECTOR4 &color)
{
	IRenderer::GetRendererInstance()->drawQuad(x0, y0, x1, y1, s0, t0, s1, t1, color);
}
inline void drawRect(const float x0,  const float y0,  const float x1,  const float y1,  const D3DXFROMWINEVECTOR4 &color,  const float lineWidth )
{
	IRenderer::GetRendererInstance()->drawRect(x0, y0, x1, y1, color, lineWidth);
}
inline void drawRoundRect(const float x0,  const float y0,  const float x1,  const float y1,  const float r,  const D3DXFROMWINEVECTOR4 &color,  const float lineWidth )
{
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, r, color, lineWidth);
}
inline bool drawText(const char *str, float x, float y, const float charWidth, const float charHeight, const FontID& font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState)
{
	return IRenderer::GetRendererInstance()->drawText(str, x, y, charWidth, charHeight, font, samplerState, blendState, depthState);
}
inline void drawTexture(TextureID texture,  int x,  int y,  int sx,  int sy,  bool InScene,  float tx,  float ty,  float stx,  float sty)
{
	IRenderer::GetRendererInstance()->drawTexture(texture, x, y, sx, sy, InScene, tx, ty, stx, sty);
}
inline void drawTextured(const Primitives primitives,  TexVertex *vertices,  const unsigned int nVertices,  const TextureID texture,  const SamplerStateID samplerState,  const BlendStateID blendState,  const DepthStateID depthState,  const D3DXFROMWINEVECTOR4 *color )
{
	IRenderer::GetRendererInstance()->drawTextured(primitives, vertices, nVertices, texture, samplerState, blendState, depthState, color);
}
inline void drawTriangle(const float x0,  const float y0,  const float x1,  const float y1,  const float x2,  const float y2,  const D3DXFROMWINEVECTOR4 &color,  const float lineWidth )
{
	IRenderer::GetRendererInstance()->drawTriangle(x0, y0, x1, y1, x2, y2, color, lineWidth);
}
inline void elevate(float add)
{
	IRenderer::GetRendererInstance()->elevate(add);
}
inline BYTE fillIndexBuffer(const IndexBufferID id,  BYTE*  m_pData,  int l)
{
	return IRenderer::GetRendererInstance()->fillIndexBuffer(id, m_pData, l);
}
inline void fillTextBuffer(TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font)
{
	IRenderer::GetRendererInstance()->fillTextBuffer(dest, str, x, y, charWidth, charHeight, font);
}
inline BYTE fillVertexBuffer(const VertexBufferID id,  BYTE*  m_pData,  int l)
{
	return IRenderer::GetRendererInstance()->fillVertexBuffer(id, m_pData, l);
}
inline bool generateMipMaps(const TextureID renderTarget)
{
	return IRenderer::GetRendererInstance()->generateMipMaps(renderTarget);
}
inline BlendStateID getBlendState(const TextureID texture)
{
	return IRenderer::GetRendererInstance()->getBlendState(texture);
}
inline int getFPS()
{
	return IRenderer::GetRendererInstance()->getFPS();
}
inline int getFormatSize(const AttributeFormat format)
{
	return IRenderer::GetRendererInstance()->getFormatSize(format);
}
inline unsigned int getIndicesCount(const Primitives primitives)
{
	return IRenderer::GetRendererInstance()->getIndicesCount(primitives);
}
inline unsigned int getPrimitiveCount(const Primitives primitives, const int count)
{
	return IRenderer::GetRendererInstance()->getPrimitiveCount(primitives, count);
}
inline unsigned int getPrimitiveCountR(const Primitives primitives, const int count)
{
	return IRenderer::GetRendererInstance()->getPrimitiveCountR(primitives, count);
}
inline float getStringWidth(FontID font,const char* str, const float charWidth)
{
	return IRenderer::GetRendererInstance()->getStringWidth(font, str, charWidth);
}
inline unsigned int getTextQuads(const char *str)
{
	return IRenderer::GetRendererInstance()->getTextQuads(str);
}
inline float getTextWidth(const FontID font, const char *str, int length = -1)
{
	return IRenderer::GetRendererInstance()->getTextWidth(font, str, length);
}
inline unsigned int getVertexCount(const Primitives primitives, const int  NumPrim)
{
	return IRenderer::GetRendererInstance()->getVertexCount(primitives, NumPrim);
}
inline D3DXFROMWINEMATRIX identity()
{
	return IRenderer::GetRendererInstance()->identity();
}
inline void lineTo(int x, int y)
{
	IRenderer::GetRendererInstance()->lineTo(x, y);
}
inline BYTE lockIndexBuffer(const IndexBufferID indexBuffer)
{
	return IRenderer::GetRendererInstance()->lockIndexBuffer(indexBuffer);
}
inline BYTE lockTexture(const TextureID texture, int level, int& pitch)
{
	return IRenderer::GetRendererInstance()->lockTexture(texture, level, pitch);
}
inline BYTE lockVertexBuffer(const VertexBufferID vertexBuffer)
{
	return IRenderer::GetRendererInstance()->lockVertexBuffer(vertexBuffer);
}
inline void moveTo(int x, int y)
{
	IRenderer::GetRendererInstance()->moveTo(x, y);
}
inline void playSound(int id)
{
	IRenderer::GetRendererInstance()->playSound(id);
}
inline int removeTexture(const TextureID texture)
{
	return IRenderer::GetRendererInstance()->removeTexture(texture);
}
inline void renderAnimation(int id, int x, int y, int sx, int sy)
{
	IRenderer::GetRendererInstance()->renderAnimation(id, x, y, sx, sy);
}
inline void renderMesh(int id, D3DXFROMWINEMATRIX mvp)
{
	IRenderer::GetRendererInstance()->renderMesh(id, mvp);
}
inline void resetToDefaults()
{
	IRenderer::GetRendererInstance()->resetToDefaults();
}
inline void reset(const unsigned int flags)
{
	IRenderer::GetRendererInstance()->reset(flags);
}
inline bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
{
	return IRenderer::GetRendererInstance()->resizeRenderTarget(renderTarget, width, height, depth, mipMapCount, arraySize);
}
inline void setAlphaState(const AlphaStateID alphaState)
{
	IRenderer::GetRendererInstance()->setAlphaState(alphaState);
}
inline void setBlendState(const BlendStateID blendState,  const unsigned int sampleMask )
{
	IRenderer::GetRendererInstance()->setBlendState(blendState, sampleMask);
}
inline void setDepthState(const DepthStateID depthState,  const unsigned int stencilRef )
{
	IRenderer::GetRendererInstance()->setDepthState(depthState, stencilRef);
}
inline void setIndexBuffer(const IndexBufferID indexBuffer)
{
	IRenderer::GetRendererInstance()->setIndexBuffer(indexBuffer);
}
inline void setRasterizerState(const RasterizerStateID rasterizerState)
{
	IRenderer::GetRendererInstance()->setRasterizerState(rasterizerState);
}
inline void setSamplerState(const char *samplerName, const SamplerStateID samplerState)
{
	IRenderer::GetRendererInstance()->setSamplerState(samplerName, samplerState);
}
inline void setShader(const ShaderID shader)
{
	IRenderer::GetRendererInstance()->setShader(shader);
}
inline void setShaderConstant1f(const char *name, const float constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant1f(name, constant);
}
inline void setShaderConstant1i(const char *name, const int constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant1i(name, constant);
}
inline void setShaderConstant2f(const char *name, const D3DXFROMWINEVECTOR2 &constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant2f(name, constant);
}
inline void setShaderConstant3f(const char *name, const D3DXFROMWINEVECTOR3 &constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant3f(name, constant);
}
inline void setShaderConstant4f(const char *name, const D3DXFROMWINEVECTOR4 &constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant4f(name, constant);
}
inline void setShaderConstant4x4f(const char *name, const D3DXFROMWINEMATRIX &constant)
{
	IRenderer::GetRendererInstance()->setShaderConstant4x4f(name, constant);
}
inline void setShaderConstantArray1f(const char *name, const float *constant, const unsigned int count)
{
	IRenderer::GetRendererInstance()->setShaderConstantArray1f(name, constant, count);
}
inline void setShaderConstantArray2f(const char *name, const D3DXFROMWINEVECTOR2  *constant, const unsigned int count)
{
	IRenderer::GetRendererInstance()->setShaderConstantArray2f(name, constant, count);
}
inline void setShaderConstantArray3f(const char *name, const D3DXFROMWINEVECTOR3  *constant, const unsigned int count)
{
	IRenderer::GetRendererInstance()->setShaderConstantArray3f(name, constant, count);
}
inline void setShaderConstantArray4f(const char *name, const D3DXFROMWINEVECTOR4  *constant, const unsigned int count)
{
	IRenderer::GetRendererInstance()->setShaderConstantArray4f(name, constant, count);
}
inline void setShaderConstantArray4x4f(const char *name, const D3DXFROMWINEMATRIX *constant, const unsigned int count)
{
	IRenderer::GetRendererInstance()->setShaderConstantArray4x4f(name, constant, count);
}
inline void setShaderConstantRaw(const char *name, const void *data, const int size)
{
	IRenderer::GetRendererInstance()->setShaderConstantRaw(name, data, size);
}
inline void setTexture(const char *textureName, const TextureID texture)
{
	IRenderer::GetRendererInstance()->setTexture(textureName, texture);
}
inline void setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
	IRenderer::GetRendererInstance()->setTextureSlice(textureName, texture, slice);
}
inline void setVertexBuffer(const int stream, const void *base)
{
	IRenderer::GetRendererInstance()->setVertexBuffer(stream, base);
}
inline void setVertexBuffer(const int stream,  const VertexBufferID vertexBuffer,  const intptr offset )
{
	IRenderer::GetRendererInstance()->setVertexBuffer(stream, vertexBuffer, offset);
}
inline void setVertexFormat(const VertexFormatID vertexFormat)
{
	IRenderer::GetRendererInstance()->setVertexFormat(vertexFormat);
}
inline void setViewport(int x,  int y,  int viewportWidth,  int viewportHeight,  float minZ,  float maxZ)
{
	IRenderer::GetRendererInstance()->setViewport(x, y, viewportWidth, viewportHeight, minZ, maxZ);
}
inline void setup2DMode(const float left, const float right, const float top, const float bottom)
{
	IRenderer::GetRendererInstance()->setup2DMode(left, right, top, bottom);
}
inline void stopSound(int id)
{
	IRenderer::GetRendererInstance()->stopSound(id);
}
inline bool supportsHalf()
{
	return IRenderer::GetRendererInstance()->supportsHalf();
}
inline bool supportsR2VB()
{
	return IRenderer::GetRendererInstance()->supportsR2VB();
}
inline int texturePitch(const TextureID texture,  int level)
{
	return IRenderer::GetRendererInstance()->texturePitch(texture, level);
}
inline BYTE unlockIndexBuffer(const IndexBufferID indexBuffer)
{
	return IRenderer::GetRendererInstance()->unlockIndexBuffer(indexBuffer);
}
inline BYTE unlockTexture(const TextureID texture,  int level)
{
	return IRenderer::GetRendererInstance()->unlockTexture(texture, level);
}
inline BYTE unlockVertexBuffer(const VertexBufferID vertexBuffer)
{
	return IRenderer::GetRendererInstance()->unlockVertexBuffer(vertexBuffer);
}
inline void updateClipPlanes(float zNear, float zFar)
{
	IRenderer::GetRendererInstance()->updateClipPlanes(zNear, zFar);
}
inline void viewport(int x, int y, int viewportWidth, int viewportHeight)
{
	IRenderer::GetRendererInstance()->viewport(x, y, viewportWidth, viewportHeight);
}
#endif

