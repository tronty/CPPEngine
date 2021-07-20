/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowShaderCollection.cpp

Copyright NVIDIA_Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:


-------------------------------------------------------------------------------|--------------------*/

#include "NV_D3DCommon\NV_D3DCommonDX9PCH.h"
#include "GlowShaderCollection.h"

#define VSHN_8_SAMPLES	("D3D9_Glow\\TexCoordOffset_8_vs20.vsh")
#define PSHN_8_SAMPLES	("D3D9_Glow\\Add_8Samples_ps20.psh")

#define VSHN_6_SAMPLES	("D3D9_Glow\\TexCoordOffset_6_vs20.vsh")
#define PSHN_6_SAMPLES	("D3D9_Glow\\Add_6Samples_ps20.psh")

#define VSHN_4_SAMPLES	("D3D9_Glow\\TexCoordOffset_4_vs11.vsh")
#define PSHN_4_SAMPLES	("D3D9_Glow\\Add_4Samples_ps11.psh")

#define VSHN_2_SAMPLES	("D3D9_Glow\\TexCoordOffset_2_vs11.vsh")
#define PSHN_2_SAMPLES	("D3D9_Glow\\Add_2Samples_ps11.psh")


GlowShaderCollection::GlowShaderCollection()
{
	SetAllNull();
}

GlowShaderCollection::~GlowShaderCollection()
{
	Free();
	SetAllNull();
}

int GlowShaderCollection::Free()
{
	//SAFE_RELEASE( m_pD3DDev );
	m_ppShaderManager = 0;
	return( 0 );
}

int GlowShaderCollection::Initialize( IDirect3DDevice9 * pDev, ShaderManager ** ppShaderManager )
{
	int hr, hr1, hr2;
	hr = 0;
	Free();
	FAIL_IF_NULL( pDev );
	FAIL_IF_NULL( ppShaderManager );
	FAIL_IF_NULL( *ppShaderManager );
	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	ShaderManager * pSM = *ppShaderManager;
	GlowShader shader;

	hr1 = pSM->LoadAndAssembleShader( VSHN_8_SAMPLES, SM_SHADERTYPE_VERTEX,	&(shader.m_VSHI) );
	hr2 = pSM->LoadAndAssembleShader( PSHN_8_SAMPLES, SM_SHADERTYPE_PIXEL,	&(shader.m_PSHI) );
	if( SUCCEEDED(hr1) && SUCCEEDED(hr2) )
	{
		shader.m_nNumSamples = 8;
		m_vShaders.push_back( shader );
		FMsg("Loaded %d sample shaders\n", shader.m_nNumSamples );
	}

	hr1 = pSM->LoadAndAssembleShader( VSHN_6_SAMPLES, SM_SHADERTYPE_VERTEX,	&(shader.m_VSHI) );
	hr2 = pSM->LoadAndAssembleShader( PSHN_6_SAMPLES, SM_SHADERTYPE_PIXEL,	&(shader.m_PSHI) );
	if( SUCCEEDED(hr1) && SUCCEEDED(hr2) )
	{
		shader.m_nNumSamples = 6;
		m_vShaders.push_back( shader );
		FMsg("Loaded %d sample shaders\n", shader.m_nNumSamples );
	}

	hr1 = pSM->LoadAndAssembleShader( VSHN_4_SAMPLES, SM_SHADERTYPE_VERTEX,	&(shader.m_VSHI) );
	hr2 = pSM->LoadAndAssembleShader( PSHN_4_SAMPLES, SM_SHADERTYPE_PIXEL,	&(shader.m_PSHI) );
	if( SUCCEEDED(hr1) && SUCCEEDED(hr2) )
	{
		shader.m_nNumSamples = 4;
		m_vShaders.push_back( shader );
		FMsg("Loaded %d sample shaders\n", shader.m_nNumSamples );
	}

	hr1 = pSM->LoadAndAssembleShader( VSHN_2_SAMPLES, SM_SHADERTYPE_VERTEX,	&(shader.m_VSHI) );
	hr2 = pSM->LoadAndAssembleShader( PSHN_2_SAMPLES, SM_SHADERTYPE_PIXEL,	&(shader.m_PSHI) );
	if( SUCCEEDED(hr1) && SUCCEEDED(hr2) )
	{
		shader.m_nNumSamples = 2;
		m_vShaders.push_back( shader );
		FMsg("Loaded %d sample shaders\n", shader.m_nNumSamples );
	}

	return( hr );
}

