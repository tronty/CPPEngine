//-----------------------------------------------------------------------------
// File: D3D10\Skin Animation\App.cpp
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------
// This sample uses GPU to compute skinning animation and ouput the result by
// the stream out feature of DX10. The sample also uses instancing feature to
// reduce the small batch problem, so the sample only call one draw function
// to resolve the animation of all m_models. After resolving the skinning animation,
// another draw call is used to stream out all skinned vertex data for following
// shadow rendering. We also apply the parallel spliter shadow map on this sample,
// so we will need to render shadows for multiple times. By using SO, we can save
// the VS power because we only skin vertices once and render them multiple times.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "App.h"

#define RANGE                           600
#define SCALE                           0.02f
#define ANIMATION_STEP                  2.0f

D3D10App *app = new App();

//-----------------------------------------------------------------------------------------
// Convert quaterion to float4X4 matrix
//-----------------------------------------------------------------------------------------
void Quaternion2float4x4(float4x4 &m, float4 &q)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0f;

	xs = q.x*s;        ys = q.y*s;        zs = q.z*s;
	wx = q.w*xs;       wy = q.w*ys;       wz = q.w*zs;
	xx = q.x*xs;       xy = q.x*ys;       xz = q.x*zs;
	yy = q.y*ys;       yz = q.y*zs;       zz = q.z*zs;

	m.elem[0][0] = 1.0f - (yy+zz);
	m.elem[0][1] = xy + wz;
	m.elem[0][2] = xz - wy;

	m.elem[1][0] = xy - wz;
	m.elem[1][1] = 1.0f - (xx+zz);
	m.elem[1][2] = yz + wx;
	
	m.elem[2][0] = xz + wy;
	m.elem[2][1] = yz - wx;
	m.elem[2][2] = 1.0f - (xx + yy);
}
//-----------------------------------------------------------------------------------------
// New m_floor rendering function, render m_floor with PSSM.
//-----------------------------------------------------------------------------------------
void FloorModel::RenderWithShadow(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, const float4x4& litmvp, ID3D10ShaderResourceView* sm)
{
	UINT stride[1];
	UINT offset[1] = {0};

	pdev->IASetInputLayout(m_IL);
	pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	stride[0] = sizeof(STATICVERTEX);
	pdev->IASetVertexBuffers(0, 1, &m_VB, stride, offset);
	for (unsigned int i = 0; i < m_numMeshs; i++)
	{
		m_ppMeshEffect[i]->GetVariableByName("mvp")->AsMatrix()->SetMatrix((float *)&mvp);
		m_ppMeshEffect[i]->GetVariableByName("lightMvp")->AsMatrix()->SetMatrix((float *)&litmvp);
		m_ppMeshEffect[i]->GetVariableByName("cameraPos")->AsVector()->SetFloatVector((float *)&cmaerapos);
		if (m_pMesh[i].GetMaterialIndex() >= 0)
		{
			m_ppMeshEffect[i]->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource(m_pD3D10Material[m_pMesh[i].GetMaterialIndex()].diffuseMapView);
			m_ppMeshEffect[i]->GetVariableByName("bumpMap")->AsShaderResource()->SetResource(m_pD3D10Material[m_pMesh[i].GetMaterialIndex()].bumpMapView);
		}
		m_ppMeshEffect[i]->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(sm);
		m_ppMeshEffect[i]->GetTechniqueByName("techShadowFloor")->GetPassByName("P0")->Apply(0);
		for (unsigned int p = 0; p < m_pMesh[i].GetNumPrimitives(); p++)
		{
			pdev->IASetPrimitiveTopology(m_pMesh[i].GetPrimitive(p).type);
			pdev->DrawIndexed(m_pMesh[i].GetPrimitive(p).numIndices, m_pMesh[i].GetPrimitive(p).baseIndex, 0);
		}
	}
	m_ppMeshEffect[0]->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(NULL);
	m_ppMeshEffect[0]->GetTechniqueByName("techShadowFloor")->GetPassByName("P0")->Apply(0);
}
//-----------------------------------------------------------------------------------------
// Create the appliction.
//-----------------------------------------------------------------------------------------
bool App::Create()
{
	// Create our rendering context. It'll take care of our D3D10 device and simplify some tasks.
	m_context = new D3D10Context();
	if (!m_context->Create(_T("GPU Skin Animation - D3D10"), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 800, 600, 4, false)) return false;

	// Let base-class initialize itself as well
	if (!D3D10App::Create()) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load m_model data and setup camera
//-----------------------------------------------------------------------------------------
bool App::Load()
{
	m_numModels = 1024*5;
	m_showHelp = false;

	// Load chracter m_model
	m_model = new GPUModel(1024*8);
	if (!(m_model->Load(m_context->GetDevice(), MODEL_PATH _T("npc/npc_01.am"), TEX_PATH, _T("SkinAnimation.fx"), GPU))) return false;
	m_model->SetRange(0, 396);

	// Load m_floor m_model
	m_floor = new FloorModel();
	if (!(m_floor->Load(m_context->GetDevice(), MODEL_PATH _T("floor.am"), TEX_PATH, _T("SkinAnimation.fx"), CONSTANT_BUFFER))) return false;
	m_floor->CreateResources(m_context->GetDevice());

	// Setup camera for light source
	m_lightCamera.SetPosition(float3(-3000*SCALE, 3000*SCALE, -3000*SCALE));
	m_lightCamera.LookAt(float3(0, 0, 0), float3(0, 1, 0));
	m_lightCamera.SetFrustumExtents(PI*0.5f, 500.0f*SCALE, 10000.0f*SCALE);
	m_lightCamera.SetViewport(1024, 1024);

	// Setup camera
	m_camera.SetPosition(float3(0.0f, 500*SCALE, 500*SCALE));
	m_camera.SetRotation(0.7f, PI, 0.0f);
	m_camera.SetFrustumExtents(PI*0.5f, 0.1f, 5000.0f);
	m_camera.SetViewport(800, 600);

	// Setup the AABB for the whole scene
	m_sceneAABB[0]=D3DXVECTOR3(-RANGE*SCALE*5.0f, -RANGE*SCALE*0.1f, -RANGE*SCALE*5.0f);
	m_sceneAABB[1]=D3DXVECTOR3( RANGE*SCALE*5.0f, -RANGE*SCALE*0.1f, -RANGE*SCALE*5.0f);
	m_sceneAABB[2]=D3DXVECTOR3(-RANGE*SCALE*5.0f, -RANGE*SCALE*0.1f,  RANGE*SCALE*5.0f);
	m_sceneAABB[3]=D3DXVECTOR3( RANGE*SCALE*5.0f, -RANGE*SCALE*0.1f,  RANGE*SCALE*5.0f);
	m_sceneAABB[4]=D3DXVECTOR3(-RANGE*SCALE*5.0f,  RANGE*SCALE*0.1f, -RANGE*SCALE*5.0f);
	m_sceneAABB[5]=D3DXVECTOR3( RANGE*SCALE*5.0f,  RANGE*SCALE*0.1f, -RANGE*SCALE*5.0f);
	m_sceneAABB[6]=D3DXVECTOR3(-RANGE*SCALE*5.0f,  RANGE*SCALE*0.1f,  RANGE*SCALE*5.0f);
	m_sceneAABB[7]=D3DXVECTOR3( RANGE*SCALE*5.0f,  RANGE*SCALE*0.1f,  RANGE*SCALE*5.0f);

	// Setup parameters for PSSM
	m_numPSSM = 3;
	m_splitSchemeLambda = 0.5f;
	m_splitDistances = new float[m_numPSSM+1];

	resetModels();

	// Load textures
	if ((m_env = m_context->LoadTexture(TEX_PATH _T("Park.dds"), &m_envSRV)) == NULL) return false;

	// Create a quad
	float2 vertices[] =
	{
		float2( 0,  2),
		float2( 3, -1),
		float2(-3, -1),
	};
	if ((m_skyboxVB = m_context->CreateVertexBuffer(sizeof(vertices), D3D10_USAGE_IMMUTABLE, vertices)) == NULL) return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC skyboxLayout[] =
	{
		{ "Vertex", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	if ((m_skyboxIL = m_context->CreateInputLayout(m_model->GetMeshEffect(0)->GetTechniqueByName("Skybox")->GetPassByIndex(0), skyboxLayout, elementsOf(skyboxLayout))) == NULL) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::Unload()
{
	SAFE_RELEASE(m_env);
	SAFE_RELEASE(m_envSRV)
	SAFE_RELEASE(m_skyboxIL);
	SAFE_RELEASE(m_skyboxVB);

	delete[] m_splitDistances;
	delete m_model;
	delete m_floor;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::ResetCamera()
{
	// Set start position and rotation
	m_camera.SetPosition(float3(0.0f, 500*SCALE, 500*SCALE));
	m_camera.SetRotation(0.7f, PI, 0.0f);

	resetModels();
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::OnKeyPress(HWND hwnd, const unsigned int key, const bool pressed)
{
	// Let framework process key first
	if (D3D10App::OnKeyPress(hwnd, key, pressed)) return true;

	if (pressed)
	{
		switch (key)
		{		
		case VK_ADD: // Cycle wall color
			m_numModels += 256;
			if (m_numModels > 1024*8)
				m_numModels = 1024*8;
			return true;
		case VK_SUBTRACT: // Cycle wall color
			m_numModels -= 256;
			if (m_numModels < 256)
				m_numModels = 256;
			return true;
		case VK_F1: // Toggle help display
			m_showHelp = !m_showHelp;
			return true;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------
// Reset m_model's AI data.
//-----------------------------------------------------------------------------------------
void App::resetModels()
{
	for (int x = 0; x < MAX_NUM_MODELS; x++)
	{
		m_modelAI[x].bBounce = false;
		m_modelAI[x].bInSafeZone = true;
		m_modelAI[x].Deg = (float)(rand() % 360);
		m_modelAI[x].Speed = (1.5f + rand() % 50 / 10.0f)*SCALE;
		m_modelAI[x].Rotation.x = sinf(D2R(m_modelAI[x].Deg/2)) * 0.0f;
		m_modelAI[x].Rotation.y = sinf(D2R(m_modelAI[x].Deg/2)) * 1.0f;
		m_modelAI[x].Rotation.z = sinf(D2R(m_modelAI[x].Deg/2)) * 0.0f;
		m_modelAI[x].Rotation.w = cosf(D2R(m_modelAI[x].Deg/2));
		m_modelAI[x].Trans = transpose(Identity4());
		Quaternion2float4x4(m_modelAI[x].Trans, m_modelAI[x].Rotation);
		m_modelAI[x].Trans.elem[3][0] = (float)(rand()%RANGE-RANGE/2)*SCALE;
		m_modelAI[x].Trans.elem[3][1] = 0.0f*SCALE;
		m_modelAI[x].Trans.elem[3][2] = (float)(rand()%RANGE-RANGE/2)*SCALE;

		m_modelAI[x].time = (float)(rand() % ((int)RUN_T1)) + 1.0f;

		m_modelAI[x].bBounceByGround = false;

		m_modelAI[x].currentState = RUN;
	}
}
//-----------------------------------------------------------------------------------------
// Adjusts the camera planes to contain the visible scene
// as tightly as possible. This implementation is not very
// accurate.
//-----------------------------------------------------------------------------------------
void App::AdjustCameraPlanes(void)
{
	float4x4 vm = m_camera.GetModelView();
	vm = transpose(vm);

	// Find the most distant point of AABB
	float fMaxZ = 0;
	for(int i = 0; i < 8; i++)
	{
		// transform z coordinate with view matrix
		float fZ= m_sceneAABB[i].x*vm.elem[0][2]+m_sceneAABB[i].y*vm.elem[1][2]+m_sceneAABB[i].z*vm.elem[2][2]+1*vm.elem[3][2];

		// check if its largest
		if(fZ > fMaxZ)
			fMaxZ = fZ;
	}
	// use largest Z coordinate as new far plane
	float Far = fMaxZ + m_camera.GetzNear();
	m_camera.SetFrustumExtents(m_camera.GetFOV(), m_camera.GetzNear(), Far);
}
//-----------------------------------------------------------------------------------------
// Computes 8 corner points of the frustum of camera.
//-----------------------------------------------------------------------------------------
void App::CalculateCameraFrustumCorners(D3DXVECTOR3 *pPoints, float fScale)
{
	float4x4 vm = m_camera.GetModelView();
	vm = transpose(vm);
	float3 Pos = m_camera.GetPosition();
	D3DXVECTOR3 vSource = D3DXVECTOR3(Pos.x, Pos.y, Pos.z);
	D3DXVECTOR3 vTarget = vSource + D3DXVECTOR3(vm.elem[0][2], vm.elem[1][2], vm.elem[2][2]);
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1, 0);

	D3DXVECTOR3 vZ = vTarget-vSource;
	D3DXVec3Normalize(&vZ,&vZ);

	D3DXVECTOR3 vX;
	D3DXVec3Cross(&vX,&vUp,&vZ);
	D3DXVec3Normalize(&vX,&vX);

	D3DXVECTOR3 vY;
	D3DXVec3Cross(&vY,&vZ,&vX);

	float fNearPlaneHeight = tanf(m_camera.GetFOV() * 0.5f) * m_camera.GetzNear();
	float fNearPlaneWidth = fNearPlaneHeight * m_camera.GetAspect();

	float fFarPlaneHeight = tanf(m_camera.GetFOV() * 0.5f) * m_camera.GetzFar();
	float fFarPlaneWidth = fFarPlaneHeight * m_camera.GetAspect();

	D3DXVECTOR3 vNearPlaneCenter = vSource + vZ * m_camera.GetzNear();
	D3DXVECTOR3 vFarPlaneCenter = vSource + vZ * m_camera.GetzFar();

	pPoints[0]=D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth - vY*fNearPlaneHeight);
	pPoints[1]=D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[2]=D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[3]=D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth - vY*fNearPlaneHeight);

	pPoints[4]=D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth - vY*fFarPlaneHeight);
	pPoints[5]=D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[6]=D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[7]=D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth - vY*fFarPlaneHeight);

	// calculate center of points
	D3DXVECTOR3 vCenter(0,0,0);
	for (int i = 0; i < 8; i++)
		vCenter += pPoints[i];
	vCenter /= 8;

	// for each point
	for (int i = 0; i < 8; i++)
	{
		// scale by adding offset from center
		pPoints[i] += (pPoints[i] - vCenter) * (fScale - 1);
	}
}
//-----------------------------------------------------------------------------------------
// Calculates the frustum split distances, or in other
// words, the near and far planes for each split.
//-----------------------------------------------------------------------------------------
void App::CalculateSplitDistances(void)
{
	// Practical split scheme:
	//
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform

	m_splitSchemeLambda = clamp(m_splitSchemeLambda, 0.0f, 1.0f);

	for (int i = 0; i < m_numPSSM; i++)
	{
		float fIDM = i / (float)m_numPSSM;
		float fLog = m_camera.GetzNear()*powf((m_camera.GetzFar()/m_camera.GetzNear()),fIDM);
		float fUniform = m_camera.GetzNear()+(m_camera.GetzFar()-m_camera.GetzNear())*fIDM;
		m_splitDistances[i] = fLog*m_splitSchemeLambda+fUniform*(1-m_splitSchemeLambda);
	}

	// make sure border values are right
	m_splitDistances[0]=m_camera.GetzNear();
	m_splitDistances[m_numPSSM]=m_camera.GetzFar();
}
//-----------------------------------------------------------------------------------------
// Calculates the view and projection matrix for a light. The projection
// matrix is "zoomed in" on the given frustum split.
//-----------------------------------------------------------------------------------------
float4x4 App::CalculateLightForFrustum(D3DXVECTOR3 *pCorners)
{
	float4x4 VM, PM;

	float4x4 LightViewProj = transpose(m_lightCamera.GetModelViewProjection());

	// Next we will find the min and max values of the current
	// frustum split in lights post-projection space
	// (where coordinate range is from -1.0 to 1.0)
	float fMaxX = -FLT_MAX;
	float fMaxY = -FLT_MAX;
	float fMinX = FLT_MAX;
	float fMinY = FLT_MAX;
	float fMaxZ = 0.0f;

	// for each corner point
	for (int i = 0; i < 8; i++)
	{
		// transform point
		D3DXVECTOR4 vTransformed;
		D3DXVec3Transform(&vTransformed, &pCorners[i], (D3DXMATRIX*)&LightViewProj);

		// project x and y
		vTransformed.x /= vTransformed.w;
		vTransformed.y /= vTransformed.w;

		// find min and max values
		if(vTransformed.x>fMaxX) fMaxX=vTransformed.x;
		if(vTransformed.y>fMaxY) fMaxY=vTransformed.y;
		if(vTransformed.y<fMinY) fMinY=vTransformed.y;
		if(vTransformed.x<fMinX) fMinX=vTransformed.x;

		// find largest z distance
		if(vTransformed.z>fMaxZ) fMaxZ=vTransformed.z;
	}
	// set values to valid range (post-projection)
	fMaxX=clamp(fMaxX,-1.0f,1.0f);
	fMaxY=clamp(fMaxY,-1.0f,1.0f);
	fMinX=clamp(fMinX,-1.0f,1.0f);
	fMinY=clamp(fMinY,-1.0f,1.0f);

	// Adjust the far plane of the light to be at the farthest
	// point of the frustum split. Some bias may be necessary.
	float Near = m_lightCamera.GetzNear();
	float Far = fMaxZ + Near + 1.5f;
	m_lightCamera.SetFrustumExtents(m_lightCamera.GetFOV(), Near, Far);

	VM = transpose(m_lightCamera.GetModelView());
	PM = transpose(m_lightCamera.GetProjection());

	// Next we build a special matrix for cropping the lights view
	// to only contain points of the current frustum split
	float fScaleX=2.0f/(fMaxX-fMinX);
	float fScaleY=2.0f/(fMaxY-fMinY);

	float fOffsetX=-0.5f*(fMaxX+fMinX)*fScaleX;
	float fOffsetY=-0.5f*(fMaxY+fMinY)*fScaleY;

	float4x4 mCropView( fScaleX,    0.0f,       0.0f,   0.0f,
	                    0.0f,       fScaleY,    0.0f,   0.0f,
	                    0.0f,       0.0f,       1.0f,   0.0f,
	                    fOffsetX,   fOffsetY,   0.0f,   1.0f );

	// multiply the projection matrix with it
	PM = PM * mCropView;

	// finally modify projection matrix for linearized depth
	PM.elem[2][2] /= Far;
	PM.elem[3][2] /= Far;

	return (VM*PM);
}
//-----------------------------------------------------------------------------------------
// AI loop, control the movement of every character in the scene.
//-----------------------------------------------------------------------------------------
void App::AILoop()
{
	static float Grav = -0.7f*SCALE;

	for (int x = 0; x < m_numModels; x++)
	{
		float3      Dist = float3(m_modelAI[x].Trans.elem[3][0], m_modelAI[x].Trans.elem[3][1], m_modelAI[x].Trans.elem[3][2]) - m_camera.GetPosition();
		float       Len;

		Len = length(Dist);
		// if the character collide the camera
		if (Len < 150.0f*SCALE)
		{
			if (m_modelAI[x].currentState != DYING)
			{
				m_modelAI[x].currentState = DYING;
				m_modelAI[x].time = DIE_T0;
				m_modelAI[x].upSpeed = (8.0f + ((rand()%800)/100.0f))*SCALE;
				m_modelAI[x].flyDir = Dist/Len;
				m_modelAI[x].bDie = false;
				m_modelAI[x].bBounceByGround = false;
			}
		}
		else
		{
			if (m_modelAI[x].currentState == RUN)
			{
				if (!(rand()%600)) // Sometimes they want to dance
				{
					m_modelAI[x].currentState = DANCE;
					m_modelAI[x].time = DANCE_T0;
				}
				else if (!(rand()%600)) // Sometimes they want to kick
				{
					m_modelAI[x].currentState = KICK;
					m_modelAI[x].time = KICK_T0;
				}
			}
		}
	}

	for (int x = 0; x < m_numModels; x++)
	{
		if (m_modelAI[x].Deg > 360.0f)
		{
			m_modelAI[x].Deg -= 360.0f;
		}
		else if (m_modelAI[x].Deg < -360.0f)
		{
			m_modelAI[x].Deg += 360.0f;
		}

		switch (m_modelAI[x].currentState)
		{
			// if the character is running
			case RUN:
				// if the character is out of the boundary
				if (m_modelAI[x].bBounce)
				{
					// turn the character until he face to the origin point
					m_modelAI[x].Deg += 3.0f;
					float3    v1, v2;
					float     d;
					v1 = float3(m_modelAI[x].Trans.elem[3][0], 0.0f, m_modelAI[x].Trans.elem[3][2]);
					v2 = float3(m_modelAI[x].Trans.elem[2][0], 0.0f, m_modelAI[x].Trans.elem[2][2]);
					v1 = normalize(v1);
					d = dot(v1, v2);
					if (d > 0.9f)
						m_modelAI[x].bBounce = false;

					m_modelAI[x].Rotation.y = sinf(D2R(m_modelAI[x].Deg/2.0f));
					m_modelAI[x].Rotation.w = cosf(D2R(m_modelAI[x].Deg/2.0f));
					Quaternion2float4x4(m_modelAI[x].Trans, m_modelAI[x].Rotation);
				}
				else // if the character is inside the boundary
				{
					// turn a degree randomly 
					if (!(rand()%20))
					{
						m_modelAI[x].Deg += 3.0f;
						m_modelAI[x].Rotation.y = sinf(D2R(m_modelAI[x].Deg/2.0f));
						m_modelAI[x].Rotation.w = cosf(D2R(m_modelAI[x].Deg/2.0f));
						Quaternion2float4x4(m_modelAI[x].Trans, m_modelAI[x].Rotation);
					}
				}	
				// if the character is running and not dead, move the character
				m_modelAI[x].Trans.elem[3][0] -= m_modelAI[x].Trans.elem[2][0]*m_modelAI[x].Speed;
				m_modelAI[x].Trans.elem[3][1] -= m_modelAI[x].Trans.elem[2][1]*m_modelAI[x].Speed;
				m_modelAI[x].Trans.elem[3][2] -= m_modelAI[x].Trans.elem[2][2]*m_modelAI[x].Speed;
				m_modelAI[x].Position.x = m_modelAI[x].Trans.elem[3][0];
				m_modelAI[x].Position.y = m_modelAI[x].Trans.elem[3][1];
				m_modelAI[x].Position.z = m_modelAI[x].Trans.elem[3][2];
				break;

			// if the caharcter is dying
			case DYING: 
				if (!m_modelAI[x].bDie)
				{
					// charcater fly away and do some physical computing
					m_modelAI[x].Trans.elem[3][0] += m_modelAI[x].flyDir.x*10.0f*SCALE;
					m_modelAI[x].Trans.elem[3][2] += m_modelAI[x].flyDir.z*10.0f*SCALE;
					m_modelAI[x].Trans.elem[3][1] += m_modelAI[x].upSpeed;
					m_modelAI[x].Position.x = m_modelAI[x].Trans.elem[3][0];
					m_modelAI[x].Position.y = m_modelAI[x].Trans.elem[3][1];
					m_modelAI[x].Position.z = m_modelAI[x].Trans.elem[3][2];
					m_modelAI[x].upSpeed += Grav;
					if (m_modelAI[x].upSpeed < 0.0f)
					{
						if (m_modelAI[x].Trans.elem[3][1] <= 0.0f)
						{
							m_modelAI[x].Trans.elem[3][1] = 0.0f;
							if (m_modelAI[x].upSpeed < -6.0f*SCALE)
							{
								m_modelAI[x].upSpeed *= -0.6f;
								m_modelAI[x].bBounceByGround = true;
								m_modelAI[x].time = DIE_T1;
							}
							else
								m_modelAI[x].bDie = true;
						}
					}
				}
				break;
		}
		
		if (m_modelAI[x].bInSafeZone) //character in safe zone
		{
			if (m_modelAI[x].Trans.elem[3][0] > 2500.0f*SCALE || m_modelAI[x].Trans.elem[3][0] < -2500.0f*SCALE ||
				m_modelAI[x].Trans.elem[3][1] > 2500.0f*SCALE || m_modelAI[x].Trans.elem[3][1] < -2500.0f*SCALE ||
				m_modelAI[x].Trans.elem[3][2] > 2500.0f*SCALE || m_modelAI[x].Trans.elem[3][2] < -2500.0f*SCALE)
			{
				// charcater out of safe zone, then need to turn him back into the boundary
				m_modelAI[x].bBounce = true;
				m_modelAI[x].bInSafeZone = false;
			}
		}
		else
		{
			if (m_modelAI[x].Trans.elem[3][0] < 2000.0f*SCALE && m_modelAI[x].Trans.elem[3][0] > -2000.0f*SCALE &&
				m_modelAI[x].Trans.elem[3][1] < 2000.0f*SCALE && m_modelAI[x].Trans.elem[3][1] > -2000.0f*SCALE &&
				m_modelAI[x].Trans.elem[3][2] < 2000.0f*SCALE && m_modelAI[x].Trans.elem[3][2] > -2000.0f*SCALE)
			{
				// charcater out of the boundary, go into safe zone
				m_modelAI[x].bInSafeZone = true;
			}
		}
	}

	for (int x = 0; x < m_numModels; x++)
	{
		switch (m_modelAI[x].currentState)
		{
			case RUN:
				m_modelAI[x].time += ANIMATION_STEP;
				if (m_modelAI[x].time >= RUN_T1)
					m_modelAI[x].time = RUN_T0;
				break;

			case DYING:
				if (!m_modelAI[x].bDie)
				{
					m_modelAI[x].time += ANIMATION_STEP;
					if (m_modelAI[x].bBounceByGround)
					{
						if (m_modelAI[x].time >= DIE_T2)
						{
							m_modelAI[x].time = DIE_T2;
						}
					}
					else // if the character is dead, stop the animation at the last frame
					{
						if (m_modelAI[x].time >= DIE_T1)
						{
							m_modelAI[x].time = DIE_T1;
						}
					}
				}
				else
				{
					m_modelAI[x].currentState = RESPAWN;
					m_modelAI[x].time = RESPAWN_T0;
					m_modelAI[x].bDie = false;

				}
				break;

			// if the character is dancing
			case DANCE:
				m_modelAI[x].time += ANIMATION_STEP;
				if (m_modelAI[x].time >= DANCE_T1)
				{
					// Run again.
					m_modelAI[x].currentState = RUN;
					m_modelAI[x].time = RUN_T0;
				}
				break;

				// if the character is kicking
			case KICK:
				m_modelAI[x].time += ANIMATION_STEP;
				if (m_modelAI[x].time >= KICK_T1)
				{
					// Run again.
					m_modelAI[x].currentState = RUN;
					m_modelAI[x].time = RUN_T0;
				}
				break;

			// if the character is respawning
			case RESPAWN:
				m_modelAI[x].time += ANIMATION_STEP;
				if (m_modelAI[x].time >= RESPAWN_T1)
				{
					// Run again.
					m_modelAI[x].currentState = RUN;
					m_modelAI[x].time = RUN_T0;
				}
				break;
		}
	}
}
//-----------------------------------------------------------------------------------------
// Render the frame
//-----------------------------------------------------------------------------------------
void App::OnRender()
{
	static UINT NumViewports = 1;
	static D3D10_VIEWPORT CameraViewport;
	D3DXVECTOR3 pCorners[8];
	float4x4 MVP, LightMVP, LightMVPS;
	static float4x4 ScaleM(0.5, 0,    0, 0,
	                       0,   -0.5, 0, 0,
	                       0,   0,    1, 0,
	                       0.5, 0.5,  0, 1);
	static float4 clearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
	AILoop();

	UpdateCameraPosition(20.0f * m_frameTime);
	float3 Pos = m_camera.GetPosition();
	if (Pos.y < 1.0f)
	{
		Pos.y = 1.0f;
		m_camera.SetPosition(Pos);
	}	
	// Position the camera far plane as near as possible to minimize the amount of empty space
	AdjustCameraPlanes();

	// Calculate the distances of split planes according to the selected split scheme
	CalculateSplitDistances();

	m_context->Clear(clearColor, D3D10_CLEAR_DEPTH, 1.0f, 0);

	// Draw Skybox
	m_context->SetEffect(m_model->GetMeshEffect(0));
	m_context->SetTexture("Env", m_envSRV);
	m_model->GetMeshEffect(0)->GetVariableByName("clipToEnv")->AsMatrix()->SetMatrix((float *)&(!m_camera.GetSkyboxMVP()));
	m_model->GetMeshEffect(0)->GetTechniqueByName("Skybox")->GetPassByIndex(0)->Apply(0);
	m_context->GetDevice()->IASetInputLayout(m_skyboxIL);
	// Setup buffer and topology
	UINT stride = sizeof(float2), offset = 0;
	m_context->GetDevice()->IASetVertexBuffers(0, 1, &m_skyboxVB, &stride, &offset);
	m_context->GetDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->GetDevice()->Draw(3, 0);

	// do animation on the GPU
	m_model->AnimationPass(m_context->GetDevice(), m_modelAI, 0, m_numModels);
	// stream out skinned vertices
	m_model->StreamOutPass(m_context->GetDevice(), 0, m_numModels);
	for (int PSSMIndex = 0; PSSMIndex < m_numPSSM; PSSMIndex++)
	{
		// near and far planes for current frustum split
		float fNear = m_splitDistances[PSSMIndex];
		float fFar = m_splitDistances[PSSMIndex+1];

		m_camera.SetFrustumExtents(PI*0.5f, fNear, fFar);

		// Calculate corner points of frustum split.
		// To avoid edge problems, scale the frustum so that it's at least a few pixels larger
		CalculateCameraFrustumCorners(pCorners, 1.1f);

		// Calculate view and projection matrices
		LightMVP = CalculateLightForFrustum(pCorners);
		LightMVPS = LightMVP * ScaleM;

		m_model->SetShadowmapAsRenderTarget(m_context->GetDevice(), true);
		m_model->RenderStreamOutShadowPass(m_context->GetDevice(), LightMVP, m_lightCamera.GetPosition());

		m_context->SetRTToBackBuffer();
		m_context->GetDevice()->RSGetViewports(&NumViewports, &CameraViewport);
		CameraViewport.MinDepth = PSSMIndex/(float)m_numPSSM;
		CameraViewport.MaxDepth = (PSSMIndex+1)/(float)m_numPSSM;
		m_context->GetDevice()->RSSetViewports(1, &CameraViewport);
		MVP = transpose(m_camera.GetModelViewProjection());
		m_floor->RenderWithShadow(m_context->GetDevice(), MVP, m_camera.GetPosition(), LightMVPS, m_model->GetShadowMapRV());
	}
	m_context->GetDevice()->RSGetViewports(&NumViewports, &CameraViewport);
	// as long as ranges are in order and don't overlap it should be all good...
	CameraViewport.MinDepth = 0;
	CameraViewport.MaxDepth = 1;
	m_context->GetDevice()->RSSetViewports(1, &CameraViewport);
	m_camera.SetFrustumExtents(PI*0.5f, 0.1f, 50000.0f);
	MVP = transpose(m_camera.GetModelViewProjection());
	m_model->RenderStreamOutPass(m_context->GetDevice(), MVP, m_camera.GetPosition());

	RenderGUI();

	float4 color(1.0f, 1.0f, 1.0f, 1.0f);
	char Buffer[64];
	if (m_showHelp)
	{		
		m_mainFont.DrawText(
			"Controls:\n\n"
			"-/+ - dec/inc number of characters\n",
			-0.65f, 0.0f, 0.1f, 0.1f, HA_LEFT, VA_CENTER, &color);
	}
	sprintf(Buffer, "%d characters", m_numModels);
	int w = m_context->GetWidth();
	int h = m_context->GetHeight();
	m_mainFont.DrawText(Buffer, 10.0f / w - 0.7f, 1 - 10.0f / h, 80.0f / w, 60.0f / h, HA_LEFT, VA_TOP, &color);

	m_context->Present();
}
