/* Copyright (c) Mark J. Kilgard, 1994. */

/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class Teapot : public STXGUI {
public:
	Teapot(){}
	virtual ~Teapot(){}
	MeshRenderer2 teapot;
	float mAngleX;
	float mAngleY;
	virtual void init(const char* aTitle="")
	{
		//STXGUI::init("/3DAnimation/GUILayout.xml");
		teapot.Load("/dx9sdk/teapot.x", ePhongLighting2, true, true);
	mAngleX=0.0f;
	mAngleY=295.0f;
  float ambient[] =
  {0.0, 0.0, 0.0, 1.0};
  float diffuse[] =
  {1.0, 1.0, 1.0, 1.0};
  float position[] =
  {0.0, 3.0, 3.0, 0.0};

  float lmodel_ambient[] =
  {0.2, 0.2, 0.2, 1.0};
  float local_view[] =
  {0.0};
}

void
renderTeapot(float ax, float ay,
  float ambr, float ambg, float ambb,
  float difr, float difg, float difb,
  float specr, float specg, float specb, float shine)
{
  float x=ax-10.0f;
  float y=ay-10.0f;
  float mat[4];
  //glTranslatef(x, y, 0.0);
  D3DXFROMWINEMATRIX I,mvp,R,S,T;
  float s=10.0f;
  D3DXFROMWINEMatrixTranslation(&T, x/s-s*0.5f, y/s-s*0.5f, 0.0f);
  D3DXFROMWINEMatrixTranslation(&T, x/s, y/s, 0.0f);
  D3DXFROMWINEMatrixScaling(&S, 1.0f/s, 1.0f/s, 1.0f/s);
  D3DXFROMWINEMatrixIdentity(&I);
  mat[0] = ambr;
  mat[1] = ambg;
  mat[2] = ambb;
  mat[3] = 1.0;
  //glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
  mat[0] = difr;
  mat[1] = difg;
  mat[2] = difb;
  //glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
  mat[0] = specr;
  mat[1] = specg;
  mat[2] = specb;
  //glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
  //glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
  D3DXFROMWINEVECTOR4 a(ambr, ambg, ambb, 1.0f);
  D3DXFROMWINEVECTOR4 d(difr, difg, difb, 1.0f);
  D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
  teapot.SetAmbientColor(a);
  teapot.SetDiffuseColor(d);
  teapot.SetEyeDir(e);

    D3DXFROMWINEMATRIX mLightModelMatrix;
    D3DXFROMWINEVECTOR4 mLightPosInWorldSpace, mLightPosInModelSpace;
	static float angleInRadians = 2.0f*D3DXFROMWINE_PI * 1.0f;
	angleInRadians += (float)D3DXFROMWINE_PI * 0.0005f;
    D3DXFROMWINEMatrixRotationY(&mLightModelMatrix, angleInRadians);
    mLightPosInModelSpace=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

    D3DXFROMWINEVec4TransformCoord(&mLightPosInWorldSpace, &mLightPosInModelSpace, &mLightModelMatrix);
	teapot.SetLightDir(mLightPosInWorldSpace);

	stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
  mvp=R*S*T;
  teapot.render(&mvp);
}

/*
 *  First column:  emerald, jade, obsidian, pearl, ruby, turquoise
 *  2nd column:  brass, bronze, chrome, copper, gold, silver
 *  3rd column:  black, cyan, green, red, white, yellow plastic
 *  4th column:  black, cyan, green, red, white, yellow rubber
 */
	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
		IRenderer::GetRendererInstance()->BeginScene();
  renderTeapot(2.0, 17.0, 0.0215, 0.1745, 0.0215,
    0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6);
  renderTeapot(2.0, 14.0, 0.135, 0.2225, 0.1575,
    0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1);
  renderTeapot(2.0, 11.0, 0.05375, 0.05, 0.06625,
    0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3);
  renderTeapot(2.0, 8.0, 0.25, 0.20725, 0.20725,
    1, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088);
  renderTeapot(2.0, 5.0, 0.1745, 0.01175, 0.01175,
    0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6);
  renderTeapot(2.0, 2.0, 0.1, 0.18725, 0.1745,
    0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1);
  renderTeapot(6.0, 17.0, 0.329412, 0.223529, 0.027451,
    0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843,
    0.21794872);
  renderTeapot(6.0, 14.0, 0.2125, 0.1275, 0.054,
    0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2);
  renderTeapot(6.0, 11.0, 0.25, 0.25, 0.25,
    0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6);
  renderTeapot(6.0, 8.0, 0.19125, 0.0735, 0.0225,
    0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1);
  renderTeapot(6.0, 5.0, 0.24725, 0.1995, 0.0745,
    0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4);
  renderTeapot(6.0, 2.0, 0.19225, 0.19225, 0.19225,
    0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4);
  renderTeapot(10.0, 17.0, 0.0, 0.0, 0.0, 0.01, 0.01, 0.01,
    0.50, 0.50, 0.50, .25);
  renderTeapot(10.0, 14.0, 0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392,
    0.50196078, 0.50196078, 0.50196078, .25);
  renderTeapot(10.0, 11.0, 0.0, 0.0, 0.0,
    0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25);
  renderTeapot(10.0, 8.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
    0.7, 0.6, 0.6, .25);
  renderTeapot(10.0, 5.0, 0.0, 0.0, 0.0, 0.55, 0.55, 0.55,
    0.70, 0.70, 0.70, .25);
  renderTeapot(10.0, 2.0, 0.0, 0.0, 0.0, 0.5, 0.5, 0.0,
    0.60, 0.60, 0.50, .25);
  renderTeapot(14.0, 17.0, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01,
    0.4, 0.4, 0.4, .078125);
  renderTeapot(14.0, 14.0, 0.0, 0.05, 0.05, 0.4, 0.5, 0.5,
    0.04, 0.7, 0.7, .078125);
  renderTeapot(14.0, 11.0, 0.0, 0.05, 0.0, 0.4, 0.5, 0.4,
    0.04, 0.7, 0.04, .078125);
  renderTeapot(14.0, 8.0, 0.05, 0.0, 0.0, 0.5, 0.4, 0.4,
    0.7, 0.04, 0.04, .078125);
  renderTeapot(14.0, 5.0, 0.05, 0.05, 0.05, 0.5, 0.5, 0.5,
    0.7, 0.7, 0.7, .078125);
  renderTeapot(14.0, 2.0, 0.05, 0.05, 0.0, 0.5, 0.5, 0.4,
    0.7, 0.7, 0.04, .078125);
  		//STXGUI::update();

		const char* txt = "Use mouse buttons to rotate the model.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		txt = "CTRL-r resets the scene.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		char txt2[128];

		stx_snprintf(txt2, 128, "AngleX=%f\n", mAngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleY=%f\n", mAngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present();
	}

	virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		#if 0
		if(callbackString == "")
		{
			m_i=;
		}
		#endif
	}
	}
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("teapots_8c");
	IInput*    i=STX_Service::GetInputInstance();
	Teapot app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}

