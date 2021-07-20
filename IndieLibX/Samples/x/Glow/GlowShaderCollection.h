/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowShaderCollection.h

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

#ifndef H_GLOWSHADERCOLLECTION_H
#define H_GLOWSHADERCOLLECTION_H

#include "NV_D3DCommon\NV_D3DCommonTypes.h"
#include <vector>
using namespace std;

class GlowShader
{
public:
	SM_SHADER_INDEX		m_VSHI;		// vertex shader
	SM_SHADER_INDEX		m_PSHI;		// pixel shader
	int		m_nNumSamples;
};

class GlowShaderCollection
{
public:
	IDirect3DDevice9 *		m_pD3DDev;
	ShaderManager **		m_ppShaderManager;
	vector< GlowShader >	m_vShaders;

	int Free();
	int Initialize( IDirect3DDevice9 * pDev, ShaderManager ** ppShaderManager );

	GlowShaderCollection();
	~GlowShaderCollection();
	void SetAllNull()
	{
		m_pD3DDev				= 0;
		m_ppShaderManager		= 0;
	}
};

#endif
