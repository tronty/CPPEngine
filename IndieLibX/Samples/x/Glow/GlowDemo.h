/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowDemo.h

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

#ifndef H_GlowDemo_H
#define H_GlowDemo_H

#include "nvafx.h"
struct IDirect3DDevice9;
class GlowScene;
class GlowShaderCollection;

class GlowDemo
{
public:
	IDirect3DDevice9 *		m_pD3DDev;
	TextureFactory *		m_pTextureFactory;
	RenderTargetFactory *	m_pRenderTargetFactory;
	ShaderManager *			m_pShaderManager;
	ITextureDisplay *		m_pTextureDisplay;

	GlowScene *				m_pScene;
	RenderTargetSet			m_FlipChainBuffers;
	RenderTargetDesc **		m_ppRTTHalfSize;
	RenderTargetDesc **		m_ppRTTGlowSource;
	RenderTargetDesc **		m_ppRTTBlurHoriz;
	RenderTargetDesc **		m_ppRTTBlurVert;			// usually same RTT as m_ppRTTHalfSize

	TD_TEXID				m_TID_HalfSizeToBlurHoriz;	// texture display entry IDs
	TD_TEXID				m_TID_BlurHorizToBlurVert;
	TD_TEXID				m_TID_BlurToFlipChain;
	TD_TEXID				m_TID_FullscreenQuadNoOffsets;

	IPlot *					m_pPlot;
	PLOT_ID					m_PID_PlotHBlur;
	bool					m_bPlotProfiles;
	bool					m_bDisplayFinalBlurTexture;

	GlowShaderCollection *	m_pGlowShaderCollection;
	vector< D3DXFROMWINEVECTOR4 >	m_vBlurHCoefficients;
	vector< D3DXFROMWINEVECTOR4 >	m_vBlurHCoords;
	vector< D3DXFROMWINEVECTOR4 >	m_vBlurVCoefficients;
	vector< D3DXFROMWINEVECTOR4 >	m_vBlurVCoords;

	__D3DCOLOR__				m_dwGlowTextureAttenuation;		// multiplied by glow texture when adding to backbuffer

	int Free();
	int Initialize( IDirect3DDevice9 * pDev );
	int Render();
	GlowDemo();
	~GlowDemo();
	void SetAllNull()
	{
		m_pD3DDev				= 0;
		m_pTextureFactory		= 0;
		m_pRenderTargetFactory	= 0;
		m_pShaderManager		= 0;
		m_pTextureDisplay		= 0;

		m_pScene				= 0;
		m_ppRTTHalfSize			= 0;
		m_ppRTTGlowSource		= 0;
		m_ppRTTBlurHoriz		= 0;
		m_ppRTTBlurVert			= 0;
		m_pPlot					= 0;
		m_pGlowShaderCollection	= 0;
		m_bPlotProfiles				= false;
		m_bDisplayFinalBlurTexture	= false;
		m_dwGlowTextureAttenuation = 0x00808080;	// ARGB
	}
};

#endif
