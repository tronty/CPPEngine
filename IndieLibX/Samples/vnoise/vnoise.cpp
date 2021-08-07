/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

ShaderID shd=-1;
VertexFormatID vf=-1;
D3DXFROMWINEVECTOR4* g=0;

D3DXFROMWINEVECTOR3 noise_translate;
float s=0.5f;
D3DXFROMWINEVECTOR3 noise_scale(s, s, s);
float displacement = 1.0;
unsigned int dlist;

int permGradTableParam;
MeshRenderer2 shape3D;

float sfrand()
{
    return (stx_rand() * 2.0f / (float) RAND_MAX) - 1.0f;
}



int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/vnoise.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
    shape3D.CreateSphere(1.0f, eShaderNone);

    // initalize constants for noise
    //srand(42);
    //g=initVertexNoiseConstants(32);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX W,R,S,T;
    D3DXFROMWINEMatrixScaling(&S, noise_scale.x, noise_scale.y, noise_scale.z);
    D3DXFROMWINEMatrixTranslation(&T, noise_translate.x, noise_translate.y, noise_translate.z);
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
	W=R*S*T;


		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
	D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);
	shape3D.BeginDraw(&W, -1, shd, vf, AmbientColor, DiffuseColor, LightDir, EyeDir);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
/*
    glUniform4fvARB(permGradTableParam, table_size*2, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2), 1, g[0].v);
    glUniform4fvARB(permGradTableParam + (table_size*2)+1, 1, g[1].v);
*/
	shape3D.EndDraw();

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
/*
void key(unsigned char k, int x, int y)
{

    case '=':
    case '+':
      noise_scale[0] = noise_scale[1] = noise_scale[2] += 0.1;
      break;
    case '-':
    case '_':
      noise_scale[0] = noise_scale[1] = noise_scale[2] -= 0.1;
      break;

    case ']':
      displacement += 0.1f;
      break;
    case '[':
      displacement -= 0.1f;
      break;

    case 'r':
      noise_scale = D3DXFROMWINEVECTOR3(1.0);
      noise_translate = D3DXFROMWINEVECTOR3(0.0);
      break;
    }
}

void idle()
{
        object.trackball.increment_rotation();
        noise_translate[1] -= 0.01;
}
*/
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("GLSL noise displacement");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
