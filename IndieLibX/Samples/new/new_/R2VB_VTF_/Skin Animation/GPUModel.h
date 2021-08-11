//-----------------------------------------------------------------------------
// File: D3D10\Skin Animation\GPUModel.h
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _GPUMODEL_H_
#define _GPUMODEL_H_

#include "../Framework/Model/D3D10Model.h"

enum NPC_State{
	RUN = 0,
	DYING,
	DANCE,
	RESPAWN,
	KICK,
};

typedef struct {
	float4x4		Trans;
	float3			Position;
	float4			Rotation;
	float			Speed;
	float			Deg;
	bool			bBounce;
	bool			bInSafeZone;

	float			time;

	bool			bDie, bBounceByGround;
	float			upSpeed;
	float3			flyDir;

	NPC_State		currentState;	
}AI;

#define GPU					NUM_BONE_MATRIX_CONSTNAT_TYPE

typedef struct{
	float4 Position;
	float4 Normal;	
}GPUSTREAMOUTVERTEX;

typedef struct{	
	uint BoneIndex;
	#if __USE_CONSTANT__
		uint ModelIndex;
	#endif
}ANIMATIONKEYINPUT;

typedef struct{	
	float4 Rotation; // x, y, z, w -> Quaternion
	float4 Position; // x, y, z -> Position, w -> animation time.
	//float Time;
}ANIMATIONPASSINSTANCEDATA;

typedef struct{	
	float4 Rotation; // x, y, z, w -> Quaternion
	float3 Position; // x, y, z -> Position
}DRAWPASSINSTANCEDATA;

class GPUModel : public D3D10Model {
public:
	#define MAX_MODELS_A_DRAW				1024*5
protected:
	unsigned int m_MaxADraw;										
	//GPU Animation Buffers
	ID3D10EffectTechnique* m_techGPU;
	ID3D10EffectShaderResourceVariable* m_RotationKeysBufferVar;
	ID3D10EffectShaderResourceVariable* m_PositionKeysBufferVar;
	ID3D10EffectShaderResourceVariable* m_ScaleKeysBufferVar;
	
	ID3D10Texture2D* m_RotationKeysTex;
	ID3D10Texture2D* m_PositionKeysTex;
	ID3D10ShaderResourceView* m_RotationKeysTexView;
	ID3D10ShaderResourceView* m_PositionKeysTexView;

	ID3D10Buffer* m_boneMatrixBuffer; 
	ID3D10ShaderResourceView* m_boneMatrixBufferRV;

	ID3D10InputLayout* m_stramOutIL;
	ID3D10Buffer* m_streamOutVB; 
	ID3D10Buffer* m_stramOutIB;

	ID3D10InputLayout* m_animationKeyInputLayout;
	ID3D10Buffer* m_modelAIPositionCBuffer;
	ID3D10Buffer* m_modelAIRotationCBuffer;
	ID3D10Buffer* m_modelAnimationInstanceBuffer;

	ID3D10Texture2D* m_SkinVertexTexCoordTex;
	ID3D10ShaderResourceView* m_SkinVertexTexCoordTexView;

	ID3D10Texture2D* m_SkinVertexPosTex;
	ID3D10ShaderResourceView* m_SkinVertexPosTexView;

	ID3D10Texture2D* m_SkinVertexNormalTex;
	ID3D10ShaderResourceView* m_SkinVertexNormalTexView;

	ID3D10Texture2D* m_SkinVertexBoneIndexTex;
	ID3D10ShaderResourceView* m_SkinVertexBoneIndexTexView;

	ID3D10Texture2D* m_ShadowMap;
	ID3D10RenderTargetView* m_ShadowMapView;
	ID3D10ShaderResourceView* m_ShadowMapRV;
private:
	bool Load(ID3D10Device* pdev, BoneMatrixConstantType t);
public:
	GPUModel();
	GPUModel(unsigned int max_a_draw);
	virtual ~GPUModel();
	bool Load(ID3D10Device* pdev, const TCHAR *name, const TCHAR *texture_path, LPCWSTR shader, BoneMatrixConstantType t);
	bool Load(ID3D10Device* pdev, const TCHAR *name, const TCHAR *texture_path, ID3D10Effect *eff, BoneMatrixConstantType t);
	virtual bool CreateInputLayout(ID3D10Device* pdev); 
	virtual bool CreateVertexBuffer(ID3D10Device* pdev);
	virtual bool CreateResources(ID3D10Device* pdev);	
	virtual bool CreateAnimationBuffer(ID3D10Device* pdev);	
	virtual bool CreateIndexBuffer(ID3D10Device* pdev) ;

	void SetShadowmapAsRenderTarget(ID3D10Device* pdev, bool bclear);
	void AnimationPass(ID3D10Device* pdev, AI* ai, int base, int num);
	void StreamOutPass(ID3D10Device* pdev, int base, int num);
	void RenderStreamOutShadowPass(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos); 
	void RenderStreamOutPass(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos);
	void Render(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, int base, int num);
	void RenderShadow(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, int base, int num);

	ID3D10ShaderResourceView* GetShadowMapRV() {return m_ShadowMapRV;};
};

#endif