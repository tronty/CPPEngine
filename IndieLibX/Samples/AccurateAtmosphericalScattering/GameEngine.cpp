/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, wyaith or without
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

#include "Master.h"
#include "GameEngine.h"

const unsigned int flags=RESET_ALL;

#define DELTA				1e-6f

	float mAngleX=0.0f;
	float mAngleY=0.0f;
D3DXFROMWINEMATRIX GetMVP()
{
	D3DXFROMWINEMATRIX matRot;
	stx_GetAngles(mAngleX, mAngleY);


		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
#if 1
	D3DXFROMWINEMATRIX matView, matProj;
	static D3DXFROMWINEVECTOR3 _look(0.0f, 0.0f, -1.0f);
	static D3DXFROMWINEVECTOR3 _right(1.0f, 0.0f, 0.0f);
	static D3DXFROMWINEVECTOR3 _up(0.0f, 1.0f, 0.0f);
	static D3DXFROMWINEVECTOR3 _at(0.0f, 0.0f, 25.0f);
	D3DXFROMWINEVec3TransformCoord(&_look, &_look, &matRot);
	D3DXFROMWINEVec3TransformCoord(&_right, &_right, &matRot);
	D3DXFROMWINEVec3TransformCoord(&_up, &_up, &matRot);
/* ???
		if (STX_Service::GetInputInstance()->OnMouseButtonPress	(STX_MBUTTON_WHEELUP)) _at += _look*mSpeedRotation * mDelta;
		if (STX_Service::GetInputInstance()->OnMouseButtonPress	(STX_MBUTTON_WHEELDOWN)) _at -= _look*mSpeedRotation * mDelta;
*/
	D3DXFROMWINEMatrixLookAtLH(&matView, &_look, &_at, &_up);
	D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.001f, 1000000.0f);
	D3DXFROMWINEMatrixIdentity(&matProj);

	D3DXFROMWINEMATRIX mvp = matRot*matView*matProj;
	D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
#else
	D3DXFROMWINEMATRIX mvp = matRot;
#endif
	return mvp;
}

void MakeOpticalDepthBuffer(float* m_pBuffer, float fInnerRadius, float fOuterRadius, float fRayleighScaleHeight, float fMieScaleHeight)
{
	const int m_nChannels = 4;
	const int nSize = 64;
	const int nSamples = 50;
	const float fScale = 1.0f / (fOuterRadius - fInnerRadius);
	//std::ofstream ofScale1("scale1.txt");
	//std::ofstream ofScale2("scale2.txt");

	//Init(nSize, nSize, 1, 4, GL_RGBA, GL_FLOAT);
	int nIndex = 0;
	float fPrev = 0;
	for(int nAngle=0; nAngle<nSize; nAngle++)
	{
		// As the y tex coord goes from 0 to 1, the angle goes from 0 to 180 degrees
		float fCos = 1.0f - (nAngle+nAngle) / (float)nSize;
		float fAngle = acosf(fCos);
		D3DXFROMWINEVECTOR3 vRay(sinf(fAngle), cosf(fAngle), 0);	// Ray pointing to the viewpoint

		/*char szName[256];
		sprintf(szName, "graph%-2.2d.txt", nAngle);
		std::ofstream ofGraph;
		if(fCos >= 0.0f)
			ofGraph.open(szName);
		ofGraph << "# fCos = " << fCos << std::endl;*/

		float fFirst = 0;
		for(int nHeight=0; nHeight<nSize; nHeight++)
		{
			// As the x tex coord goes from 0 to 1, the height goes from the bottom of the atmosphere to the top
			float fHeight = DELTA + fInnerRadius + ((fOuterRadius - fInnerRadius) * nHeight) / nSize;
			D3DXFROMWINEVECTOR3 vPos(0, fHeight, 0);				// The position of the camera

			// If the ray from vPos heading in the vRay direction intersects the inner radius (i.e. the planet), then this spot is not visible from the viewpoint
			//float B = 2.0f * (vPos | vRay);
			float B = 2.0f * (!(vPos == vRay));//???

			float Bsq = B * B;
			//float Cpart = (vPos | vPos);
			float Cpart = (!(vPos == vPos));//???
			float C = Cpart - fInnerRadius*fInnerRadius;
			float fDet = Bsq - 4.0f * C;
			bool bVisible = (fDet < 0 || (0.5f * (-B - sqrtf(fDet)) <= 0) && (0.5f * (-B + sqrtf(fDet)) <= 0));
			float fRayleighDensityRatio;
			float fMieDensityRatio;
			if(bVisible)
			{
				fRayleighDensityRatio = expf(-(fHeight - fInnerRadius) * fScale / fRayleighScaleHeight);
				fMieDensityRatio = expf(-(fHeight - fInnerRadius) * fScale / fMieScaleHeight);
			}
			else
			{
				// Smooth the transition from light to shadow (it is a soft shadow after all)
				fRayleighDensityRatio = ((float *)m_pBuffer)[nIndex - nSize*m_nChannels] * 0.5f;
				fMieDensityRatio = ((float *)m_pBuffer)[nIndex+2 - nSize*m_nChannels] * 0.5f;
			}

			// Determine where the ray intersects the outer radius (the top of the atmosphere)
			// This is the end of our ray for determining the optical depth (vPos is the start)
			C = Cpart - fOuterRadius*fOuterRadius;
			fDet = Bsq - 4.0f * C;
			float fFar = 0.5f * (-B + sqrtf(fDet));

			// Next determine the length of each sample, scale the sample ray, and make sure position checks are at the center of a sample ray
			float fSampleLength = fFar / nSamples;
			float fScaledLength = fSampleLength * fScale;
			D3DXFROMWINEVECTOR3 vSampleRay = vRay * fSampleLength;
			vPos += vSampleRay * 0.5f;

			// Iterate through the samples to sum up the optical depth for the distance the ray travels through the atmosphere
			float fRayleighDepth = 0;
			float fMieDepth = 0;
			for(int i=0; i<nSamples; i++)
			{
				float fHeight = D3DXFROMWINEVec3Length(&vPos);
				float fAltitude = (fHeight - fInnerRadius) * fScale;
				//fAltitude = stx_max(fAltitude, 0.0f);
				fRayleighDepth += expf(-fAltitude / fRayleighScaleHeight);
				fMieDepth += expf(-fAltitude / fMieScaleHeight);
				vPos += vSampleRay;
			}

			// Multiply the sums by the length the ray traveled
			fRayleighDepth *= fScaledLength;
			fMieDepth *= fScaledLength;

			if(!isfinite(fRayleighDepth) || fRayleighDepth > 1.0e25f)
				fRayleighDepth = 0;
			if(!isfinite(fMieDepth) || fMieDepth > 1.0e25f)
				fMieDepth = 0;

			// Store the results for Rayleigh to the light source, Rayleigh to the camera, Mie to the light source, and Mie to the camera
			((float *)m_pBuffer)[nIndex++] = fRayleighDensityRatio;
			((float *)m_pBuffer)[nIndex++] = fRayleighDepth;
			((float *)m_pBuffer)[nIndex++] = fMieDensityRatio;
			((float *)m_pBuffer)[nIndex++] = fMieDepth;

			/*
			if(nHeight == 0)
			{
				fFirst = fRayleighDepth;
				if(fCos >= 0.0f)
				{
					ofScale1 << 1-fCos << "\t" << logf(fRayleighDepth / fRayleighScaleHeight) << std::endl;
					ofScale2 << 1-fCos << "\t" << logf(fMieDepth) << std::endl;
					fPrev = fRayleighDepth;
				}
			}
			float x = (fHeight-fInnerRadius) / (fOuterRadius-fInnerRadius);
			float y = fRayleighDepth / fFirst;
			ofGraph << x << "\t" << y << std::endl;
			*/
		}
		//ofGraph << std::endl;
	}
}

void CGameEngine::renderMoon()
{

	IRenderer::GetRendererInstance()->setShader(shd3);
	IRenderer::GetRendererInstance()->setVertexFormat(vf3);
	D3DXFROMWINEMATRIX W,S,T;
	float s=0.05f;
	float t=-50.0f;
	D3DXFROMWINEMatrixScaling(&S, s, s, s);
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, t);
	W=S*T;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", m_tMoonGlow);
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);
	#if 1
	float d1=1.0f;
	float d2=0.0f;
	#else
	float d1=4.0f;
	float d2=50.0f;
	#endif
	//	   	   x	 y     tx    ty
	float v[20] =  {  d1,-d1, d2, 1.0f, 1.0f,
			  d1, d1, d2, 1.0f, 0.0f,
			 -d1,-d1, d2, 0.0f, 1.0f,
			 -d1, d1, d2, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 5*sizeof(float), 0);
}

CGameEngine::CGameEngine()
{
	m_pbOpticalDepth=-1;

	m_tMoonGlow=-1;
	m_tEarth=-1;

	m_shSkyFromSpace=-1;
	m_shSkyFromAtmosphere=-1;
	m_shGroundFromSpace=-1;
	m_shGroundFromAtmosphere=-1;
	m_shSpaceFromSpace=-1;
	m_shSpaceFromAtmosphere=-1;

	m_shSkyFromSpaceVF=-1;
	m_shSkyFromAtmosphereVF=-1;
	m_shGroundFromSpaceVF=-1;
	m_shGroundFromAtmosphereVF=-1;
	m_shSpaceFromSpaceVF=-1;
	m_shSpaceFromAtmosphereVF=-1;

	shd=-1;
	vf=-1;

	shd3=-1;
	vf3=-1;

	m_pBufferRenderTarget=-1;

	m_3DCamera=Camera3(Camera3::AIRCRAFT);
	STXGUI::init("/AccurateAtmosphericalScattering/GUILayout.xml");

	bGround=true;
	bSky=true;
	bSpace=true;
	bMoonGlow=true;
	bBufferRenderTarget=false;

	IRenderer::GetRendererInstance()->InitTex(shd, vf);
	IRenderer::GetRendererInstance()->InitTex3(shd3, vf3);

	m_bUseHDR = true;

	m_nPolygonMode = true;

	m_pBufferRenderTarget = IRenderer::GetRendererInstance()->addRenderTarget(1024, 1024, FORMAT_RGBA8, IRenderer::GetRendererInstance()->Getlinear());

	#if 0
	// Read last camera position and orientation from registry
	D3DXFROMWINEVECTOR3 vPos(0, 0, 25);
	const char *psz = GetApp()->GetProfileString("Camera", "Position", NULL);
	if(psz)
		sscanf(psz, "%f, %f, %f", &vPos.x, &vPos.y, &vPos.z);
	m_3DCamera.SetPosition(D3DXFROMWINEVECTOR3(vPos));
	D3DXFROMWINEQUATERNION qOrientation(0.0f, 0.0f, 0.0f, 1.0f);
	psz = GetApp()->GetProfileString("Camera", "Orientation", NULL);
	if(psz)
		sscanf(psz, "%f, %f, %f, %f", &qOrientation.x, &qOrientation.y, &qOrientation.z, &qOrientation.w);
	qOrientation.normalize();
	m_3DCamera = qOrientation;
	#endif

	//???CTexture::InitStaticMembers(238653, 256);
#if 0
	m_fInnerRadius = 10.0f;
	m_fOuterRadius = 10.25f;
#else
	m_fInnerRadius = 1.0f;
	m_fOuterRadius = 1.025f;
#endif
	#if 0
	m_fAltitude = 10.15f;
	#elif 1
	m_fAltitude = 10.5f;
	m_fAltitude = 1.05f;
#if 0
	m_fAltitude = 1.0125f;
	LuaScript script;
	script.Load("/AccurateAtmosphericalScattering/config.lua");
    	m_fAltitude = script.get<float>("parameters.Altitude");
#endif
	#elif 0
	m_fAltitude = 0.5f*(m_fOuterRadius-m_fInnerRadius)+m_fOuterRadius;
	#elif 0
	m_fAltitude = 0.5f*(m_fOuterRadius-m_fInnerRadius)+m_fInnerRadius;
	#endif

	LOG_PRINT("m_fAltitude=%f\n", m_fAltitude);

	m_vLight = D3DXFROMWINEVECTOR3(0, 0, 1000);
	m_vLightDirection = m_vLight / D3DXFROMWINEVec3Length(&m_vLight);
	D3DXFROMWINEVECTOR3 pos(m_fAltitude, 0.0f, 0.0f);
	m_3DCamera.setPosition(&pos);

	m_nSamples = 3;		// Number of sample rays to use in integral equation
	m_Kr = 0.0025f;		// Rayleigh scattering constant
	m_Kr4PI = m_Kr*4.0f*D3DXFROMWINE_PI;
	m_Km = 0.0010f;		// Mie scattering constant
	m_Km4PI = m_Km*4.0f*D3DXFROMWINE_PI;
	m_ESun = 20.0f;		// Sun brightness constant
	m_g = -0.990f;		// The Mie phase asymmetry factor
	m_fExposure = 2.0f;

	m_fScale = 1 / (m_fOuterRadius - m_fInnerRadius);

	m_fWavelength[0] = 0.650f;		// 650 nm for red
	m_fWavelength[1] = 0.570f;		// 570 nm for green
	m_fWavelength[2] = 0.475f;		// 475 nm for blue
	m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

	m_fRayleighScaleDepth = 0.25f;
	m_fMieScaleDepth = 0.1f;

	m_shSkyFromSpace = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SkyFromSpace.shd", "main", "main");
	//
	LOG_PRINT("m_shSkyFromSpace:%x\n", m_shSkyFromSpace);

	m_shSkyFromAtmosphere = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SkyFromAtmosphere.shd", "main", "main");
	//
	LOG_PRINT("m_shSkyFromAtmosphere:%x\n", m_shSkyFromAtmosphere);

	m_shGroundFromSpace = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/GroundFromSpace.shd", "main", "main");
	//
	LOG_PRINT("m_shGroundFromSpace:%x\n", m_shGroundFromSpace);

	m_shGroundFromAtmosphere = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/GroundFromAtmosphere.shd", "main", "main");
	//
	LOG_PRINT("m_shGroundFromAtmosphere:%x\n", m_shGroundFromAtmosphere);

	m_shSpaceFromSpace = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SpaceFromSpace.shd", "main", "main");
	//
	LOG_PRINT("m_shSpaceFromSpace:%x\n", m_shSpaceFromSpace);

	m_shSpaceFromAtmosphere = IRenderer::GetRendererInstance()->addShaderFromFile("/AccurateAtmosphericalScattering/SpaceFromAtmosphere.shd", "main", "main");
	//
	LOG_PRINT("m_shSpaceFromAtmosphere:%x\n", m_shSpaceFromAtmosphere);
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	{
	m_shGroundFromSpaceVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shGroundFromSpace);
	//
	//LOG_PRINT("m_shGroundFromSpaceVF:%x\n", m_shGroundFromSpaceVF);
	}
	{
	m_shGroundFromAtmosphereVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shGroundFromAtmosphere);
	//
	//LOG_PRINT("m_shGroundFromAtmosphereVF:%x\n", m_shGroundFromAtmosphereVF);
	}
	{
	m_shSkyFromSpaceVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shSkyFromSpace);
	//
	//LOG_PRINT("m_shSkyFromSpaceVF:%x\n", m_shSkyFromSpaceVF);
	}
	{
	m_shSkyFromAtmosphereVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shSkyFromAtmosphere);
	//
	//LOG_PRINT("m_shSkyFromAtmosphereVF:%x\n", m_shSkyFromAtmosphereVF);
	}
	{
	m_shSpaceFromSpaceVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shSpaceFromSpace);
	//
	//LOG_PRINT("m_shSpaceFromSpaceVF:%x\n", m_shSpaceFromSpaceVF);
	}
	{
	m_shSpaceFromAtmosphereVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_shSpaceFromAtmosphere);
	//
	//LOG_PRINT("m_shSpaceFromAtmosphereVF:%x\n", m_shSpaceFromAtmosphereVF);
	}

#if 0
	m_tMoonGlow=-1;
	m_tEarth=-1;
#else

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
//
	m_tEarth=IRenderer::GetRendererInstance()->addImageLibTexture("/AccurateAtmosphericalScattering/earthmap1k.jpg", true, ss);
//
//LOG_PRINT("m_tEarth=%x\n", m_tEarth);

#endif

	Image3 img;

	float* m_pBuffer__=(float*)img.create(FORMAT_RGBA32f, 64, 64, 1, 1);

	MakeOpticalDepthBuffer(m_pBuffer__,m_fInnerRadius, m_fOuterRadius, m_fRayleighScaleDepth, m_fMieScaleDepth);

	m_pbOpticalDepth=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
	#if 0
	Image3 img_;
	int m_nWidth=256;
	int m_nHeight=256;
	int m_nChannels=4;
	ubyte* m_pBuffer_=img_.create(FORMAT_RGBA8, m_nWidth, m_nHeight, 1, 1);
	float fExposure=40.0f;
	float fRadius=0.1f;
	{
	int nIndex=0;

	for(int y=0; y<m_nHeight; y++)
	{
		for(int x=0; x<m_nWidth; x++)
		{
			float fX = ((m_nWidth-1)*0.5f - x) / (float)(m_nWidth-1);
			float fY = ((m_nHeight-1)*0.5f - y) / (float)(m_nHeight-1);
			float fDist = stx_max(0.0f, sqrtf(fX*fX + fY*fY) - fRadius);

			float fIntensity = exp(-fExposure * fDist);
			unsigned char c = (unsigned char)(fIntensity*192 + 0.5f);
			for(int i=0; i<m_nChannels; i++)
				((unsigned char *)m_pBuffer_)[nIndex++] = c;
		}
	}

	}

	m_tMoonGlow=IRenderer::GetRendererInstance()->addTexture(&img_, false, IRenderer::GetRendererInstance()->Getlinear());
#else
	m_tMoonGlow=IRenderer::GetRendererInstance()->addImageLibTexture("/Stars/SolarSystemDisks/Moon.png", true, ss);
#endif
//
#if 0
	innersphere.CreateSphere_(m_fInnerRadius, 100, 100, eShaderNone);
#elif 0
	innersphere.Load("/dx9sdk/sphere.x", eSimpleShader, true, true);
#else
	innersphere.CreateSphere(m_fInnerRadius, eShaderNone);
	//D3DXFROMWINEMatrixScaling(&m_fInnerMatrix, m_fInnerRadius, m_fInnerRadius, m_fInnerRadius);
	D3DXFROMWINEMatrixIdentity(&m_fInnerMatrix);
#endif
//
#if 0
	outersphere.CreateSphere_(m_fOuterRadius, 100, 100, eShaderNone);
#else
	outersphere.CreateSphere(m_fOuterRadius, eShaderNone);
	//D3DXFROMWINEMatrixScaling(&m_fOuterMatrix, m_fOuterRadius, m_fOuterRadius, m_fOuterRadius);
	D3DXFROMWINEMatrixIdentity(&m_fOuterMatrix);
#endif
//
}

CGameEngine::~CGameEngine()
{
	// Write the camera position and orientation to the registry
	char szBuffer[256];
	D3DXFROMWINEVECTOR3 pos;
	m_3DCamera.getPosition(&pos);
	stx_snprintf(szBuffer, 256, "Camera Position %f, %f, %f", pos.x, pos.y, pos.z);
	//LOG_PRINT("%s\n", szBuffer);
	//stx_snprintf(szBuffer, 256, "Camera Orientation %f, %f, %f, %f", m_3DCamera.x, m_3DCamera.y, m_3DCamera.z, m_3DCamera.w);
	//LOG_PRINT("%s\n", szBuffer);
}

void CGameEngine::actionPerformed(GUIEvent &evt)
{
  const std::string &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == CHECK_BOX)
  {
	GUICheckBox   *checkBox = (GUICheckBox*)sourceRectangle;
	if(checkBox->isPressed())
	{
		//LOG_PRINT("callbackString=%s\n", callbackString.data);
      		if(callbackString == "eGround")
		{
			bGround=checkBox->isChecked();
		}
		else if(callbackString == "eSky")
		{
			bSky=checkBox->isChecked();
		}
		else if(callbackString == "eSpace")
		{
			bSpace=checkBox->isChecked();
		}
		else if(callbackString == "eMoonGlow")
		{
			bMoonGlow=checkBox->isChecked();
		}
		else if(callbackString == "eBufferRenderTarget")
		{
			bBufferRenderTarget=checkBox->isChecked();
		}

	}
	}
}
void CGameEngine::RenderFrame(int nMilliseconds)
{
	/*
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1)) bGround^=bGround;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2)) bSky^=bSky;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3)) bSpace^=bSpace;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4)) bMoonGlow^=bMoonGlow;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5)) bBufferRenderTarget^=bBufferRenderTarget;
	*/

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		m_3DCamera.reset();
		D3DXFROMWINEVECTOR3 pos(m_fAltitude, 0.0f, 0.0f);
		m_3DCamera.setPosition(&pos);
	}
	// Determine the FPS
	static char szFrameCount[256] = {0};
	static int nTime = 0;
	static int nFrames = 0;
	nTime += nMilliseconds;

	if(nTime >= 1000)
	{

		m_fFPS = (float)(nFrames * 1000) / (float)nTime;

		stx_snprintf(szFrameCount, 256, "%2.2f FPS", m_fFPS);

#if 1
		IRenderer::GetRendererInstance()->drawText(szFrameCount, 10, 10,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
#elif 0
		//LOG_PRINT("%s\n", szFrameCount);
#endif

		nTime = nFrames = 0;

	}

	nFrames++;
#if 0
if(0)
{
	static unsigned int t0=timeGetTime();
	unsigned int t=timeGetTime();
	float dt=1.0f;
	if(t>t0)
		dt=t-t0;
	float r=m_fAltitude;
	D3DXFROMWINEVECTOR3 pos(r*cosf(dt), r*sinf(dt));
	D3DXFROMWINEVECTOR3 vel(-r*sinf(dt), r*cosf(dt));
	D3DXFROMWINEVECTOR3 acc(-r*cosf(dt), -r*sinf(dt));
	D3DXFROMWINEVECTOR3 look=vel;
	D3DXFROMWINEVec3Normalize(&look, &look);
	m_3DCamera.setPosition(&pos);
	m_3DCamera.setLook(&look);
	m_3DCamera.SetVelocity(vel);
	m_3DCamera.Accelerate(acc, );
}
#endif

	// Move the camera
	HandleInput(nMilliseconds * 0.001f);

	m_pBufferRenderTarget=-1;
	if(m_pBufferRenderTarget>-1)
	{
		IRenderer::GetRendererInstance()->changeRenderTarget(m_pBufferRenderTarget);
		IRenderer::GetRendererInstance()->viewport(0, 0, 1024, 1024);
	}
	IRenderer::GetRendererInstance()->Clear(true,true,
		//D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7f, 1.0f));
		D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
#if 0
	glPushMatrix();
	glLoadMatrixf(m_3DCamera.GetViewMatrix());

	C3DObject obj;
	glMultMatrixf(obj.GetModelMatrix(&m_3DCamera));
#endif
	D3DXFROMWINEMatrixIdentity(&mvp_);//=GetMVP();
	m_3DCamera.getViewMatrix(&mvp_);
	D3DXFROMWINEVECTOR3 vCamera;
	m_3DCamera.getPosition(&vCamera);
	D3DXFROMWINEVECTOR3 vUnitCamera = vCamera / D3DXFROMWINEVec3Length(&vCamera);

	if(0){D3DXFROMWINEMATRIX mLightModelMatrix;
    D3DXFROMWINEVECTOR3 mLightPosInWorldSpace;//, mLightPosInModelSpace;
	static float angleInRadians = 0.0f;
	angleInRadians += (float)D3DXFROMWINE_PI * 0.0005f;
    D3DXFROMWINEMatrixRotationY(&mLightModelMatrix, angleInRadians);
	static D3DXFROMWINEVECTOR3 mLightPosInModelSpace=m_vLightDirection;
    D3DXFROMWINEVec3TransformCoord(&mLightPosInWorldSpace, &mLightPosInModelSpace, &mLightModelMatrix);
	m_vLightDirection=mLightPosInWorldSpace;}

	//D3DXFROMWINEMatrixIdentity(&mvp_);

	ShaderID pSpaceShader = m_shSpaceFromSpace;
	VertexFormatID pSpaceVF = m_shSpaceFromSpaceVF;
	if(D3DXFROMWINEVec3Length(&vCamera) < m_fOuterRadius)
	{
		pSpaceShader = m_shSpaceFromAtmosphere;
		pSpaceVF = m_shSpaceFromSpaceVF;
	}

	pSpaceShader==-1;

	if((pSpaceShader>-1) && (bSpace))
	{
	//
	//LOG_PRINT("x:pSpaceShader=%x\n", pSpaceShader);
	//LOG_PRINT("x:pSpaceVF=%x\n", pSpaceVF);

	//
	//LOG_PRINT("outersphere.BeginDraw:Space\n");
		D3DXFROMWINEMATRIX m=mvp_*m_fOuterMatrix;
	//
	//LOG_PRINT("outersphere.BeginDraw:Space\n");
		outersphere.BeginDraw(&mvp_, -1, pSpaceShader, pSpaceVF);

		IRenderer::GetRendererInstance()->setShaderConstant4x4f("GL_ModelViewProjectionMatrix", mvp_);

		IRenderer::GetRendererInstance()->setShaderConstant3f("v3CameraPos", vCamera);
		IRenderer::GetRendererInstance()->setShaderConstant3f("v3LightPos", m_vLightDirection);

	D3DXFROMWINEVECTOR3 v3InvWavelength(1/m_fWavelength4[0], 1/m_fWavelength4[1], 1/m_fWavelength4[2]);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3InvWavelength", v3InvWavelength);

		IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight", D3DXFROMWINEVec3Length(&vCamera));
		IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight2", D3DXFROMWINEVec3Length(&vCamera)*D3DXFROMWINEVec3Length(&vCamera));
		IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius", m_fInnerRadius);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius2", m_fInnerRadius*m_fInnerRadius);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius", m_fOuterRadius);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius2", m_fOuterRadius*m_fOuterRadius);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKrESun", m_Kr*m_ESun);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKmESun", m_Km*m_ESun);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKr4PI", m_Kr4PI);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKm4PI", m_Km4PI);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fScale", 1.0f / (m_fOuterRadius - m_fInnerRadius));
		IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleDepth", m_fRayleighScaleDepth);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleOverScaleDepth", (1.0f / (m_fOuterRadius - m_fInnerRadius)) / m_fRayleighScaleDepth);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g", m_g);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g2", m_g*m_g);
		//IRenderer::GetRendererInstance()->setTexture("s2Test", m_tMoonGlow);

  //IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  //IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	//
	//LOG_PRINT("outersphere.EndDraw:Space\n");
		outersphere.EndDraw(flags);
	}

	if((m_tMoonGlow>-1) && (bMoonGlow))
		renderMoon();

	ShaderID pGroundShader = m_shGroundFromAtmosphere;
	VertexFormatID pGroundVF = m_shGroundFromAtmosphereVF;
	if(D3DXFROMWINEVec3Length(&vCamera) >= m_fOuterRadius)
	{
		pGroundShader = m_shGroundFromSpace;
		pGroundVF = m_shGroundFromSpaceVF;
	}


	if((pGroundShader>-1) && (bGround))

	{
	//
	//LOG_PRINT("x:pGroundShader=%x\n", pGroundShader);
	//LOG_PRINT("x:pGroundVF=%x\n", pGroundVF);

	//
	//LOG_PRINT("innersphere.BeginDraw:Ground\n");
	D3DXFROMWINEMATRIX m=mvp_*m_fInnerMatrix;
	//
	//LOG_PRINT("innersphere.BeginDraw:Ground\n");
	innersphere.BeginDraw(&mvp_, -1, pGroundShader, pGroundVF);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("GL_ModelViewProjectionMatrix", mvp_);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3CameraPos", vCamera);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3LightPos", m_vLightDirection);

	D3DXFROMWINEVECTOR3 v3InvWavelength(1/m_fWavelength4[0], 1/m_fWavelength4[1], 1/m_fWavelength4[2]);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3InvWavelength", v3InvWavelength);

	IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight", D3DXFROMWINEVec3Length(&vCamera));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight2", D3DXFROMWINEVec3Length(&vCamera)*D3DXFROMWINEVec3Length(&vCamera));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius", m_fInnerRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius2", m_fInnerRadius*m_fInnerRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius", m_fOuterRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius2", m_fOuterRadius*m_fOuterRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKrESun", m_Kr*m_ESun);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKmESun", m_Km*m_ESun);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKr4PI", m_Kr4PI);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKm4PI", m_Km4PI);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScale", 1.0f / (m_fOuterRadius - m_fInnerRadius));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleDepth", m_fRayleighScaleDepth);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleOverScaleDepth", (1.0f / (m_fOuterRadius - m_fInnerRadius)) / m_fRayleighScaleDepth);
	IRenderer::GetRendererInstance()->setShaderConstant1f("g", m_g);
	IRenderer::GetRendererInstance()->setShaderConstant1f("g2", m_g*m_g);
	IRenderer::GetRendererInstance()->setTexture("s2Test", m_tEarth);


	/*
	if(vCamera.z < 0 && pGroundShader == &m_shGroundFromAtmosphere)
	{
		// Try setting the moon as a light source
		D3DXFROMWINEVECTOR3 vLightDir = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -50.0f) - vCamera;
		vLightDir.normalize();
		IRenderer::GetRendererInstance()->setShaderConstant3f("v3LightPos", vLightDir.x, vLightDir.y, vLightDir.z);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKrESun", m_Kr*m_ESun*0.1f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKmESun", 10.0f*m_Km*m_ESun*0.1f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g", -0.75f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g2", -0.75f * -0.75f);
	}
	*/
	//
	//LOG_PRINT("innersphere.EndDraw:Ground\n");
	innersphere.EndDraw(flags);

	//
	//LOG_PRINT("innersphere.EndDraw:Ground\n");
}
	ShaderID pSkyShader = m_shSkyFromAtmosphere;
	VertexFormatID pSkyVF = m_shSkyFromAtmosphereVF;
	if(D3DXFROMWINEVec3Length(&vCamera) >= m_fOuterRadius)
	{
		pSkyShader = m_shSkyFromSpace;
		pSkyVF = m_shSkyFromSpaceVF;
	}
	pSkyShader = -1;
	if((pSkyShader>-1) && (bSky))

	{
	//
	//LOG_PRINT("x:pSkyShader=%x\n", pSkyShader);
	//LOG_PRINT("x:pSkyVF=%x\n", pSkyVF);

	//
	//LOG_PRINT("outersphere.BeginDraw:Sky\n");
	D3DXFROMWINEMATRIX m=mvp_*m_fOuterMatrix;
	//glFrontFace(GL_CW);
	//
	//LOG_PRINT("outersphere.BeginDraw:Sky\n");
	outersphere.BeginDraw(&mvp_, -1, pSkyShader, pSkyVF);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("GL_ModelViewProjectionMatrix", mvp_);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3CameraPos", vCamera);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3LightPos", m_vLightDirection);

	D3DXFROMWINEVECTOR3 v3InvWavelength2(1/m_fWavelength4[0], 1/m_fWavelength4[1], 1/m_fWavelength4[2]);
	IRenderer::GetRendererInstance()->setShaderConstant3f("v3InvWavelength", v3InvWavelength2);

	IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight", D3DXFROMWINEVec3Length(&vCamera));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fCameraHeight2", D3DXFROMWINEVec3Length(&vCamera)*D3DXFROMWINEVec3Length(&vCamera));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius", m_fInnerRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fInnerRadius2", m_fInnerRadius*m_fInnerRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius", m_fOuterRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fOuterRadius2", m_fOuterRadius*m_fOuterRadius);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKrESun", m_Kr*m_ESun);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKmESun", m_Km*m_ESun);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKr4PI", m_Kr4PI);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fKm4PI", m_Km4PI);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScale", 1.0f / (m_fOuterRadius - m_fInnerRadius));
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleDepth", m_fRayleighScaleDepth);
	IRenderer::GetRendererInstance()->setShaderConstant1f("fScaleOverScaleDepth", (1.0f / (m_fOuterRadius - m_fInnerRadius)) / m_fRayleighScaleDepth);
	IRenderer::GetRendererInstance()->setShaderConstant1f("g", m_g);
	IRenderer::GetRendererInstance()->setShaderConstant1f("g2", m_g*m_g);


	/*
	if(vCamera.z < 0 && pSkyShader == &m_shSkyFromAtmosphere)
	{
		// Try setting the moon as a light source
		D3DXFROMWINEVECTOR3 vLightDir = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -50.0f) - vCamera;
		vLightDir.normalize();
		IRenderer::GetRendererInstance()->setShaderConstant3f("v3LightPos", vLightDir.x, vLightDir.y, vLightDir.z);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKrESun", m_Kr*m_ESun*0.1f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("fKmESun", 10.0f*m_Km*m_ESun*0.1f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g", -0.75f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("g2", -0.75f * -0.75f);
	}
	*/

	//
	//LOG_PRINT("outersphere.EndDraw:Sky\n");
	outersphere.EndDraw(flags);
	//glFrontFace(GL_CCW);

	//
	//LOG_PRINT("outersphere.EndDraw:Sky\n");
	}

	if((m_pBufferRenderTarget>-1) && (bBufferRenderTarget))

	{
	IRenderer::GetRendererInstance()->EndScene();
  	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->BeginScene();
#if 0
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);

	m_pBufferRenderTarget.BindTexture(m_fExposure, m_bUseHDR);
#endif
#if 0
	IRenderer::GetRendererInstance()->Begin(PRIM_QUADS);
	IRenderer::GetRendererInstance()->TexCoord2f(0, 0); IRenderer::GetRendererInstance()->Vertex2f(0, 0);	// For rect texture, can't use 1 as the max texture coord
	IRenderer::GetRendererInstance()->TexCoord2f(1, 0); IRenderer::GetRendererInstance()->Vertex2f(1, 0);
	IRenderer::GetRendererInstance()->TexCoord2f(1, 1); IRenderer::GetRendererInstance()->Vertex2f(1, 1);
	IRenderer::GetRendererInstance()->TexCoord2f(0, 1); IRenderer::GetRendererInstance()->Vertex2f(0, 1);
	IRenderer::GetRendererInstance()->End(m_pBufferRenderTarget);
#else
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setTexture("Base", m_pBufferRenderTarget);
	//	   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float), 0);
#endif
	}
	m_3DCamera.update();
  	STXGUI::update();


#if 1
	// Draw info in the top-left corner
	char szBuffer[256]; /*
		const char* txt = "F1-F5 key";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest()); */
	stx_snprintf(szBuffer, 256, "Samples (9/Sh+9): %d", m_nSamples);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 30,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Kr (1/Sh+1): %-4.4f", m_Kr);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 50,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Km (2/Sh+2): %-4.4f", m_Km);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 70,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "g (3/Sh+3): %-3.3f", m_g);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 90,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "ESun (4/Sh+4): %-1.1f", m_ESun);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 110,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Red (5/Sh+5): %-3.3f", m_fWavelength[0]);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 130,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Green (6/Sh+6): %-3.3f", m_fWavelength[1]);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 150,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Blue (7/Sh+7): %-3.3f", m_fWavelength[2]);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 170,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	stx_snprintf(szBuffer, 256, "Exposure (8/Sh+8): %-2.2f", m_fExposure);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 190,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	D3DXFROMWINEVECTOR3 pos;
	m_3DCamera.getPosition(&pos);
	stx_snprintf(szBuffer, 256, "Camera Position %f, %f, %f", pos.x, pos.y, pos.z);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 210,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest()); /*
	stx_snprintf(szBuffer, 256, "Camera Orientation %f, %f, %f, %f", m_3DCamera.x, m_3DCamera.y, m_3DCamera.z, m_3DCamera.w);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 230,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest()); */
	D3DXFROMWINEVECTOR3 vel=m_3DCamera.GetVelocity();
	stx_snprintf(szBuffer, 256, "Camera Velocity %f, %f, %f", vel.x, vel.y, vel.z);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 230,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	D3DXFROMWINEVECTOR3 acc=m_3DCamera.GetAcceleration();
	stx_snprintf(szBuffer, 256, "Camera Acceleration %f, %f, %f", acc.x, acc.y, acc.z);
		IRenderer::GetRendererInstance()->drawText(szBuffer, 10, 250,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
#if 0
STX_KEY_F1	Camera Reset
STX_KEY_F2	Camera Orbit
STX_KEY_F3	Camera Yaw
STX_KEY_F4	Camera Pitch
STX_KEY_F5	Camera Roll
STX_KEY_p	!PolygonMode
STX_KEY_h	!HDR
STX_KEY_PLUS	m_nSamples++
STX_KEY_MINUS	m_nSamples--
STX_KEY_1	m_Kr
STX_KEY_2	m_Km
STX_KEY_3	g
STX_KEY_4	ESun
STX_KEY_5	Wavelength4[0]
STX_KEY_6	Wavelength4[1]
STX_KEY_7	Wavelength4[2]
STX_KEY_8	Exposure
STX_KEY_SPACE	Stop
STX_KEY_t	Thrust++
STX_KEY_b	Thrust--
STX_KEY_w	Forward Acceleration
STX_KEY_s	Backward Acceleration
STX_KEY_d	Left Acceleration
STX_KEY_a	Right Acceleration
STX_KEY_n	Up Accel
STX_KEY_m	Down Accel
#elif 0
STX_KEY_RIGHT
STX_KEY_LEFT
STX_KEY_UP
STX_KEY_DOWN
STX_KEY_KP7
STX_KEY_KP9
#endif
#endif
}

void CGameEngine::OnChar(int c)
{
	IInput*    i=STX_Service::GetInputInstance();
	if(i->OnKeyPress (STX_KEY_p))
	{
		m_nPolygonMode = !m_nPolygonMode;
		if (m_nPolygonMode)
				IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillSolid());
		else
			IRenderer::GetRendererInstance()->setRasterizerState(
				IRenderer::GetRendererInstance()->GetfillWireFrame());

	}

	if(i->OnKeyPress (STX_KEY_h))
	{
		m_bUseHDR = !m_bUseHDR;
	}

	#if 0
	if(i->OnKeyPress (STX_KEY_PLUS))
	{
		m_nSamples++;
	}

	if(i->OnKeyPress (STX_KEY_MINUS))
	{
		m_nSamples--;
	}
	#else
	if(i->OnKeyPress (STX_KEY_9))
	{
		if(i->OnShiftKeyPress())
			m_nSamples--;
		else
			m_nSamples++;
	}
	#endif
}

void CGameEngine::HandleInput(float fSeconds)
{
	IInput*    i=STX_Service::GetInputInstance();

	if(i->OnKeyPress (STX_KEY_1))
	{
		if(i->OnShiftKeyPress())
			m_Kr = stx_max(0.0f, m_Kr - 0.0001f);
		else
			m_Kr += 0.0001f;
		m_Kr4PI = m_Kr*4.0f*D3DXFROMWINE_PI;
	}
	else if(i->OnKeyPress (STX_KEY_2))
	{
		if(i->OnShiftKeyPress())
			m_Km = stx_max(0.0f, m_Km - 0.0001f);
		else
			m_Km += 0.0001f;
		m_Km4PI = m_Km*4.0f*D3DXFROMWINE_PI;
	}
	else if(i->OnKeyPress (STX_KEY_3))
	{
		if(i->OnShiftKeyPress())
			m_g = stx_max(-1.0f, m_g-0.001f);
		else
			m_g = stx_min(1.0f, m_g+0.001f);
	}
	else if(i->OnKeyPress (STX_KEY_4))
	{
		if(i->OnShiftKeyPress())
			m_ESun = stx_max(0.0f, m_ESun - 0.1f);
		else
			m_ESun += 0.1f;
	}
	else if(i->OnKeyPress (STX_KEY_5))
	{
		if(i->OnShiftKeyPress())
			m_fWavelength[0] = stx_max(0.001f, m_fWavelength[0] -= 0.001f);
		else
			m_fWavelength[0] += 0.001f;
		m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	}
	else if(i->OnKeyPress (STX_KEY_6))
	{
		if(i->OnShiftKeyPress())
			m_fWavelength[1] = stx_max(0.001f, m_fWavelength[1] -= 0.001f);
		else
			m_fWavelength[1] += 0.001f;
		m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	}
	else if(i->OnKeyPress (STX_KEY_7))
	{
		if(i->OnShiftKeyPress())
			m_fWavelength[2] = stx_max(0.001f, m_fWavelength[2] -= 0.001f);
		else
			m_fWavelength[2] += 0.001f;
		m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);
	}
	else if(i->OnKeyPress (STX_KEY_8))
	{
		if(i->OnShiftKeyPress())
			m_fExposure = stx_max(0.1f, m_fExposure-0.1f);
		else
			m_fExposure += 0.1f;
	}


	const float ROTATE_SPEED = 1.0f;
#if 0
    D3DXFROMWINEVECTOR3 vUp=m_3DCamera.GetUpAxis();
    D3DXFROMWINEVECTOR3 vRight=m_3DCamera.GetRightAxis();
    D3DXFROMWINEVECTOR3 vView=m_3DCamera.GetViewAxis();
#else
    D3DXFROMWINEVECTOR3 vUp;
    D3DXFROMWINEVECTOR3 vRight;
	D3DXFROMWINEVECTOR3 vView;
	m_3DCamera.getUp(&vUp);
	m_3DCamera.getRight(&vRight);
	m_3DCamera.getLook(&vView);
#endif
#if 0
    // Turn left/right means rotate around the up axis
	if(i->OnKeyPress (STX_KEY_KP6) || i->OnKeyPress(STX_KEY_RIGHT))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * -ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vUp, fSeconds * -ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.yaw(fSeconds * -ROTATE_SPEED);
		#endif
	}

	if(i->OnKeyPress (STX_KEY_KP4) || i->OnKeyPress(STX_KEY_LEFT))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetUpAxis(), fSeconds * ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vUp, fSeconds * ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.yaw(fSeconds * ROTATE_SPEED);
		#endif
	}

	// Turn up/down means rotate around the right axis
	if(i->OnKeyPress (STX_KEY_KP8) || i->OnKeyPress(STX_KEY_UP))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * -ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vRight, fSeconds * -ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.pitch(fSeconds * -ROTATE_SPEED);
		#endif
	}
	if(i->OnKeyPress (STX_KEY_KP2) || i->OnKeyPress(STX_KEY_DOWN))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetRightAxis(), fSeconds * ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vRight, fSeconds * ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.yaw(fSeconds * ROTATE_SPEED);
		#endif
	}

	// Roll means rotate around the view axis
	if(i->OnKeyPress (STX_KEY_KP7))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * -ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vView, fSeconds * -ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.roll(fSeconds * -ROTATE_SPEED);
		#endif
	}
	if(i->OnKeyPress (STX_KEY_KP9))
	{
		#if 0
		//m_3DCamera.Rotate(m_3DCamera.GetViewAxis(), fSeconds * ROTATE_SPEED);
		D3DXFROMWINEMATRIX out;
		D3DXFROMWINEMatrixRotationAxis(&out, &vView, fSeconds * ROTATE_SPEED);
		IRenderer::GetRendererInstance()->SetTransform(TS_VIEW, out);
		#else
		m_3DCamera.roll(fSeconds * -ROTATE_SPEED);
		#endif
	}
#endif

#define THRUST		1.0f	// Acceleration rate due to thrusters (units/s*s)
#define RESISTANCE	0.1f	// Damping effect on velocity

	// Handle acceleration keys
	D3DXFROMWINEVECTOR3 vAccel(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 vel(0.0f, 0.0f, 0.0f);
#if 1
	m_3DCamera.getRight(&vRight);
	m_3DCamera.getUp(&vUp);
	m_3DCamera.getLook(&vView);
#endif
	//else
	{
		// Add camera's acceleration due to thrusters
		float fThrust = THRUST;
		if(i->OnKeyPress(STX_KEY_t))
			fThrust *= 10.0f;
		if(i->OnKeyPress(STX_KEY_b))
			fThrust *= 0.1f;

		// Thrust forward/reverse affects velocity along the view axis
		if(i->OnKeyPress (STX_KEY_w))
			vAccel += vView * fThrust;
		if(i->OnKeyPress (STX_KEY_s))
			vAccel += vView * -fThrust;

		// Thrust left/right affects velocity along the right axis
		if(i->OnKeyPress (STX_KEY_d))
			vAccel += vRight * fThrust;
		if(i->OnKeyPress (STX_KEY_a))
			vAccel += vRight * -fThrust;

		// Thrust up/down affects velocity along the up axis
//#define WORLD_UPDOWN
#ifdef WORLD_UPDOWN
		D3DXFROMWINEVECTOR3 v = m_3DCamera.GetPosition();
		v.normalize();
		if(i->OnKeyPress (STX_KEY_n))
		{

			vAccel += v * fThrust;
		}
		if(i->OnKeyPress (STX_KEY_m))
		{

			vAccel += v * -fThrust;
		}
#else
		if(i->OnKeyPress (STX_KEY_n))
		{

			vAccel += vUp * fThrust;
		}
		if(i->OnKeyPress (STX_KEY_m))
		{

			vAccel += vUp * -fThrust;
		}
#endif
#if 0
		m_3DCamera.Accelerate(vAccel, fSeconds, RESISTANCE);
		D3DXFROMWINEVECTOR3 vPos;
		m_3DCamera.getPosition(&vPos);
		float fMagnitude = D3DXFROMWINEVec3Length(&vPos);
		if(fMagnitude < m_fInnerRadius)
		{
			vPos *= (m_fInnerRadius * (1 + DELTA)) / fMagnitude;
			D3DXFROMWINEVECTOR3 v(vPos.x, vPos.y, vPos.z);
			m_3DCamera.setPosition(&v);
			D3DXFROMWINEVECTOR3 v2=-m_3DCamera.GetVelocity();
			m_3DCamera.SetVelocity(v2);
		}
#else
		static float C=0.001f;
		if(i->OnKeyPress (STX_KEY_HOME))
			C*=0.1f;
		if(i->OnKeyPress (STX_KEY_END))
			C*=10.0f;
		static float r=m_fAltitude;
		if(i->OnKeyPress (STX_KEY_PAGEDOWN))
		{
			r-=0.1f*(m_fOuterRadius-m_fInnerRadius);
			if(r<0.0f)
				r=0.0f;
		}
		if(i->OnKeyPress (STX_KEY_PAGEUP))
			r+=0.1f*(m_fOuterRadius-m_fInnerRadius);
		static D3DXFROMWINEVECTOR3 vR;
		if(i->OnKeyPress (STX_KEY_RIGHT))
			vR.x+=D3DXFROMWINE_PI/2.0f;
		if(i->OnKeyPress (STX_KEY_LEFT))
			vR.x-=D3DXFROMWINE_PI/2.0f;
		if(i->OnKeyPress (STX_KEY_UP))
			vR.y+=D3DXFROMWINE_PI/2.0f;
		if(i->OnKeyPress (STX_KEY_DOWN))
			vR.y-=D3DXFROMWINE_PI/2.0f;
	static bool bOrbit=true;
	if(i->OnKeyPress (STX_KEY_o))
		bOrbit=!bOrbit;
	if(i->OnKeyPress(STX_KEY_SPACE))
	{
		//m_3DCamera.SetVelocity(vel);	// Full stop
		//static float r=m_fAltitude;
		static float r=0.5f*(m_fOuterRadius-m_fInnerRadius)+m_fOuterRadius;
		float dt=0.0f;
		D3DXFROMWINEVECTOR3 pos(r*cosf(dt), 0.0f, r*sinf(dt));
		D3DXFROMWINEVECTOR3 vel;
		D3DXFROMWINEVECTOR3 acc;
		D3DXFROMWINEVECTOR3 look=vel;
		D3DXFROMWINEVec3Normalize(&look, &look);
		D3DXFROMWINEVECTOR3 vR;
		D3DXFROMWINEMATRIX R;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, vR.x, vR.y, vR.z);
		D3DXFROMWINEVec3TransformCoord(&look, &look, &R);
		m_3DCamera.setPosition(&pos);
		m_3DCamera.setLook(&look);
		m_3DCamera.SetVelocity(vel);
		m_3DCamera.Accelerate(acc, fSeconds, RESISTANCE);
		bOrbit=false;
	}
	if(bOrbit)
	{
		static float t0=C*timeGetTime();
		float t=C*timeGetTime();
		float dt=0.0f;
		if(t>t0)
			dt=t-t0;
		D3DXFROMWINEVECTOR3 pos(r*cosf(dt), 0.0f, r*sinf(dt));
		D3DXFROMWINEVECTOR3 vel(-r*sinf(dt), 0.0f, r*cosf(dt));
		D3DXFROMWINEVECTOR3 acc(-r*cosf(dt), 0.0f, -r*sinf(dt));

		// v=sqrt(G*M/r)

		D3DXFROMWINEVECTOR3 look=vel;
		D3DXFROMWINEVec3Normalize(&look, &look);

		D3DXFROMWINEMATRIX R;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, vR.x, vR.y, vR.z);
		D3DXFROMWINEVec3TransformCoord(&look, &look, &R);

		m_3DCamera.setPosition(&pos);
		m_3DCamera.setLook(&look);
		m_3DCamera.SetVelocity(vel);
		m_3DCamera.Accelerate(acc, fSeconds, RESISTANCE);
	}
#endif
	}
}
CGameEngine* app=0;
int init(const char* aTitle)
{
	return 0;
}

void render()
{
#if 0
	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX mvp;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

	app->innersphere.Draw(mvp, app->m_tEarth);
#elif 1
	app->RenderFrame(timeGetTime());
#elif 0
	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	app->renderMoon();
#endif
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("AccurateAtmosphericalScattering");
	IInput*    i=STX_Service::GetInputInstance();
	app=new CGameEngine();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	delete app;
	return 0;
}

