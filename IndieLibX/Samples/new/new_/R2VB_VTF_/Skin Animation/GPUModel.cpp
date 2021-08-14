//-----------------------------------------------------------------------------
// File: D3D10\Skin Animation\GPUModel.cpp
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "GPUModel.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Framework/D3D10/D3D10Context.h"


#define BUFFER_SIZE			2048
#define SHADOWMAP_SIZE		2048

//-----------------------------------------------------------------------------------------
// Animation key input layout for each instance.
//-----------------------------------------------------------------------------------------
const D3D10_INPUT_ELEMENT_DESC AnimationKeyInputLayout[] =
{
	{ "MODELROT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	0,	D3D10_INPUT_PER_INSTANCE_DATA, 1 },	
	{ "MODELPOS",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	16,	D3D10_INPUT_PER_INSTANCE_DATA, 1 },	
};
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
GPUModel::GPUModel()
{	
	m_MaxADraw = MAX_MODELS_A_DRAW;
	m_techGPU = NULL;
	m_RotationKeysBufferVar = NULL;
	m_PositionKeysBufferVar = NULL;
	m_ScaleKeysBufferVar = NULL;
	
	m_RotationKeysTex = NULL;
	m_PositionKeysTex = NULL;
	m_RotationKeysTexView = NULL;
	m_PositionKeysTexView = NULL;

	m_boneMatrixBuffer = NULL; 
	m_boneMatrixBufferRV = NULL;

	m_animationKeyInputLayout = NULL;
	m_modelAIPositionCBuffer = NULL;
	m_modelAIRotationCBuffer = NULL;
	m_modelAnimationInstanceBuffer = NULL;
}
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
GPUModel::GPUModel(unsigned int max_a_draw)
{	
	m_MaxADraw = max_a_draw;
	m_techGPU = NULL;
	m_RotationKeysBufferVar = NULL;
	m_PositionKeysBufferVar = NULL;
	m_ScaleKeysBufferVar = NULL;
	
	m_RotationKeysTex = NULL;
	m_PositionKeysTex = NULL;
	m_RotationKeysTexView = NULL;
	m_PositionKeysTexView = NULL;

	m_boneMatrixBuffer = NULL; 
	m_boneMatrixBufferRV = NULL;

	m_animationKeyInputLayout = NULL;
	m_modelAIPositionCBuffer = NULL;
	m_modelAIRotationCBuffer = NULL;
	m_modelAnimationInstanceBuffer = NULL;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
GPUModel::~GPUModel()
{
	SAFE_RELEASE(m_RotationKeysTex);
	SAFE_RELEASE(m_PositionKeysTex);
	SAFE_RELEASE(m_RotationKeysTexView);
	SAFE_RELEASE(m_PositionKeysTexView);

	SAFE_RELEASE(m_boneMatrixBuffer); 
	SAFE_RELEASE(m_boneMatrixBufferRV);

	SAFE_RELEASE(m_animationKeyInputLayout);
	SAFE_RELEASE(m_modelAIPositionCBuffer);
	SAFE_RELEASE(m_modelAIRotationCBuffer);
	SAFE_RELEASE(m_modelAnimationInstanceBuffer);

	SAFE_RELEASE(m_SkinVertexTexCoordTex);
	SAFE_RELEASE(m_SkinVertexTexCoordTexView);

	SAFE_RELEASE(m_SkinVertexPosTex);
	SAFE_RELEASE(m_SkinVertexPosTexView);

	SAFE_RELEASE(m_SkinVertexNormalTex);
	SAFE_RELEASE(m_SkinVertexNormalTexView);

	SAFE_RELEASE(m_SkinVertexBoneIndexTex);
	SAFE_RELEASE(m_SkinVertexBoneIndexTexView);

	SAFE_RELEASE(m_stramOutIL);
	SAFE_RELEASE(m_streamOutVB); 
	SAFE_RELEASE(m_stramOutIB);

	SAFE_RELEASE(m_ShadowMap);
	SAFE_RELEASE(m_ShadowMapView);
	SAFE_RELEASE(m_ShadowMapRV);
}
//-----------------------------------------------------------------------------------------
// Load model data from .AM file, create buffers. Effect file has been loaded by caller.
//-----------------------------------------------------------------------------------------
bool GPUModel::Load(ID3D10Device* pdev, const TCHAR *name, const TCHAR *texture_path, ID3D10Effect *eff, BoneMatrixConstantType t) 
{
	if (!D3D10Model::Load(pdev, name, texture_path, eff, t))
		return false;
	if (!Load(pdev, t))
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load model data from .AM file, create buffers. Effect file has been loaded by caller.
//-----------------------------------------------------------------------------------------
bool GPUModel::Load(ID3D10Device* pdev, BoneMatrixConstantType t) 
{
	D3D10_BUFFER_DESC vbdesc;
	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	D3D10_PASS_DESC			PassDesc;
	D3D10_BUFFER_DESC		bd;

	switch (m_animationType)
	{
		case SKINNING_ANIMATION:			
			switch (m_boneMatrixConstantType)
			{
				case GPU:					
					//Buffer
					m_techGPU = m_ppMeshEffect[0]->GetTechniqueByName( "techGPUResolveSkinning" );
					m_techBuffer = m_ppMeshEffect[0]->GetTechniqueByName( "techGPUDrawMultiModels" );
					m_boneMatrixBufferVar = m_ppMeshEffect[0]->GetVariableByName( "boneMatrixBuffer" )->AsShaderResource();
					m_RotationKeysBufferVar = m_ppMeshEffect[0]->GetVariableByName( "rotationKeys" )->AsShaderResource();
					m_PositionKeysBufferVar = m_ppMeshEffect[0]->GetVariableByName( "positionKeys" )->AsShaderResource();

					ZeroMemory( &vbdesc, sizeof(vbdesc) );
					vbdesc.ByteWidth = m_numBones*m_MaxADraw*sizeof(float4x4);
					vbdesc.Usage = D3D10_USAGE_DEFAULT;
					vbdesc.BindFlags = D3D10_BIND_SHADER_RESOURCE|D3D10_BIND_STREAM_OUTPUT;
					vbdesc.CPUAccessFlags = 0;
					vbdesc.MiscFlags = 0;
					pdev->CreateBuffer(&vbdesc, NULL, &m_boneMatrixBuffer);
					
					ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
					SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_BUFFER;
					SRVDesc.Buffer.ElementOffset = 0;
					SRVDesc.Buffer.ElementWidth = m_numBones*m_MaxADraw*4;
					pdev->CreateShaderResourceView(m_boneMatrixBuffer, &SRVDesc, &m_boneMatrixBufferRV);

					ZeroMemory( &vbdesc, sizeof(vbdesc) );
					vbdesc.ByteWidth = m_numTriangles*3*m_MaxADraw*sizeof(GPUSTREAMOUTVERTEX);
					vbdesc.Usage = D3D10_USAGE_DEFAULT;
					vbdesc.BindFlags = D3D10_BIND_VERTEX_BUFFER|D3D10_BIND_STREAM_OUTPUT;
					vbdesc.CPUAccessFlags = 0;
					vbdesc.MiscFlags = 0;
					pdev->CreateBuffer(&vbdesc, NULL, &m_streamOutVB);		

					m_techGPU->GetPassByIndex( 0 )->GetDesc( &PassDesc );
					if (FAILED( pdev->CreateInputLayout( AnimationKeyInputLayout, (sizeof(AnimationKeyInputLayout)/sizeof(AnimationKeyInputLayout[0])), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_animationKeyInputLayout)) )
						return false;
					
					bd.Usage = D3D10_USAGE_DYNAMIC;
					bd.ByteWidth = m_MaxADraw*sizeof(ANIMATIONPASSINSTANCEDATA);
					bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
					bd.MiscFlags = 0;			
					if (FAILED(pdev->CreateBuffer(&bd, NULL, &m_modelAnimationInstanceBuffer))) 
					{
						return false;
					}
					break;
			}
			break;
	}	
	

	if (!CreateResources(pdev))
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load model data from .AM file, create buffers. Effect file will be loaded by this function.
//-----------------------------------------------------------------------------------------
bool GPUModel::Load(ID3D10Device* pdev, const TCHAR *name, const TCHAR *texture_path, LPCWSTR shader, BoneMatrixConstantType t) 
{
	if (!D3D10Model::Load(pdev, name, texture_path, shader, t))
		return false;
	if (!Load(pdev, t))
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Create input layout for this model.
//-----------------------------------------------------------------------------------------
bool GPUModel::CreateInputLayout(ID3D10Device* pdev) 
{
	D3D10Model::CreateInputLayout(pdev);
	
	const D3D10_INPUT_ELEMENT_DESC GPUStreamOutayout[] =
    {
		{ "SKINNED",			0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,	0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",				0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,	16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

	D3D10_PASS_DESC PassDesc;
	switch (m_animationType)
	{
		case SKINNING_ANIMATION:
			switch (m_boneMatrixConstantType)
			{
				case GPU:
					m_ppMeshEffect[0]->GetTechniqueByName( "techGPUDrawStreamoutModels" )->GetPassByIndex( 0 )->GetDesc( &PassDesc );
					if (FAILED( pdev->CreateInputLayout( GPUStreamOutayout, (sizeof(GPUStreamOutayout)/sizeof(GPUStreamOutayout[0])), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_stramOutIL)) )
						return false;
					break;
			}
			break;
	}
	return true;
}
//-----------------------------------------------------------------------------------------
// Create vertex buffer for this model.
//-----------------------------------------------------------------------------------------
bool GPUModel::CreateVertexBuffer(ID3D10Device* pdev) 
{
	D3D10Model::CreateVertexBuffer(pdev);

	switch (m_animationType)
	{
		case SKINNING_ANIMATION:
			switch (m_boneMatrixConstantType)
			{
				case GPU:
					D3D10_TEXTURE2D_DESC t2dd;
					D3D10_SUBRESOURCE_DATA subinit;

					// Create vertex position texture
					float4 *SkinVertexPos = new float4[512 * 2];
					for (unsigned int i=0; i<m_numVertices; i++) 
					{
						SkinVertexPos[i].x = m_pVSkinningData[i].pBlendData[0].offset.x;
						SkinVertexPos[i].y = m_pVSkinningData[i].pBlendData[0].offset.y;
						SkinVertexPos[i].z = m_pVSkinningData[i].pBlendData[0].offset.z;
						SkinVertexPos[i].w = m_pVSkinningData[i].pBlendData[0].weight;
					}
					t2dd.Width = 512;
					t2dd.Height = 2;
					t2dd.MipLevels = 1;
					t2dd.ArraySize = 1;
					t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					t2dd.SampleDesc.Count = 1;
					t2dd.SampleDesc.Quality = 0;
					t2dd.Usage = D3D10_USAGE_IMMUTABLE;
					t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
					t2dd.CPUAccessFlags = 0;
					t2dd.MiscFlags = 0;
					subinit.pSysMem = SkinVertexPos;
					subinit.SysMemPitch = 512*sizeof(float4);			
					pdev->CreateTexture2D(&t2dd, &subinit, &m_SkinVertexPosTex);
					delete [] SkinVertexPos;

					D3D10_TEXTURE2D_DESC desc;
					D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;

					m_SkinVertexPosTex->GetDesc(&desc);
					ZeroMemory(&srvDesc, sizeof(srvDesc));
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = desc.MipLevels;
					pdev->CreateShaderResourceView(m_SkinVertexPosTex, &srvDesc, &m_SkinVertexPosTexView);

					// Create vertex normal texture
					float4 *SkinVertexNor = new float4[m_numVertices];
					for (unsigned int i=0; i<m_numVertices; i++) 
					{
						SkinVertexNor[i].x = m_pVNormal[i].x;
						SkinVertexNor[i].y = m_pVNormal[i].y;
						SkinVertexNor[i].z = m_pVNormal[i].z;
						if (m_pVSkinningData[i].numBones > 1)
							SkinVertexNor[i].w = m_pVSkinningData[i].pBlendData[1].weight;
						else
							SkinVertexNor[i].w = 0.0f;
					}
					t2dd.Width = 512;
					t2dd.Height = 2;
					t2dd.MipLevels = 1;
					t2dd.ArraySize = 1;
					t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					t2dd.SampleDesc.Count = 1;
					t2dd.SampleDesc.Quality = 0;
					t2dd.Usage = D3D10_USAGE_IMMUTABLE;
					t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
					t2dd.CPUAccessFlags = 0;
					t2dd.MiscFlags = 0;
					subinit.pSysMem = SkinVertexNor;
					subinit.SysMemPitch = 512*sizeof(float4);			
					pdev->CreateTexture2D(&t2dd, &subinit, &m_SkinVertexNormalTex);
					delete [] SkinVertexNor;

					m_SkinVertexNormalTex->GetDesc(&desc);
					ZeroMemory(&srvDesc, sizeof(srvDesc));
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = desc.MipLevels;
					pdev->CreateShaderResourceView(m_SkinVertexNormalTex, &srvDesc, &m_SkinVertexNormalTexView);

					// Create vertex texture coordinate texture
					float4 *SkinVertex = new float4[m_numVertices];
					for (unsigned int i=0; i<m_numVertices; i++) 
					{
						SkinVertex[i].x = m_pVTexcoord[i].x;
						SkinVertex[i].y = m_pVTexcoord[i].y;
						if (m_pVSkinningData[i].numBones > 2)
							SkinVertex[i].z = m_pVSkinningData[i].pBlendData[2].weight;
						else
							SkinVertex[i].z = 0.0f;

						if (m_pVSkinningData[i].numBones > 3)
							SkinVertex[i].w = m_pVSkinningData[i].pBlendData[3].weight;
						else
							SkinVertex[i].w = 0.0f;
					}
					t2dd.Width = 512;
					t2dd.Height = 2;
					t2dd.MipLevels = 1;
					t2dd.ArraySize = 1;
					t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					t2dd.SampleDesc.Count = 1;
					t2dd.SampleDesc.Quality = 0;
					t2dd.Usage = D3D10_USAGE_IMMUTABLE;
					t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
					t2dd.CPUAccessFlags = 0;
					t2dd.MiscFlags = 0;
					subinit.pSysMem = SkinVertex;
					subinit.SysMemPitch = 512*sizeof(float4);			
					pdev->CreateTexture2D(&t2dd, &subinit, &m_SkinVertexTexCoordTex);
					delete [] SkinVertex;

					m_SkinVertexTexCoordTex->GetDesc(&desc);
					ZeroMemory(&srvDesc, sizeof(srvDesc));
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = desc.MipLevels;
					pdev->CreateShaderResourceView(m_SkinVertexTexCoordTex, &srvDesc, &m_SkinVertexTexCoordTexView);							

					// Create vertex bone index texture	
					float *SkinVertexBone = new float[m_numVertices*4];
					for (unsigned int i=0; i<m_numVertices; i++) 
					{
						for (unsigned int j=0; j<4; j++)
						{
							if (j < m_pVSkinningData[i].numBones)
							{
								SkinVertexBone[i*4+j] = (float)m_pVSkinningData[i].pBlendData[j].bone;						
							}
							else
							{
								SkinVertexBone[i*4+j] = 0;
							}
						}
					}
					t2dd.Width = 512;
					t2dd.Height = 2;
					t2dd.MipLevels = 1;
					t2dd.ArraySize = 1;
					t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					t2dd.SampleDesc.Count = 1;
					t2dd.SampleDesc.Quality = 0;
					t2dd.Usage = D3D10_USAGE_IMMUTABLE;
					t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
					t2dd.CPUAccessFlags = 0;
					t2dd.MiscFlags = 0;
					subinit.pSysMem = SkinVertexBone;
					subinit.SysMemPitch = 512*sizeof(float)*4;			
					pdev->CreateTexture2D(&t2dd, &subinit, &m_SkinVertexBoneIndexTex);
					delete [] SkinVertexBone;

					m_SkinVertexBoneIndexTex->GetDesc(&desc);
					ZeroMemory(&srvDesc, sizeof(srvDesc));
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = desc.MipLevels;
					pdev->CreateShaderResourceView(m_SkinVertexBoneIndexTex, &srvDesc, &m_SkinVertexBoneIndexTexView);

					D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
					t2dd.Width = SHADOWMAP_SIZE;
					t2dd.Height = SHADOWMAP_SIZE;
					t2dd.MipLevels = 1;
					t2dd.ArraySize = 1;
					t2dd.Format = DXGI_FORMAT_R8_UNORM;
					t2dd.SampleDesc.Count = 1;
					t2dd.SampleDesc.Quality = 0;
					t2dd.Usage = D3D10_USAGE_DEFAULT;
					t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE|D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE;
					t2dd.CPUAccessFlags = 0;
					t2dd.MiscFlags = 0;
					pdev->CreateTexture2D(&t2dd, NULL, &m_ShadowMap);

					ZeroMemory(&rtDesc, sizeof(rtDesc));
					rtDesc.Format = t2dd.Format;
					rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
					rtDesc.Texture2D.MipSlice = 0;
					pdev->CreateRenderTargetView(m_ShadowMap, &rtDesc, &m_ShadowMapView);

					ZeroMemory(&srvDesc, sizeof(srvDesc));
					srvDesc.Format = t2dd.Format;
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = t2dd.MipLevels;
					pdev->CreateShaderResourceView(m_ShadowMap, &srvDesc, &m_ShadowMapRV);
					break;				
			}
			break;		
	}
	return true;
}
//-----------------------------------------------------------------------------------------
// Create index buffer and stream out index buffer for this model.
//-----------------------------------------------------------------------------------------
bool GPUModel::CreateIndexBuffer(ID3D10Device* pdev) 
{	
	if (!D3D10Model::CreateIndexBuffer(pdev))
		return false;

	D3D10_BUFFER_DESC bd;
	D3D10_SUBRESOURCE_DATA initData;
	DWORD numIndices;
	unsigned int* iDest; 
	unsigned int* oiDest;

	numIndices = 0;	
	for (unsigned int m=0; m<m_MaxADraw; m++)
	{
		for (unsigned int i=0; i<m_numMeshs; i++)
		{
			for (unsigned int j=0; j<m_pMesh[i].GetNumPrimitives(); j++)
			{
				numIndices += m_pMesh[i].GetPrimitive(j).numIndices;	
			}
		}
	}

	oiDest = iDest = new unsigned int[numIndices];
	for (unsigned int m=0; m<m_MaxADraw; m++)
	{
		for (unsigned int i=0; i<m_numMeshs; i++)
		{
			for (unsigned int j=0; j<m_pMesh[i].GetNumPrimitives(); j++)
			{
				for (unsigned int k=0; k<m_pMesh[i].GetPrimitive(j).numIndices; k++)
				{
					(*iDest++) = m_pMesh[i].GetPrimitive(j).pIndices[k] + m*m_numVertices;
				}
			}
		}
	}
	// Create the index buffer
	bd.Usage = D3D10_USAGE_IMMUTABLE;
	bd.ByteWidth = numIndices*sizeof(DWORD);
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = oiDest;
	if (FAILED(pdev->CreateBuffer(&bd, &initData, &m_stramOutIB))) 
	{
		delete [] oiDest;
		return false;
	}
	delete [] oiDest;
	return true;
}
//-----------------------------------------------------------------------------------------
// Create all resources for this model.
//-----------------------------------------------------------------------------------------
bool GPUModel::CreateResources(ID3D10Device* pdev) 
{		
	if (!CreateInputLayout(pdev)) return false;
	if (!CreateVertexBuffer(pdev)) return false;
	if (!CreateIndexBuffer(pdev)) return false;
	if (!CreateAnimationBuffer(pdev)) return false;
	
	return true;
}
//-----------------------------------------------------------------------------------------
// Create animation buffer resource for this model.
//-----------------------------------------------------------------------------------------
bool GPUModel::CreateAnimationBuffer(ID3D10Device* pdev) 
{	
	int TotalNumKeys;

	switch (m_boneMatrixConstantType)
	{
		case GPU:			
			float4 *orotkey, *rotkey;
			float4 *oposkey, *poskey;
			TotalNumKeys = m_pBone[0].GetAnimation().GetRotationKeys().pKey[m_pBone[0].GetAnimation().GetRotationKeys().numKeys-1].endTime;
			orotkey = rotkey = new float4[TotalNumKeys*m_numBones];
			oposkey = poskey = new float4[TotalNumKeys*m_numBones];
			for (int i=0; i<TotalNumKeys; ++i)
			{
				SetTime((float)i);
				Play(0.0f);
				for (uint j=0; j<m_numBones; ++j)
				{
					float4x4 M = m_pBone[j].GetTransformMatrix();
					(*poskey).x = M.elem[3][0];
					(*poskey).y = M.elem[3][1];
					(*poskey).z = M.elem[3][2];
					poskey++;
					Mat42Quaternion((*rotkey), M);
					rotkey++;
				}
			}	
			
			D3D10_TEXTURE2D_DESC t2dd;
			D3D10_SUBRESOURCE_DATA subinit;
			rotkey = new float4[BUFFER_SIZE*BUFFER_SIZE];
			stx_memcpy(rotkey, orotkey, TotalNumKeys*m_numBones*sizeof(float4));			
			t2dd.Width = BUFFER_SIZE;
			t2dd.Height = BUFFER_SIZE;
			t2dd.MipLevels = 1;
			t2dd.ArraySize = 1;
			t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			t2dd.SampleDesc.Count = 1;
			t2dd.SampleDesc.Quality = 0;
			t2dd.Usage = D3D10_USAGE_IMMUTABLE;
			t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			t2dd.CPUAccessFlags = 0;
			t2dd.MiscFlags = 0;
			subinit.pSysMem = rotkey;
			subinit.SysMemPitch = BUFFER_SIZE*sizeof(float4);			
			pdev->CreateTexture2D(&t2dd, &subinit, &m_RotationKeysTex);
			delete [] orotkey;
			delete [] rotkey;

			D3D10_TEXTURE2D_DESC desc;
			D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;

			m_RotationKeysTex->GetDesc(&desc);
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			pdev->CreateShaderResourceView(m_RotationKeysTex, &srvDesc, &m_RotationKeysTexView); 

			poskey = new float4[BUFFER_SIZE*BUFFER_SIZE];
			stx_memcpy(poskey, oposkey, TotalNumKeys*m_numBones*sizeof(float4));			
			t2dd.Width = BUFFER_SIZE;
			t2dd.Height = BUFFER_SIZE;
			t2dd.MipLevels = 1;
			t2dd.ArraySize = 1;
			t2dd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			t2dd.SampleDesc.Count = 1;
			t2dd.SampleDesc.Quality = 0;
			t2dd.Usage = D3D10_USAGE_IMMUTABLE;
			t2dd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			t2dd.CPUAccessFlags = 0;
			t2dd.MiscFlags = 0;
			subinit.pSysMem = poskey;
			subinit.SysMemPitch = BUFFER_SIZE*sizeof(float4);			
			pdev->CreateTexture2D(&t2dd, &subinit, &m_PositionKeysTex);
			delete [] poskey;
			delete [] oposkey;

			m_PositionKeysTex->GetDesc(&desc);
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			pdev->CreateShaderResourceView(m_PositionKeysTex, &srvDesc, &m_PositionKeysTexView);
			break;
	}
	return true;
}
//-----------------------------------------------------------------------------------------
// Do animation pass on the GPU. 
//-----------------------------------------------------------------------------------------
void GPUModel::AnimationPass(ID3D10Device* pdev, AI* ai, int base, int num) 
{
	UINT stride[1];
	UINT offset[1] = {0};
	float4 LastKey;
	float4 Data[31];
	ID3D10Buffer* Buffers[1];
		
	// Bind m_boneMatrixBuffer as output buffer.
	Buffers[0] = m_boneMatrixBuffer;
	pdev->SOSetTargets(1, Buffers, offset);	

	pdev->IASetInputLayout(m_animationKeyInputLayout);		
	ANIMATIONPASSINSTANCEDATA* pData = NULL;
	m_modelAnimationInstanceBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pData ); 
	for (int i=base; i<base+num; ++i)
	{
		pData[i-base].Position = float4(ai[i].Position, ai[i].time);
		pData[i-base].Rotation = ai[i].Rotation;
	}
	m_modelAnimationInstanceBuffer->Unmap();

	stride[0] = sizeof(ANIMATIONPASSINSTANCEDATA);
	Buffers[0] = m_modelAnimationInstanceBuffer;
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);		

	// Bind animation keys texture to this shader
	m_RotationKeysBufferVar->SetResource( m_RotationKeysTexView );
	m_PositionKeysBufferVar->SetResource( m_PositionKeysTexView );	
	
	pdev->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_techGPU->GetPassByName("P0")->Apply(0);
	pdev->DrawInstanced(m_numBones, num, 0, 0);

	Buffers[0] = NULL;
    pdev->SOSetTargets( 1, Buffers, offset );	
}
//-----------------------------------------------------------------------------------------
// Render the model
//-----------------------------------------------------------------------------------------
void GPUModel::Render(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, int base, int num) 
{	
	UINT stride[1] = {0};
	UINT offset[1] = {0};
	ID3D10Buffer* Buffers[1] = {NULL};

	pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	pdev->IASetInputLayout(NULL);
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);
	// Bind m_boneMatrixBuffer as input buffer.
	m_boneMatrixBufferVar->SetResource( m_boneMatrixBufferRV );

	for (unsigned int i=0; i<m_numMeshs; i++)
	{
		m_ppMeshEffect[i]->GetVariableByName("mvp")->AsMatrix()->SetMatrix((float *)&mvp);
		m_ppMeshEffect[i]->GetVariableByName("cameraPos")->AsVector()->SetFloatVector((float *)&cmaerapos);											
		if (m_pMesh[i].GetMaterialIndex() >= 0)
			m_ppMeshEffect[i]->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource(m_pD3D10Material[m_pMesh[i].GetMaterialIndex()].diffuseMapView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexPos")->AsShaderResource()->SetResource(m_SkinVertexPosTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexNormal")->AsShaderResource()->SetResource(m_SkinVertexNormalTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexTexCoord")->AsShaderResource()->SetResource(m_SkinVertexTexCoordTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexBoneIndex")->AsShaderResource()->SetResource(m_SkinVertexBoneIndexTexView);						
		m_ppMeshEffect[i]->GetTechniqueByName("techGPUDrawMultiModels")->GetPassByName("P0")->Apply(0);	

		pdev->IASetPrimitiveTopology(m_pMesh[i].GetPrimitive(i).type);
		pdev->DrawIndexedInstanced( (UINT)m_pMesh[i].GetPrimitive(i).numIndices, num, m_pMesh[i].GetPrimitive(i).baseIndex, (UINT)m_pMesh[i].GetBaseVertex(), 0 );						
	}	
	m_boneMatrixBufferVar->SetResource( NULL );
	m_ppMeshEffect[0]->GetTechniqueByName("techGPUStreamoutMultiModels")->GetPassByName("P0")->Apply(0);
}
//-----------------------------------------------------------------------------------------
// Shadow rendering of this model
//-----------------------------------------------------------------------------------------
void GPUModel::RenderShadow(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos, int base, int num) 
{
	UINT stride[1] = {0};
	UINT offset[1] = {0};
	ID3D10Buffer* Buffers[1] = {NULL};

	pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);	
	pdev->IASetInputLayout(NULL);
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);
	m_boneMatrixBufferVar->SetResource( m_boneMatrixBufferRV );

	for (unsigned int i=0; i<m_numMeshs; i++)
	{		
		m_ppMeshEffect[i]->GetVariableByName("mvp")->AsMatrix()->SetMatrix((float *)&mvp);
		m_ppMeshEffect[i]->GetVariableByName("cameraPos")->AsVector()->SetFloatVector((float *)&cmaerapos);											
		m_ppMeshEffect[i]->GetVariableByName("skinVertexPos")->AsShaderResource()->SetResource(m_SkinVertexPosTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexNormal")->AsShaderResource()->SetResource(m_SkinVertexNormalTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexTexCoord")->AsShaderResource()->SetResource(m_SkinVertexTexCoordTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexBoneIndex")->AsShaderResource()->SetResource(m_SkinVertexBoneIndexTexView);						
		m_ppMeshEffect[i]->GetTechniqueByName("techGPUDrawMultiModelShadows")->GetPassByName("P0")->Apply(0);	

		pdev->IASetPrimitiveTopology(m_pMesh[i].GetPrimitive(i).type);
		pdev->DrawIndexedInstanced( (UINT)m_pMesh[i].GetPrimitive(i).numIndices, num, m_pMesh[i].GetPrimitive(i).baseIndex, (UINT)m_pMesh[i].GetBaseVertex(), 0 );						
	}	
	m_boneMatrixBufferVar->SetResource( NULL );
	m_ppMeshEffect[0]->GetTechniqueByName("techGPUStreamoutMultiModels")->GetPassByName("P0")->Apply(0);
}
//-----------------------------------------------------------------------------------------
// Stream out skinned vertex data.
//-----------------------------------------------------------------------------------------
void GPUModel::StreamOutPass(ID3D10Device* pdev, int base, int num) 
{
	UINT stride[1] = {0};
	UINT offset[1] = {0};
	ID3D10Buffer* Buffers[1] = {NULL};

	// Bind m_streamOutVB as output buffer.
	pdev->SOSetTargets(1, &m_streamOutVB, offset);	
	pdev->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	pdev->IASetInputLayout(NULL);
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);
	m_boneMatrixBufferVar->SetResource( m_boneMatrixBufferRV );
	
	for (unsigned int i=0; i<m_numMeshs; i++)
	{
		m_ppMeshEffect[i]->GetVariableByName("skinVertexPos")->AsShaderResource()->SetResource(m_SkinVertexPosTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexNormal")->AsShaderResource()->SetResource(m_SkinVertexNormalTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexTexCoord")->AsShaderResource()->SetResource(m_SkinVertexTexCoordTexView);
		m_ppMeshEffect[i]->GetVariableByName("skinVertexBoneIndex")->AsShaderResource()->SetResource(m_SkinVertexBoneIndexTexView);	
		m_ppMeshEffect[i]->GetTechniqueByName("techGPUStreamoutMultiModels")->GetPassByName("P0")->Apply(0);

		pdev->IASetPrimitiveTopology(m_pMesh[i].GetPrimitive(0).type);				
		pdev->DrawIndexedInstanced( (UINT)m_pMesh[i].GetPrimitive(0).numIndices, num, m_pMesh[i].GetPrimitive(0).baseIndex, (UINT)m_pMesh[i].GetBaseVertex(), 0 );						
	}	
	Buffers[0] = NULL;
    pdev->SOSetTargets( 1, Buffers, offset );

	m_boneMatrixBufferVar->SetResource( NULL );
	m_ppMeshEffect[0]->GetTechniqueByName("techGPUStreamoutMultiModels")->GetPassByName("P0")->Apply(0);
}
//-----------------------------------------------------------------------------------------
// Set specified shadow map as render target.
//-----------------------------------------------------------------------------------------
void GPUModel::SetShadowmapAsRenderTarget(ID3D10Device* pdev, bool bclear) 
{
	const float ClearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};	

	pdev->OMSetRenderTargets(1, &m_ShadowMapView, NULL);
	D3D10_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width  = SHADOWMAP_SIZE;
	viewport.Height = SHADOWMAP_SIZE;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	pdev->RSSetViewports(1, &viewport);
	if (bclear)
		pdev->ClearRenderTargetView(m_ShadowMapView, ClearColor);		
}
//-----------------------------------------------------------------------------------------
// Shadow rendering of stream out skinned vertex data.
//-----------------------------------------------------------------------------------------
void GPUModel::RenderStreamOutShadowPass(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos) 
{
	UINT stride[2];
	UINT offset[2] = {0, 0};
	ID3D10Buffer* Buffers[2] = {NULL, NULL};
		
	pdev->IASetInputLayout(m_stramOutIL);
	stride[0] = sizeof(GPUSTREAMOUTVERTEX);
	// Bind m_streamOutVB as vertex buffer.
	pdev->IASetVertexBuffers(0, 1, &m_streamOutVB, stride, offset);		
	m_ppMeshEffect[0]->GetVariableByName("mvp")->AsMatrix()->SetMatrix((float *)&mvp);
	m_ppMeshEffect[0]->GetVariableByName("cameraPos")->AsVector()->SetFloatVector((float *)&cmaerapos);						
	m_ppMeshEffect[0]->GetTechniqueByName("techGPUDrawStreamoutShadows")->GetPassByName("P0")->Apply(0);

	pdev->DrawAuto();

	Buffers[0] = NULL;
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);		
}
//-----------------------------------------------------------------------------------------
// Render stream out skinned vertex data.
//-----------------------------------------------------------------------------------------
void GPUModel::RenderStreamOutPass(ID3D10Device* pdev, const float4x4& mvp, const float3& cmaerapos) 
{
	UINT stride[2];
	UINT offset[2] = {0, 0};
	ID3D10Buffer* Buffers[2] = {NULL, NULL};
		
	pdev->IASetInputLayout(m_stramOutIL);			
	stride[0] = sizeof(GPUSTREAMOUTVERTEX);
	// Bind m_streamOutVB as vertex buffer.
	pdev->IASetVertexBuffers(0, 1, &m_streamOutVB, stride, offset);		
	m_ppMeshEffect[0]->GetVariableByName("mvp")->AsMatrix()->SetMatrix((float *)&mvp);
	m_ppMeshEffect[0]->GetVariableByName("cameraPos")->AsVector()->SetFloatVector((float *)&cmaerapos);						
	if (m_pMesh[0].GetMaterialIndex() >= 0)
		m_ppMeshEffect[0]->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource(m_pD3D10Material[m_pMesh[0].GetMaterialIndex()].diffuseMapView);
	m_ppMeshEffect[0]->GetTechniqueByName("techGPUDrawStreamoutModels")->GetPassByName("P0")->Apply(0);	

	pdev->DrawAuto();

	Buffers[0] = NULL;
	pdev->IASetVertexBuffers(0, 1, Buffers, stride, offset);		
}