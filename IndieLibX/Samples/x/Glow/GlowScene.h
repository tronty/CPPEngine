/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowScene.h

Copyright NVIDIA_Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:
A complex scene with about 800 DrawPrimtiive() calls, meant to be representative of a typical game scene

-------------------------------------------------------------------------------|--------------------*/

#ifndef H_GLOWSCENE_H
#define H_GLOWSCENE_H

#include <vector>
using namespace std;

class MeshVB;
class TextureFactory;

class GlowScene
{
public:
	IDirect3DDevice9 *		m_pD3DDev;
	
	vector< MeshVB ** >		m_vTerrain;
	vector< MeshVB ** >		m_vBuildings;
	vector< MeshVB ** >		m_vDetailObjects;
	vector< MeshVB ** >		m_vParticles;
	vector< MeshVB ** >		m_vCylArray;

	IDirect3DTexture9 **	m_ppTex01;

	int Free();
	int Initialize( IDirect3DDevice9 * pDev );
	int RenderObjects();
	int RenderObjectsWithGlowStrengthInAlpha();

	GlowScene();
	~GlowScene();
	void SetAllNull()
	{
		m_pTextureFactory	= 0;
		m_pD3DDev			= 0;

		m_ppTex01 = 0;
	};
protected:
	TextureFactory *	m_pTextureFactory;
};


#endif
