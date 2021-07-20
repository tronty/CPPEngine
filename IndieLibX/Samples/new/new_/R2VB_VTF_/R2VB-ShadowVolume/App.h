#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Util/ATIModel.h"

typedef struct {
	DWORD numVertices;
	DWORD numTriangles;
	DWORD numBones;
	DWORD numFrames;
	mat4 transformMatrix;
	mat4 bonesMatrix[300][48];
	mat4 tBonesMatrix[48];
	float3 *pVertex;
	WORD *pTriangleList;

	DWORD vertexTextureWidth, vertexTextureHeight;
	ShaderID animationSH, skinningSH, modelSH, ambientModelSH;
	TextureID skinningVertex, vertexBoneIndex, vertexWeight, boneAnimation, boneMatrix;

	VertexFormatID VF;
	VertexBufferID R2VB_DummyVBO, R2VB_ShadowVolumeNormal_DummyVBO, UVBuffer;
	TextureID R2VB_VBO;
	IndexBufferID IB;

	TextureID diffuseMap;

	float time, interpolate;

	DWORD numVerticesShadowVolume, numTrianglesShadowVolume;
	VertexBufferID ShadowVolumePosition_VB;
	IndexBufferID shadowVolumeIB;
	TextureID R2VB_ShadowVolumeNormal_VBO;
	TextureID shadowVolumeVertexIndexMap;
	VertexFormatID shadowVolumeVF;
	ShaderID calculateNormalSH, shadowVolumeSH;

	VertexFormatID QuadVF;
	VertexBufferID QuadVB;
}Model;

typedef struct {
	float4 position;
	float4 color;
}Light;

class Floor : public ATIModel {
public:
	VertexFormatID VertexFormat;
	VertexBufferID VertexBuffer;
	IndexBufferID IndexBuffer;
	ShaderID Shader, ambientShader;
	TextureID DiffuseMap, BumpMap;

	bool load(char *name, Renderer* renderer);
};

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB shadow volume extrusion"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-ShadowVolume"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	void initGUI();
	bool load();

    bool loadSkinningVertexMap(char* name);
	bool loadBoneAnimationMap(char* name);
	bool createPlayer(char *model, char *skin, char *animation);
	bool generateShadowVolume();
	void psCalculateShadowVolumeNormals();
	void drawShadowVolume(Light &lit);

	void psCalculateAnimation(float t, float interp);
	void psSkinning();
	void psDrawModel(Light &lit, bool ambient);
	void playPlayer();
	void drawPlayer(Light& lit, bool ambient);

	void drawFloor(Light &lit, bool ambient);
	void drawSkybox(const float4x4 &mvp);
	void drawFrame();
	void ambientPass();
	void lightingPass(Light lit);

private:
	Menu *lightMenu;
	MenuItemID oneLight, twoLight, threeLight, fourLight;

	Model model;
	Floor floor;

	float4x4 MVP;
	VertexFormatID skyboxVF;
	ShaderID skybox;
	TextureID env;
	Light ambient;
	DWORD numLights;
	Light light[8];
};

#endif
