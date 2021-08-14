/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#ifndef __GameEngine_h__
#define __GameEngine_h__

#include <Framework3/IRenderer.h>
#include <FW3.h>

#include <GUI/GUIUtils.h>
#include <Framework3/camera3.h>

#define MIN_DISTANCE	0.01
#define MAX_DISTANCE	1000.0				// Distance to desired far clipping plane
#define MAX_DISCERNABLE	1000000.0			// Beyond this distance, everything is rendered at MAX_DISTANCE
#define HALF_MAX		(MAX_DISTANCE*0.5)	// Everything between HALF_MAX and MAX_DISCERNABLE is scaled exponentially between HALF_MAX and MAX_DISTANCE

class CGameEngine : public STXGUI
{
public:
	bool bGround, bSky, bSpace, bMoonGlow, bBufferRenderTarget;
	float m_fFPS;
	int m_nTime;

	//C3DObject m_3DCamera;
	Camera3  m_3DCamera;
	D3DXFROMWINEVECTOR3 m_vLight;
	D3DXFROMWINEVECTOR3 m_vLightDirection;
	
	// Variables that can be tweaked with keypresses
	bool m_bUseHDR;
	int m_nSamples;
	int m_nPolygonMode;
	float m_Kr, m_Kr4PI;
	float m_Km, m_Km4PI;
	float m_ESun;
	float m_g;
	float m_fExposure;

	float m_fAltitude;

	float m_fInnerRadius;
	float m_fOuterRadius;

	D3DXFROMWINEMATRIX m_fInnerMatrix, m_fOuterMatrix;

	float m_fScale;
	float m_fWavelength[3];
	float m_fWavelength4[3];
	float m_fRayleighScaleDepth;
	float m_fMieScaleDepth;
	TextureID m_pbOpticalDepth;

	TextureID m_tMoonGlow;
	TextureID m_tEarth;

	ShaderID m_shSkyFromSpace;
	ShaderID m_shSkyFromAtmosphere;
	ShaderID m_shGroundFromSpace;
	ShaderID m_shGroundFromAtmosphere;
	ShaderID m_shSpaceFromSpace;
	ShaderID m_shSpaceFromAtmosphere;

	VertexFormatID m_shSkyFromSpaceVF;
	VertexFormatID m_shSkyFromAtmosphereVF;
	VertexFormatID m_shGroundFromSpaceVF;
	VertexFormatID m_shGroundFromAtmosphereVF;
	VertexFormatID m_shSpaceFromSpaceVF;
	VertexFormatID m_shSpaceFromAtmosphereVF;

	ShaderID shd;
	VertexFormatID vf;

	ShaderID shd3;
	VertexFormatID vf3;

	TextureID m_pBufferRenderTarget;

	MeshRenderer2 innersphere;
	MeshRenderer2 outersphere;
	D3DXFROMWINEMATRIX mvp_;

public:
	virtual void actionPerformed(GUIEvent &evt);
	CGameEngine();
	~CGameEngine();
	void renderMoon();
	void RenderFrame(int nMilliseconds);
	void Pause()	{}
	void Restore()	{}
	void HandleInput(float fSeconds);
	void OnChar(int c);
};

#endif // __GameEngine_h__

