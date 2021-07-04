//-----------------------------------------------------------------------------
// File: D3D10\Skin Animation\App.h
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

#define FRAMEWORK_VERSION 1
#include "../Framework/Version.h"

#include "../Framework/D3D10/D3D10App.h"
#include "GPUModel.h"

#define D2R(d)				(3.141590f*d/180.0f)
#define MAX_NUM_MODELS		65536

#define __USE_CONSTANT__	0
#define __EFFECT_CBUFFER__	1

#define RUN_T0				0.0f
#define RUN_T1				40.0f

#define DIE_T0				41.0f
#define DIE_T1				60.0f
#define DIE_T2				75.0f

#define DANCE_T0			76.0f
#define DANCE_T1			224.0f

#define KICK_T0				226.0f
#define KICK_T1				340.0f

#define RESPAWN_T0			341.0f
#define RESPAWN_T1			395.0f

class FloorModel : public D3D10Model
{
public:
	void RenderWithShadow(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, const float4x4& litmvp, ID3D10ShaderResourceView* sm);	
};

class App : public D3D10App
{
public:
	const TCHAR *GetHomeDirectory(){ return _T("../D3D10/Skin Animation"); }

	void AdjustCameraPlanes(void);
	void CalculateSplitDistances(void);
	void CalculateCameraFrustumCorners(D3DXVECTOR3 *pPoints, float fScale);
	float4x4 CalculateLightForFrustum(D3DXVECTOR3 *pCorners);
	//void CalculateLightForFrustum(D3DXVECTOR3 *pCorners);

	bool Create();
	bool Load();
	void Unload();

	bool OnKeyPress(HWND hwnd, const unsigned int key, const bool pressed);
	void ResetCamera();

	void OnRender();
	void OnTestRender();

	void resetModels();
	void AILoop();

protected:
	ID3D10InputLayout *m_skyboxIL;
	ID3D10Resource *m_env;
	ID3D10ShaderResourceView *m_envSRV;
	ID3D10Buffer *m_skyboxVB;

	GPUModel* m_model;
	FloorModel* m_floor;
	AI m_modelAI[MAX_NUM_MODELS];

	D3DXVECTOR3 m_sceneAABB[8];	
	int m_numPSSM;
	float m_splitSchemeLambda;
	float* m_splitDistances;

	Camera m_lightCamera;
	int	m_numModels;	
	bool m_showHelp;
};

#endif
