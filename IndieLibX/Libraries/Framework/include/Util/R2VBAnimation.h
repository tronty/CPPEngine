/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define MAX_NUM_BONES_PER_VERTEX				4
#define MAX_NUM_ANIMATIONS						300
#define MAX_NUM_BONES							48
#define MAX_TEX_SIZE							2048

class R2VBAnimation {
public:
	__DWORD__ maxTextureSize;

	
protected:
	__DWORD__ numVertices;
	__DWORD__ numTriangles;
	__DWORD__ numBones;	
	__DWORD__ numFrames;
	D3DXFROMWINEMATRIX transformMatrix;
	D3DXFROMWINEMATRIX bonesMatrix[MAX_NUM_ANIMATIONS][MAX_NUM_BONES];
	D3DXFROMWINEMATRIX tBonesMatrix[MAX_NUM_BONES];
	__WORD__ *pTriangleList;

	__DWORD__ vertexTextureWidth, vertexTextureHeight;
	ShaderID animationSH, skinningSH, modelSH;
	TextureID skinningVertex, vertexBoneIndex, vertexWeight, boneAnimation, boneMatrix;

	VertexFormatID animationVF, skinningVF, modelVF;
	VertexBufferID R2VB_DummyVBO, UVBuffer;
	TextureID R2VB_VBO;
	IndexBufferID IB;

	TextureID diffuseMap;

	float time, interpolate;
	float startTime, endTime;

	VertexFormatID QuadVF;
	VertexBufferID QuadVB;
public:

	R2VBAnimation();
	~R2VBAnimation();

	bool loadSkinningVertexMap(char* name);
	bool loadBoneAnimationMap(char* name);
	bool load(char *modelname, char *diffuse, __DWORD__ maxtexsize, bool debugruntime, const char* aPath);
	inline const __DWORD__ getNumVertices() {return numVertices; };
	inline const __DWORD__ getNumTriangles() {return numTriangles; };
	inline const TextureID &getR2VB_VBO() {return R2VB_VBO; };
	inline void setRange(float s, float e) {startTime = s; endTime = e; };
	inline void setTime(float t) {time = t; interpolate = 0.0f; };
	inline const D3DXFROMWINEMATRIX& getTransformMatrix() {return transformMatrix; };
	inline void setTransformMatrix(const D3DXFROMWINEMATRIX& m) {transformMatrix = m; };
	inline void setPosition(const D3DXFROMWINEVECTOR3& p) {transformMatrix.m[3][0] = p.x; transformMatrix.m[3][1] = p.y; transformMatrix.m[3][2] = p.z; };
	inline void getPosition(D3DXFROMWINEVECTOR3& p) {p.x = transformMatrix.m[3][0]; p.y = transformMatrix.m[3][1]; p.z = transformMatrix.m[3][2]; };
	
	virtual void psCalculateAnimation(float t, float interp);
	void psSkinning();
	void play(float step = 1.0f, bool bloop = true);
	inline void setupVertexSource()
	{
		IRenderer::GetRendererInstance()->setShader(modelSH);
		IRenderer::GetRendererInstance()->setVertexFormat(modelVF);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, UVBuffer);
		//IRenderer::GetRendererInstance()->setVertexBuffer(1, R2VB_DummyVBO);
		IRenderer::GetRendererInstance()->setIndexBuffer(IB);
	};	
	void draw(D3DXFROMWINEMATRIX &mvp);
};

