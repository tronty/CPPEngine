#define STRICT
#include <iostream>
#include <fstream>
using namespace std;
#include <Framework3/IRenderer.h>
#define __ATLAS__ 1
#ifdef __ATLAS__
#include "particleatlas.h"
#endif

D3DXFROMWINEVECTOR3 g_CameraPosition;
D3DXFROMWINEMATRIX g_CameraProjMatrix;
D3DXFROMWINEMATRIX g_CameraViewMatrix;
float g_CameraSpeed;
D3DXFROMWINEVECTOR3 vO;

ShaderID shd3=-1;
VertexFormatID vf3=-1;

ShaderID shd4=-1;
VertexFormatID vf4=-1;

/*
enum x {
    PRIM_LINES,
    PRIM_LINE_STRIP,
};*/
extern char names[10][256];
const int constellationnum=89;
char constellationnames[constellationnum][25]={
"Andromeda",
"Antlia",
"Apus",
"Aquarius",
"Aquila",
"Ara",
"Aries",
"Auriga",
"Bootes",
"Caelum",
"Camelopardus",
"Cancer",
"Canes Venatici",
"Canis Major",
"Canis Minor",
"Capricornus",
"Carina",
"Cassiopeia",
"Centaurus",
"Cepheus",
"Cetus",
"Chamaeleon",
"Circinus",
"Columba",
"Coma Berenices",
"Corona Australis",
"Corona Borealis",
"Corvus",
"Crater",
"Crux",
"Cygnus",
"Delphinus",
"Dorado",
"Draco",
"Equuleus",
"Eridanus",
"Fornax",
"Gemini",
"Grus",
"Hercules",
"Horologium",
"Hydra",
"Hydrus",
"Indus",
"Lacerta",
"Leo",
"Leo Minor",
"Lepus",
"Libra",
"Lupus",
"Lynx",
"Lyra",
"Mensa",
"Microscopium",
"Monoceros",
"Musca",
"Norma",
"Octans",
"Ophiuchus",
"Orion",
"Pavo",
"Pegasus",
"Perseus",
"Phoenix",
"Pictor",
"Pisces",
"Pisces Austrinus",
"Puppis",
"Pyxis",
"Reticulum",
"Sagitta",
"Sagittarius",
"Scorpius",
"Sculptor",
"Scutum",
"Serpens",
"Sextans",
"Taurus",
"Telescopium",
"Triangulum",
"Triangulum Australe",
"Tucana",
"Ursa Major",
"Ursa Minor",
"Vela",
"Virgo",
"Volans",
"Vulpecula"};

char constellationabbr[constellationnum][4]={"And","Ant","Aps","Aqr","Aql","Ara","Ari","Aur","Boo","Cae","Cam","Cnc","CVn","CMa","CMi","Cap","Car","Cas","Cen","Cep","Cet",
		"Cha","Cir","Col","Com","CrA","CrB","Crv","Crt","Cru","Cyg","Del","Dor","Dra","Equ","Eri","For","Gem","Gru","Her","Hor","Hya",
		"Hyi","Ind","Lac","Leo","LMi","Lep","Lib","Lup","Lyn","Lyr","Men","Mic","Mon","Mus","Nor","Oct","Oph","Ori","Pav","Peg","Per",
		"Phe","Pic","Psc","PsA","Pup","Pyx","Ret","Sge","Sgr","Sco","Scl","Sct","Ser","Sex","Tau","Tel","Tri","TrA","Tuc","UMa","UMi",
		"Vel","Vir","Vol","Vul"
};

D3DXFROMWINEVECTOR3 constellationpositions[constellationnum];
D3DXFROMWINEVECTOR3 constellationscreenpositions[constellationnum];
//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
#ifdef __ATLAS__
cParticleEmitter* m_pSkyEmitter=0;
#endif
//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
void loadTexture(void);
void shutDown(void);
void LoadBorders();
void LoadFigures();

D3DXFROMWINEMATRIX GetMVP()
{
	D3DXFROMWINEMATRIX matRot;

		static float mAngleX=0.0f;
		static float mAngleY=0.0f;
		static float mSpeedRotation=0.05f;
		float mDelta = timeGetTime() / 1000.0f;
		if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_LEFT))	mAngleX += mSpeedRotation * mDelta;
		if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT))	mAngleY += mSpeedRotation * mDelta;

		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
#if 0
	D3DXFROMWINEMATRIX matView, matProj;
	static D3DXFROMWINEVECTOR3 _look(0.0f, 0.0f, 1.0f);
	static D3DXFROMWINEVECTOR3 _right(1.0f, 0.0f, 0.0f);
	static D3DXFROMWINEVECTOR3 _up(0.0f, 1.0f, 0.0f);
	static D3DXFROMWINEVECTOR3 _at(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVec3TransformCoord(&_look, &_look, &matRot);
	D3DXFROMWINEVec3TransformCoord(&_right, &_right, &matRot);
	D3DXFROMWINEVec3TransformCoord(&_up, &_up, &matRot);

		if (STX_Service::GetInputInstance()->OnMouseButtonPress	(STX_MBUTTON_WHEELUP)) _at += _look*mSpeedRotation * mDelta;
		if (STX_Service::GetInputInstance()->OnMouseButtonPress	(STX_MBUTTON_WHEELDOWN)) _at -= _look*mSpeedRotation * mDelta;

	D3DXFROMWINEMatrixLookAtLH(&matView, &_look, &_at, &_up);
	D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.001f, 1200000.0f);

	D3DXFROMWINEMATRIX mvp = matRot*matView*matProj;
	D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
#else
	D3DXFROMWINEMATRIX mvp = matRot;
#endif
	return mvp;
}

void PrintSphe0(D3DXFROMWINEVECTOR3 c1)
{
	D3DXFROMWINEVECTOR3 c2;

	float radius=sqrt(c1.x*c1.x+c1.y*c1.y+c1.z*c1.z);
	float phi = asinf(c1.z / radius);
	float theta;//

#if 0
	theta = acosf(c1.x / (radius * cosf(phi)));
#else
	if(0.0<=c1.y)
		theta = acosf(c1.x / (radius* cosf(phi)));
	else if(c1.y<0.0f)
		theta = -acosf(c1.x /(radius*cosf(phi)));
#endif

	c2.x = radius * cosf(phi) * cosf(theta);
	c2.y = radius * cosf(phi) * sinf(theta);
	c2.z = radius * sinf(phi);// = should be identical to starting cZ value

	LOG_PRINT("c1.x=%f\n",c1.x);
	LOG_PRINT("c1.y=%f\n",c1.y);
	LOG_PRINT("c1.z=%f\n",c1.z);
	LOG_PRINT("c2.x=%f\n",c2.x);
	LOG_PRINT("c2.y=%f\n",c2.y);
	LOG_PRINT("c2.z=%f\n",c2.z);
/*
	phi = acosf(c1.z / radius);
	if(0.0<=c1.x)
		theta = asinf(c1.y / radius);
	else if(c1.x<0.0f)
		theta = D3DXFROMWINE_PI-asinf(c1.y / radius);

	c2.x=radius*sinf(phi)*cosf(theta);
    c2.z=radius*sinf(phi)*sinf(theta);
    c2.y=radius*cosf(phi);

	LOG_PRINT("2c1.x=%f\n",c1.x);
	LOG_PRINT("2c1.y=%f\n",c1.y);
	LOG_PRINT("2c1.z=%f\n",c1.z);
	LOG_PRINT("2c2.x=%f\n",c2.x);
	LOG_PRINT("2c2.y=%f\n",c2.y);
	LOG_PRINT("2c2.z=%f\n",c2.z);
	*/
}

void PrintSphe(D3DXFROMWINEVECTOR3 c1)
{
	D3DXFROMWINEVECTOR3 c2;

	float radius=sqrt(c1.x*c1.x+c1.y*c1.y+c1.z*c1.z);
	float phi = asinf(c1.y / radius);
	float theta;//

#if 0
	theta = acosf(c1.x / (radius * cosf(phi)));
#else
	if(0.0<=c1.z)
		theta = acosf(c1.x / (radius* cosf(phi)));
	else if(c1.z<0.0f)
		theta = -acosf(c1.x /(radius*cosf(phi)));
#endif

	c2.x = radius * cosf(phi) * cosf(theta);
	c2.z = radius * cosf(phi) * sinf(theta);
	c2.y = radius * sinf(phi);// = should be identical to starting cZ value

	LOG_PRINT("c1.x=%f\n",c1.x);
	LOG_PRINT("c1.y=%f\n",c1.y);
	LOG_PRINT("c1.z=%f\n",c1.z);
	LOG_PRINT("c2.x=%f\n",c2.x);
	LOG_PRINT("c2.y=%f\n",c2.y);
	LOG_PRINT("c2.z=%f\n",c2.z);
/*
	phi = acosf(c1.z / radius);
	if(0.0<=c1.x)
		theta = asinf(c1.y / radius);
	else if(c1.x<0.0f)
		theta = D3DXFROMWINE_PI-asinf(c1.y / radius);

	c2.x=radius*sinf(phi)*cosf(theta);
    c2.z=radius*sinf(phi)*sinf(theta);
    c2.y=radius*cosf(phi);

	LOG_PRINT("2c1.x=%f\n",c1.x);
	LOG_PRINT("2c1.y=%f\n",c1.y);
	LOG_PRINT("2c1.z=%f\n",c1.z);
	LOG_PRINT("2c2.x=%f\n",c2.x);
	LOG_PRINT("2c2.y=%f\n",c2.y);
	LOG_PRINT("2c2.z=%f\n",c2.z);
	*/
}
const int constellationFigureNum=11;
char constellationFigureFiles[constellationFigureNum][20]={
"dhmenz11",
"Karkoschka_cf",
"rukl_cf",
"Skywatch8",
"hareyv8",
"dhmenzv8",
"hareyv5",
"dhmenzv5",
"Skywatch",
"harey",
"dhmenzel"
};
int figureFileID=0;
struct FigureVertex2
{
	int cmd,id;
};
FigureVertex2 constellationFigures[9820];//9968-12-136
int constellationFigureStart[constellationFigureNum];
int constellationFigureNum2[constellationFigureNum];
//extern cParticleEmitter::CatalogElement;
//extern cParticleEmitter::CatalogElement cParticleEmitter::catalog[9111];

//-----------------------------------------------------------------------------
// Name: init()
// Desc:
//-----------------------------------------------------------------------------
int init(const char* aTitle)
{


	#if defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	const char* SHD3="[Vertex shader]\n"
	"attribute mediump vec3 aPosition;\n"
	"attribute mediump vec4 aTexCoord;\n"
	"varying mediump vec4 vTexCoord;\n"
	"uniform highp mat4 mvp;\n"
	"void main(){\n"
	"	gl_Position = mvp*vec4(aPosition.x, aPosition.y, aPosition.z, 1);\n"
	"	vTexCoord=aTexCoord;\n"
	"}\n"
	"[Fragment shader]\n"
	"varying mediump vec4 vTexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_FragColor = vTexCoord;\n"
	"}\n";
	#else
	const char* SHD3="[Vertex shader]\n"
	"struct VsIn {\n"
	"	float3 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"struct VsOut {\n"
	"	float4 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"ROW_MAJOR float4x4 mvp MVPSEMANTIC;\n"
	"VsOut main(VsIn In){\n"
	"	VsOut Out=(VsOut)0;\n"
	"	Out.position = mul(mvp, float4(In.position.x, In.position.y, In.position.z, 1.0));\n"
	"	Out.texCoord = In.texCoord;\n"
	"	return Out;\n"
	"}\n"
	"[Fragment shader]\n"
	"struct VsOut {\n"
	"	float4 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"float4 main(VsOut IN): COLOR {\n"
	"	return IN.texCoord;\n"
	"}\n";
	#endif

		shd3 = IRenderer::GetRendererInstance()->addHLSLShader(SHD3, "main", "main");

		FormatDesc format3[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
		};
		vf3 = IRenderer::GetRendererInstance()->addVertexFormat(format3, elementsOf(format3), shd3);

	#if defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	const char* SHD4="[Vertex shader]\n"
	"attribute mediump vec4 aPosition;\n"
	"attribute mediump vec4 aTexCoord;\n"
	"varying mediump vec4 vTexCoord;\n"
	"uniform highp mat4 mvp;\n"
	"void main(){\n"
	"	gl_Position = mvp*aPosition;\n"
	"	vTexCoord=aTexCoord;\n"
	"}\n"
	"[Fragment shader]\n"
	"varying mediump vec4 vTexCoord;\n"
	"void main()\n"
	"{\n"
	"gl_FragColor = vTexCoord;\n"
	"}\n";
	#else
	const char* SHD4="[Vertex shader]\n"
	"struct VsIn {\n"
	"	float4 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"struct VsOut {\n"
	"	float4 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"ROW_MAJOR float4x4 mvp MVPSEMANTIC;\n"
	"VsOut main(VsIn In){\n"
	"	VsOut Out=(VsOut)0;\n"
	"	Out.position = mul(mvp, In.position);\n"
	"	Out.texCoord = In.texCoord;\n"
	"	return Out;\n"
	"}\n"
	"[Fragment shader]\n"
	"struct VsOut {\n"
	"	float4 position: POSITION;\n"
	"	float4 texCoord: TEXCOORD0;\n"
	"};\n"
	"float4 main(VsOut IN): COLOR {\n"
	"	return IN.texCoord;\n"
	"}\n";
	#endif
		shd4 = IRenderer::GetRendererInstance()->addHLSLShader(SHD4, "main", "main");

		FormatDesc format4[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
		};
		vf4 = IRenderer::GetRendererInstance()->addVertexFormat(format4, elementsOf(format4), shd4);


		figureFileID=0;//???
		//LOG_PRINT("w/h=%f\n",width/float(height));
		D3DXFROMWINEMatrixPerspectiveFovLH(&g_CameraProjMatrix, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/float(IRenderer::GetRendererInstance()->GetViewportHeight()), 0.1f, 10000.0f);


		D3DXFROMWINEVECTOR3 pvEyePt=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
		D3DXFROMWINEVECTOR3 pvLookatPt=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
		D3DXFROMWINEVECTOR3 up(0.0f,1.0f,0.0f);

		D3DXFROMWINEMatrixLookAtLH(&g_CameraViewMatrix, &pvEyePt, &pvLookatPt, &up);

		g_CameraSpeed=0.001f;
	#ifdef __ATLAS__
		m_pSkyEmitter=new cParticleEmitter();
	#endif
	figureFileID=0;//???
	LOG_PRINT("w/h=%f\n",width/float(height));
	g_Camera.SetProjParams( D3DXFROMWINEToRadian(45.0f), width/float(height), 0.1f, 10000.0f );
	D3DXFROMWINEVECTOR3 pvEyePt=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
	D3DXFROMWINEVECTOR3 pvLookatPt=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	g_Camera.SetViewParams(pvEyePt, pvLookatPt );

	g_Camera.speed0=0.001f;;
	g_Camera.reset();
	m_pSkyEmitter=new cParticleEmitter();
/*
	int rah;
	int r=sscanf ("-45","%d",&rah);
	LOG_PRINT("%d\n",rah);
	float rah2;
	r=sscanf ("45.1","%f",&rah2);
	LOG_PRINT("%f\n",rah2);
*/

		D3DXFROMWINEVECTOR3 c1;
	LOG_PRINT("1\n");
	c1.x=1.0f;
	c1.y=1.0f;
	c1.z=1.0f;
	PrintSphe(c1);
	LOG_PRINT("2\n");
	c1.x=-1.0f;
	c1.y=1.0f;
	c1.z=1.0f;
	PrintSphe(c1);
	LOG_PRINT("3\n");
	c1.x=1.0f;
	c1.y=-1.0f;
	c1.z=1.0f;
	PrintSphe(c1);
	LOG_PRINT("4\n");
	c1.x=1.0f;
	c1.y=1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("5\n");
	c1.x=-1.0f;
	c1.y=-1.0f;
	c1.z=1.0f;
	PrintSphe(c1);
	LOG_PRINT("6\n");
	c1.x=1.0f;
	c1.y=-1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("7\n");
	c1.x=-1.0f;
	c1.y=1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("8\n");
	c1.x=-1.0f;
	c1.y=-1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);

	LOG_PRINT("9\n");
	c1.x= 0.0f;
	c1.y= 1.0f;
	c1.z= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("10\n");
	c1.x= 0.0f;
	c1.y=-1.0f;
	c1.z= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("11\n");
	c1.x= 0.0f;
	c1.y= 1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("12\n");
	c1.x= 0.0f;
	c1.y=-1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);

	LOG_PRINT("13\n");
	c1.y= 0.0f;
	c1.x= 1.0f;
	c1.z= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("14\n");
	c1.y= 0.0f;
	c1.x=-1.0f;
	c1.z= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("15\n");
	c1.y= 0.0f;
	c1.x= 1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("16\n");
	c1.y= 0.0f;
	c1.x=-1.0f;
	c1.z=-1.0f;
	PrintSphe(c1);

	LOG_PRINT("17\n");
	c1.z= 0.0f;
	c1.y= 1.0f;
	c1.x= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("18\n");
	c1.z= 0.0f;
	c1.y=-1.0f;
	c1.x= 1.0f;
	PrintSphe(c1);
	LOG_PRINT("19\n");
	c1.z= 0.0f;
	c1.y= 1.0f;
	c1.x=-1.0f;
	PrintSphe(c1);
	LOG_PRINT("20\n");
	c1.z= 0.0f;
	c1.y=-1.0f;
	c1.x=-1.0f;
	PrintSphe(c1);

	//hexdump("Constellations/ConstellationFigures/dhmenz11/CONSTFIG.DAT");
	LoadBorders();
	LoadFigures();
	for(unsigned int i=0;i<constellationFigureNum;i++)
	{
		LOG_PRINT("=%s\n",&constellationFigureFiles[i][0]);
		LOG_PRINT("=%d\n",constellationFigureStart[i]);
		LOG_PRINT("=%d\n",constellationFigureNum2[i]);
	}
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc:
//-----------------------------------------------------------------------------
void shutDown( void )
{
    if(m_pSkyEmitter) delete m_pSkyEmitter;

    DeInitD3D9();
}
struct line_vertex4{
    D3DXFROMWINEVECTOR4 pos;  // The transformed(screen space) position for the vertex.
    DWORD colour;	 // The vertex colour.
};
struct line_vertex{
    D3DXFROMWINEVECTOR3 pos;  // The transformed(screen space) position for the vertex.
    DWORD colour;	 // The vertex colour.
};

//const DWORD line_fvf=D3DFVF_XYZRHW|D3DFVF_DIFFUSE;

D3DXFROMWINEVECTOR3 GetPosition3(float varphi,float theta,float r=1000.0f)
{
	D3DXFROMWINEVECTOR3 position;
	float varphi2=varphi*D3DXFROMWINE_PI/180.0f;
	float theta2=theta*D3DXFROMWINE_PI/180.0f;
	position.x = r * cosf(theta2) * cosf(varphi2)+g_Camera._pos.x;
	position.z = r * cosf(theta2) * sinf(varphi2)+g_Camera._pos.z;
	position.y = r * sinf(theta2)+g_Camera._pos.y;// = should be identical to starting cZ value
	return position;
}
D3DXFROMWINEVECTOR4 GetPosition(float varphi,float theta,float r=1000.0f)
{
D3DXFROMWINEVECTOR3 position=GetPosition3(varphi,theta,r);
	D3DXFROMWINEVECTOR3 screenposition;
  DWORD stride=sizeof(D3DXFROMWINEVECTOR3);
	D3DVIEWPORT9 viewport = { 0, 0, width, height, 0, 1 };
   D3DXFROMWINEMATRIX matProj, matView, matWorld;
   matProj=g_Camera.GetProjMatrix();
   matView=g_Camera.GetViewMatrix();
   D3DXFROMWINEMatrixIdentity(&matWorld);
	D3DXFROMWINEVec3Project(&screenposition, &position,
			    &viewport, &matProj, &matView, &matWorld
			    );

	D3DXFROMWINEVECTOR4 position4(screenposition.x,screenposition.y,0.0f,1.0f);
	return position4;
}/*
struct Plane
{
	float a;
	float b;
	float c;
	float d;
};*/
#if 0
D3DXFROMWINEPLANE m_frustum[6];
bool SphereInFrustum( float x, float y, float z, float radius )
{
      D3DXFROMWINEMATRIX matWorld, matView, matProjection;
//	V(g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld));
      V(g_pd3dDevice->GetTransform(D3DTS_VIEW,	&matView));
      V(g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection));

D3DXFROMWINEMATRIX viewProjection;
D3DXFROMWINEMatrixMultiply( &viewProjection, &matView, &matProjection );
//D3DXFROMWINEMatrixMultiply( &viewProjection, &m_mView, &m_mProjection );
// Left plane
m_frustum[0].a = viewProjection._14 + viewProjection._11;
m_frustum[0].b = viewProjection._24 + viewProjection._21;
m_frustum[0].c = viewProjection._34 + viewProjection._31;
m_frustum[0].d = viewProjection._44 + viewProjection._41;

// Right plane
m_frustum[1].a = viewProjection._14 - viewProjection._11;
m_frustum[1].b = viewProjection._24 - viewProjection._21;
m_frustum[1].c = viewProjection._34 - viewProjection._31;
m_frustum[1].d = viewProjection._44 - viewProjection._41;

// Top plane
m_frustum[2].a = viewProjection._14 - viewProjection._12;
m_frustum[2].b = viewProjection._24 - viewProjection._22;
m_frustum[2].c = viewProjection._34 - viewProjection._32;
m_frustum[2].d = viewProjection._44 - viewProjection._42;

// Bottom plane
m_frustum[3].a = viewProjection._14 + viewProjection._12;
m_frustum[3].b = viewProjection._24 + viewProjection._22;
m_frustum[3].c = viewProjection._34 + viewProjection._32;
m_frustum[3].d = viewProjection._44 + viewProjection._42;

// Near plane
m_frustum[4].a = viewProjection._13;
m_frustum[4].b = viewProjection._23;
m_frustum[4].c = viewProjection._33;
m_frustum[4].d = viewProjection._43;

// Far plane
m_frustum[5].a = viewProjection._14 - viewProjection._13;
m_frustum[5].b = viewProjection._24 - viewProjection._23;
m_frustum[5].c = viewProjection._34 - viewProjection._33;
m_frustum[5].d = viewProjection._44 - viewProjection._43;

// Normalize planes
for ( int i = 0; i < 6; i++ )
{
D3DXFROMWINEPlaneNormalize( &m_frustum[i], &m_frustum[i] );

}


for ( int i = 0; i < 6; i++ )
{
if ( D3DXFROMWINEPlaneDotCoord( &m_frustum[i], &D3DXFROMWINEVECTOR3(x,y,z) ) + radius < 0 )
{
// Outside the frustum, reject it!
return FALSE;

}

}
return TRUE;

}
/*
  ////////////////////////////////////////////////////////////////////////////////////

//										  \\

// This is a simple Frustum Function, which is for DirectX3D, its used for	 \\

// tutorial purposes, and demonstrates how you can implement the Frustum Check	  \\

// using DirectX.  How it works is, you pass in the coordinate for the vertex	  \\

// point your checking, then if its in the frustum, it will return true, else it  \\

// not, so will return false....and you can take the appropriate action.	   \\

//										  \\

\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
*/


bool PointInFrustum(float x, float y, float z)
{

      D3DXFROMWINEMATRIX matWorld, matView, matProjection;



      V(g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld));

      V(g_pd3dDevice->GetTransform(D3DTS_VIEW,	&matView));

      V(g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection));

      // I chaned this line, so you could change the projection view here, adjusting

      // the near and far plane...alternatively you can just use the GetTransform

      // DX call, and use the pre-set one.

      //D3DXFROMWINEMatrixPerspectiveFovLH( &matProjection, D3DXFROMWINE_PI/2, 1.0f, 1.0f, 30.0f );



      D3DXFROMWINEMATRIX Cull = matWorld * matView * matProjection;



      D3DXFROMWINEVECTOR4 vNode = D3DXFROMWINEVECTOR4(x,y,z,1);



      D3DXFROMWINEVECTOR4 vTestVert;

      D3DXFROMWINEVec4Transform(&vTestVert, &vNode, &Cull);



      if( abs(vTestVert.x) > abs(vTestVert.w) ) // Outside our X Frustum Plane

	    return false;



      if( abs(vTestVert.y) > abs(vTestVert.w) ) // Outside our Y Frustum Plane

	    return false;



      if( (vTestVert.z<0.0f) || (vTestVert.z>vTestVert.w)) // Outside our z Frustum Plane

	    return false;



      return true;

}

#endif
void PrintSpherical(float varphi,float theta)
{
	D3DXFROMWINEVECTOR3 position=GetPosition3(varphi,theta,1000.0f);
  D3DXFROMWINEVECTOR3 screenposition;
  DWORD stride=sizeof(D3DXFROMWINEVECTOR3);
	D3DVIEWPORT9 viewport = { 0, 0, width, height, 0, 1 };
   D3DXFROMWINEMATRIX matProj, matView, matWorld;
   matProj=g_Camera.GetProjMatrix();
   matView=g_Camera.GetViewMatrix();
   D3DXFROMWINEMatrixIdentity(&matWorld);
	D3DXFROMWINEVec3Project(&screenposition, &position,
			    &viewport, &matProj, &matView, &matWorld
			    );

if(PointInFrustum(position.x, position.y, position.z))
		 {
	  RECT destRect;
		 HRESULT hr;
		 char buf[256];
		 sprintf(buf, "(%3.0f,%3.0f)",varphi,theta);
	  SetRect( &destRect, screenposition.x, screenposition.y, 0, 0 );
		 hr=g_pD3DXFROMWINESmallFont->DrawText( NULL,buf, -1, &destRect, DT_NOCLIP,D3DXFROMWINECOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
}
}
#if 0
void DrawLines()
{
	line_vertex line[2];
	line[0].colour=0xFFFFFFFF;
	line[1].colour=0xFFFFFFFF;
	V(g_pd3dDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE));
	line[0].pos=GetPosition4(34.670,12.571);
	line[1].pos=GetPosition4(38.500,13.851);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(38.450,11.901);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(38.370,10.701);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(38.330,10.341);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(38.270,10.791);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(41.460,12.741);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition4(38.370,10.701);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=GetPosition4(38.330,10.341);
	line[1].pos=GetPosition4(34.520,21.401);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(25.200,26.311);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(25.200,26.341);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));

	line[0].pos=GetPosition(34.520,21.401);
	line[1].pos=GetPosition(30.120,25.191);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(30.040,14.181);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(30.070,12.861);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));

	line[0].pos=GetPosition(41.460,12.741);
	line[1].pos=GetPosition(41.390,13.041);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(41.270,24.311);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(38.200,15.221);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(38.270,10.791);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));

	line[0].pos=GetPosition(38.200,15.221);
	line[1].pos=GetPosition(34.320,14.511);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(34.200,21.491);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));

	line[0].pos=GetPosition(34.320,14.511);
	line[1].pos=GetPosition(34.240,16.261);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));


/*
; Ursa Minor

4628002371 0	    move to alpha
4655010531 1	    draw to delta
4580011541 1	    draw to epsilon
4563027081 1	    draw to zeta
4416017991 1	    draw to beta
4414023141 1	    draw to gamma
4567017641 1	    draw to eta
4563027081 1	    draw to zeta
0000000000 2	    end
*/
/*
424
6789
6322
5903
*/
	line[0].pos=GetPosition(46.280,02.371);
	line[1].pos=GetPosition(46.550,10.531);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(45.800,11.541);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(45.630,27.081);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(44.160,17.991);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(44.140,23.141);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(45.670,17.641);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(45.630,27.081);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));


	line[0].pos=GetPosition(90.000,15.000);
	line[1].pos=GetPosition(105.000,15.000);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(105.000,-15.000);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(90.000,-15.000);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));
	line[0].pos=line[1].pos;
	line[1].pos=GetPosition(90.000,15.000);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex));



	V(g_pd3dDevice->SetFVF(0);
}
#endif
struct BorderVertex
{
	int h,m;
	float s,dec;
};
#if 1
BorderVertex UMI[22]={
{13, 03, 16.9470, 76.3289108},
{13, 04, 23.3937, 69.3293610},
{14, 02, 36.1947, 69.3991165},
{14, 03, 16.9333, 65.3996506},
{15, 41, 19.0957, 65.6023483},
{15, 40, 12.1512, 69.6009445},
{16, 31, 21.8550, 69.7383041},
{16, 28, 52.9698, 74.7347870},
{17, 26, 08.7929, 74.9033127},
{17, 20, 52.2971, 79.8953476},
{17, 50, 37.4449, 79.9857483},
{17, 26, 53.3353, 85.9495697},
{20, 34, 53.0328, 86.4656219},
{20, 33, 19.5253, 86.6306305},
{22, 54, 02.5599, 86.8368912},
{22, 37, 02.6371, 88.6638870},
{9, 3, 19.7931, 87.5689163},
{8, 41, 36.6601, 86.0975418},
{14, 12, 5.5098, 85.9308090},
{14, 27, 7.8855, 79.4449844},
{13, 35, 14.2055, 79.3629303},
{13, 36, 37.6845, 76.3638153},
};
#endif
#if 1
BorderVertex UMA[32]={
{9, 42, 50.2171, 41.4316750},
{9, 18, 2.9977, 41.4785957},
{9, 18, 21.7707, 46.4782791},
{8, 33, 45.6249, 46.5777283},
{8, 35, 11.7921, 59.5759888},
{8, 8, 30.9843, 59.6433983},
{8, 12, 20.6949, 73.1383743},
{9, 22, 27.7137, 72.9741364},
{11, 27, 50.7287, 72.8125000},
{11, 27, 23.8431, 65.8126068},
{12, 06, 19.0213, 65.8039627},
{12, 06, 19.5743, 63.3039627},
{13, 34, 12.1293, 63.3593445},
{13, 34, 17.6739, 62.3593979},
{14, 28, 10.8609, 62.4414825},
{14, 29, 00.2997, 54.9422379},
{14, 06, 20.2539, 54.9035759},
{14, 06, 47.6957, 47.9039383},
{13, 35, 10.8273, 47.8599281},
{13, 34, 58.1757, 52.3598061},
{12, 11, 16.4454, 52.3043365},
{12, 11, 18.3441, 44.3043365},
{12, 06, 21.9399, 44.3039627},
{12, 06, 22.6815, 33.3039627},
{12, 06, 22.9593, 28.3039627},
{11, 58, 26.1459, 28.3040466},
{11, 06, 46.5595, 28.3250256},
{11, 06, 51.4141, 33.3249931},
{10, 53, 57.4581, 33.3356781},
{10, 54, 05.5605, 39.3356133},
{10, 17, 26.2590, 39.3774109},
{10, 17, 30.7737, 41.3773613},
};
#endif
struct BorderVertex2
{
	float ra,dec;
};
BorderVertex2 constellationBorders[1566];
//BorderVertex* constellationBorderStart[constellationnum];
int constellationBorderNum[constellationnum];
void LoadBorders()
{
//09 27 37.0404|-24.5425186|ANT
	int c=0;
	int constellationID=0;
	int borderNum=0;
	for(unsigned int i=0;i<constellationnum;i++)
		     constellationpositions[i]=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
	char str[2000];
	fstream file_op("IAUConstellationBorders.txt",ios::in);
	while(!file_op.eof())
	{
	      file_op.getline(str,2000);

			if(strcmp(";",str)==0)
			{
				constellationBorderNum[constellationID]=borderNum;
				constellationpositions[constellationID++]/=borderNum;





									/*
				//for(unsigned int i=0;i<constellationnum;i++)
				//{
				 constellationpositions[constellationID]=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
				 for(unsigned int j=0;j<borderNum;j++)
				{
				  D3DXFROMWINEVECTOR3 position=GetPosition3(varphi,theta,5.0f);
				  constellationpositions[constellationID]+=position;
				}
				//}

									  */



				//constellationID++;
				borderNum=0;
				continue;
			}
			  char buf[12];
			  unsigned int equinox=0;
			  buf[0]=str[equinox];
			  buf[1]=str[equinox+1];
			  buf[2]='\0';
			  int rah;
			  int r=sscanf (buf,"%d",&rah);
			  if(r!=1)continue;//LOG_PRINT("h r!=1:%d\n",c);
			  buf[0]=str[equinox+3];
			  buf[1]=str[equinox+4];
			  buf[2]='\0';
			  int ram;
			  r=sscanf (buf,"%d",&ram);
			  if(r!=1)continue;//LOG_PRINT("m r!=1:%d\n",c);
			  buf[0]=str[equinox+6];
			  buf[1]=str[equinox+7];
			  buf[2]=str[equinox+8];
			  buf[3]=str[equinox+9];
			  buf[4]=str[equinox+10];
			  buf[5]=str[equinox+11];
			  buf[6]=str[equinox+12];
			  buf[7]='\0';
			  float ras;
			  r=sscanf (buf,"%f",&ras);
			  if(r!=1)continue;//LOG_PRINT("s r!=1:%d\n",c);
			  float x=(((ras/60.0f)+float(ram))/60.0f+float(rah))*15.0f;

			  buf[0]=str[equinox+14];
			  buf[1]=str[equinox+15];
			  buf[2]=str[equinox+16];
			  buf[3]=str[equinox+17];
			  buf[4]=str[equinox+18];
			  buf[5]=str[equinox+19];

			  buf[6]=str[equinox+20];
			  buf[7]=str[equinox+21];
			  buf[8]=str[equinox+22];
			  buf[9]=str[equinox+23];

			  buf[10]=str[equinox+24];
			  buf[11]='\0';
			  float dec;
			  r=sscanf (buf,"%f",&dec);
			  if(r!=1)continue;//LOG_PRINT("s r!=1:%d\n",c);
			  float y=dec;

			  constellationBorders[c].ra=x;
			  constellationBorders[c].dec=y;

			  D3DXFROMWINEVECTOR3 position=GetPosition3(x,y,5.0f);
			  constellationpositions[constellationID]+=position;


	      c++;
	      borderNum++;
	}
	file_op.close();
}

void LoadFigures()
{
/*
/Constellations/ConstellationFigures/dhmenz11/DHMENZEL.TXT
0 8960
1 8975
;
*/
	int c=0;
	int constellationID=0;
	int figureNum=0;
	int figureStart=0;
	char str[2000];
	fstream file_op("ConstellationFigures.txt",ios::in);
	while(!file_op.eof())
	{
	      file_op.getline(str,2000);

			if(str[0]=='/')
			{
				continue;
			}
			if(str[0]==';')
			{
				constellationFigureNum2[constellationID]=figureNum;
				constellationFigureStart[constellationID]=figureStart;
				figureStart+=figureNum;
				figureNum=0;
				constellationID++;
				continue;
			}
			  char buf[15];
			  buf[0]=str[0];
			  buf[1]='\0';
			  int cmd;
			  int r=sscanf (buf,"%d",&cmd);
			  if(r!=1)
			  {
			    LOG_PRINT("x:r=-1\n");
			    continue;
			  }
			  int x=cmd;

			  buf[0]=str[2];
			  buf[1]=str[3];
			  buf[2]=str[4];
			  buf[3]=str[5];
			  buf[4]='\0';
			  int id;
			  r=sscanf (buf,"%d",&id);
			  if(r!=1)
			  {
			    LOG_PRINT("y:r=-1\n");
			    continue;
			  }
			  int y=id;

			  constellationFigures[c].cmd=x;
			  constellationFigures[c].id=y;

			  if(c<3)
			  {
			    LOG_PRINT("%s\n",str);
			  }

	      c++;
	      figureNum++;
	}
	file_op.close();
}

D3DXFROMWINEVECTOR4 GetPositionBV4(BorderVertex& bv)
{
	float ra=((bv.s/60.0f+bv.m)/60.0f+bv.h)*15.0f;
	float dec=bv.dec;
	return GetPosition(ra,dec);
}
D3DXFROMWINEVECTOR3 GetPositionBV2(BorderVertex2& bv)
{
	float ra=bv.ra;
	float dec=bv.dec;
	return GetPosition3(ra,dec,5.0f);
}
D3DXFROMWINEVECTOR3 GetPositionBV(BorderVertex& bv)
{
	float ra=((bv.s/60.0f+bv.m)/60.0f+bv.h)*15.0f;
	float dec=bv.dec;
	return GetPosition3(ra,dec,5.0f);
}
void DrawBorder4(BorderVertex* bv,int n)
{
	line_vertex4 line[2];
	line[0].colour=0xFFFFFFFF;
	line[1].colour=0xFFFFFFFF;
	V(g_pd3dDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE));
	line[0].pos=GetPositionBV4(bv[0]);
	line[1].pos=GetPositionBV4(bv[1]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex4)));
	for(unsigned int i=2;i<n;i++)
	{
	line[0].pos=line[1].pos;
	line[1].pos=GetPositionBV4(bv[i]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex4)));
	}
	line[0].pos=GetPositionBV4(bv[n-1]);
	line[1].pos=GetPositionBV4(bv[0]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex4)));

	V(g_pd3dDevice->SetFVF(0));

}
void DrawBorder(BorderVertex* bv,int n)
{
	line_vertex line[2];
	line[0].colour=0xFFFFFFFF;
	line[1].colour=0xFFFFFFFF;
	V(g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE));

	   D3DXFROMWINEMATRIX matProj, matView, matWorld;
	   matProj=g_Camera.GetProjMatrix();
	   matView=g_Camera.GetViewMatrix();
	   D3DXFROMWINEMatrixIdentity(&matWorld);

	V(g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj));
	V(g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView));
	V(g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld));

	line[0].pos=GetPositionBV(bv[0]);
	line[1].pos=GetPositionBV(bv[1]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
	for(unsigned int i=2;i<n;i++)
	{
	line[0].pos=line[1].pos;
	line[1].pos=GetPositionBV(bv[i]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
	}
	line[0].pos=GetPositionBV(bv[n-1]);
	line[1].pos=GetPositionBV(bv[0]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));

	V(g_pd3dDevice->SetFVF(0));

}
void DrawBorders()
{
	line_vertex line[2];
	line[0].colour=0xFFFFFFFF;
	line[1].colour=0xFFFFFFFF;
	V(g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE));
	   D3DXFROMWINEMATRIX matProj, matView, matWorld;
	   matProj=g_Camera.GetProjMatrix();
	   matView=g_Camera.GetViewMatrix();
	   D3DXFROMWINEMatrixIdentity(&matWorld);
	V(g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj));
	V(g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView));
	V(g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld));

unsigned int p=0;
unsigned int c=0;
for(unsigned int j=0;j<constellationnum;j++)
{
	line[0].pos=GetPositionBV2(constellationBorders[p+0]);
	line[1].pos=GetPositionBV2(constellationBorders[p+1]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
	for(unsigned int i=2;i<constellationBorderNum[c];i++)
	{
	line[0].pos=line[1].pos;
	line[1].pos=GetPositionBV2(constellationBorders[p+i]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
	}
	line[0].pos=GetPositionBV2(constellationBorders[p+constellationBorderNum[c]-1]);
	line[1].pos=GetPositionBV2(constellationBorders[p+0]);
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
	p+=constellationBorderNum[c];
	c++;
}
	V(g_pd3dDevice->SetFVF(0));

}
void DrawFigures()
{
	line_vertex line[2];
	line[0].colour=0xFFFFFFFF;
	line[1].colour=0xFFFFFFFF;
	V(g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE));
	   D3DXFROMWINEMATRIX matProj, matView, matWorld;
	   matProj=g_Camera.GetProjMatrix();
	   matView=g_Camera.GetViewMatrix();
	   D3DXFROMWINEMatrixIdentity(&matWorld);
	V(g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj));
	V(g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView));
	V(g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld));
/*
	LOG_PRINT("fid=%d\n",figureFileID);
	LOG_PRINT("fs =%d\n",constellationFigureStart[figureFileID]);
*/

	int cmd0=constellationFigures[constellationFigureStart[figureFileID]+0].cmd;
	int id0=constellationFigures[constellationFigureStart[figureFileID]+0].id;
	//id0++;
	line[0].pos=GetPosition3(cParticleEmitter::catalog[id0].ra,cParticleEmitter::catalog[id0].dec,5.0f);
/*
	LOG_PRINT("name0   =%s\n",cParticleEmitter::catalog[id0].name);
	LOG_PRINT("varname0=%s\n",cParticleEmitter::catalog[id0].varname);
	LOG_PRINT("id0=%d\n",id0);
	LOG_PRINT("cmd0=%d\n",cmd0);
	LOG_PRINT("ra0=%f\n",cParticleEmitter::catalog[id0].ra);
	LOG_PRINT("dec0=%f\n",cParticleEmitter::catalog[id0].dec);
	LOG_PRINT("pos0.x=%f\n",line[0].pos.x);
	LOG_PRINT("pos0.y=%f\n",line[0].pos.y);
	LOG_PRINT("pos0.z=%f\n",line[0].pos.z);

		//if(i<3)
		{
		     LOG_PRINT("id0=%d\n",id0);
		     LOG_PRINT("cmd0=%d\n",cmd0);
		     LOG_PRINT("name0	=%s\n",cParticleEmitter::catalog[id0].name);
		     LOG_PRINT("varname0=%s\n",cParticleEmitter::catalog[id0].varname);
		}      */
	if(cmd0==0)
	{
	for(unsigned int i=1;i<constellationFigureNum2[figureFileID];i++)
	{
		int cmd=constellationFigures[constellationFigureStart[figureFileID]+i].cmd;
		int id=constellationFigures[constellationFigureStart[figureFileID]+i].id;
		//id++;
		if(cmd==0)
		{
			//cmd0=cmd;
			//id0=id;
			line[0].pos=GetPosition3(cParticleEmitter::catalog[id].ra,cParticleEmitter::catalog[id].dec,5.0f);
/*
			LOG_PRINT("name0   =%s\n",cParticleEmitter::catalog[id].name);
			LOG_PRINT("varname0=%s\n",cParticleEmitter::catalog[id].varname);
			LOG_PRINT("id0=%d\n",id);
			LOG_PRINT("cmd0=%d\n",cmd);
			LOG_PRINT("ra0=%f\n",cParticleEmitter::catalog[id].ra);
			LOG_PRINT("dec0=%f\n",cParticleEmitter::catalog[id].dec);
			LOG_PRINT("pos0.x=%f\n",line[0].pos.x);
			LOG_PRINT("pos0.y=%f\n",line[0].pos.y);
			LOG_PRINT("pos0.z=%f\n",line[0].pos.z);
*/
		}
		else
		{
			line[1].pos=GetPosition3(cParticleEmitter::catalog[id].ra,cParticleEmitter::catalog[id].dec,5.0f);
/*
			LOG_PRINT("name1   =%s\n",cParticleEmitter::catalog[id].name);
			LOG_PRINT("varname1=%s\n",cParticleEmitter::catalog[id].varname);
			LOG_PRINT("id1=%d\n",id);
			LOG_PRINT("cmd1=%d\n",cmd);
			LOG_PRINT("ra1=%f\n",cParticleEmitter::catalog[id].ra);
			LOG_PRINT("dec1=%f\n",cParticleEmitter::catalog[id].dec);
			LOG_PRINT("pos1.x=%f\n",line[1].pos.x);
			LOG_PRINT("pos1.y=%f\n",line[1].pos.y);
			LOG_PRINT("pos1.z=%f\n",line[1].pos.z);
*/
			V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,line, sizeof(line_vertex)));
			//cmd0=cmd;
			//id0=id;
			line[0].pos=line[1].pos;
		}  /*
		if(i<3)
		{

		     LOG_PRINT("figureFileID=%d\n",figureFileID);
		     LOG_PRINT("constellationFigureStart[figureFileID]=%d\n",constellationFigureStart[figureFileID] );
		     LOG_PRINT("id1=%d\n",id);
		     LOG_PRINT("cmd1=%d\n",cmd);
		     LOG_PRINT("name1	=%s\n",cParticleEmitter::catalog[id].name);
		     LOG_PRINT("varname1=%s\n",cParticleEmitter::catalog[id].varname);
		}*/
	/*
	if(i==3)
	{
		LOG_PRINT("i==3\n");
		exit(-1);
	}*/
	}
	}
	//else
	//	LOG_PRINT();
	V(g_pd3dDevice->SetFVF(0));

}
//-----------------------------------------------------------------------------
// Name: render()
// Desc:
//-----------------------------------------------------------------------------
void render( )
{

	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1)) figureFileID=0;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2)) figureFileID=1;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F3)) figureFileID=2;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F4)) figureFileID=3;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F5)) figureFileID=4;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F6)) figureFileID=5;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F7)) figureFileID=6;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F8)) figureFileID=7;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F9)) figureFileID=8;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F10)) figureFileID=9;
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F11)) figureFileID=10;

	float f=0.0f;//128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	m_pSkyEmitter->render();
	//DrawYawPitch();
	DrawFPS();
    //V(g_pd3dDevice->EndScene();
    //V(g_pd3dDevice->BeginScene();
	static int bN=1;
	if(KeyPressed('N')) bN^=1;
	if(bN==1)
	{
  DWORD stride=sizeof(D3DXFROMWINEVECTOR3);
	D3DVIEWPORT9 viewport = { 0, 0, width, height, 0, 1 };
   D3DXFROMWINEMATRIX matProj, matView, matWorld;
   matProj=g_Camera.GetProjMatrix();
   matView=g_Camera.GetViewMatrix();
   D3DXFROMWINEMatrixIdentity(&matWorld);

	D3DXFROMWINEVec3ProjectArray(m_pSkyEmitter->screenpositions, stride,m_pSkyEmitter->solarsystempositions, stride,
			    &viewport, &matProj, &matView, &matWorld,
			    10);
	 for(unsigned int i=0;i<10;i++)
	 {
		 if(PointInFrustum(m_pSkyEmitter->solarsystempositions[i].x, m_pSkyEmitter->solarsystempositions[i].y, m_pSkyEmitter->solarsystempositions[i].z))
		 {
	  RECT destRect;
		 HRESULT hr;
		 char buf[256];
		 sprintf(buf, "%s", &names[i][0]);
	  SetRect( &destRect, m_pSkyEmitter->screenpositions[i].x, m_pSkyEmitter->screenpositions[i].y, 0, 0 );
		 hr=g_pD3DXFROMWINESmallFont->DrawText( NULL,buf, -1, &destRect, DT_NOCLIP,D3DXFROMWINECOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	 }
}


	D3DXFROMWINEVec3ProjectArray(constellationscreenpositions, stride,constellationpositions, stride,
			    &viewport, &matProj, &matView, &matWorld,
			    constellationnum);
	 for(unsigned int i=0;i<constellationnum;i++)
	 {
		if(PointInFrustum(constellationpositions[i].x, constellationpositions[i].y, constellationpositions[i].z))
		 {
	  RECT destRect;
		 HRESULT hr;
		 char buf[256];
		 sprintf(buf, "%s", &constellationnames[i][0]);
	  SetRect( &destRect, constellationscreenpositions[i].x, constellationscreenpositions[i].y, 0, 0 );
		 hr=g_pD3DXFROMWINESmallFont->DrawText( NULL,buf, -1, &destRect, DT_NOCLIP,D3DXFROMWINECOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	}
	 }
for(unsigned int i=0;i<24;i++)
{
for(int j=5;j>-6;j--)
{
  float ra=i*15.0f;//*D3DXFROMWINE_PI/180.0f;
  float dec=j*15.0f;//*D3DXFROMWINE_PI/180.0f;
PrintSpherical(ra,dec);
}
	 }
	}
/*
	//DrawLines();
	DrawBorder(UMI,22);
	DrawBorder(UMA,32);
*/
	static int bB=1;
	if(KeyPressed('B')) bB^=1;
	if(bB==1)
	{
	DrawBorders();
	}
	static int bF=1;
	if(KeyPressed('I')) bF^=1;
	if(bF==1)
	{
	#ifdef _DrawFigures_
	DrawFigures();
	#endif
		 char buf[256];
		 sprintf(buf, "FigureFile: %s", &constellationFigureFiles[figureFileID][0]);
	  	 IRenderer::GetRendererInstance()->drawText(buf, 0, 250,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
			}

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Stars");
	IInput*    i=STX_Service::GetInputInstance();
	init(640, 512);
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
/*
8960 74    PegBD+16 4954 222098108729	   W		    233235.7+161619233739.8+164932 98.66-42.54 6.26   0.00 +0.08	 A1V		    +0.134 0.000      -026SBO	 45  4.5  98.3AB   3*
8961 16Lam AndBD+45 4283 222107 53204 890I W	   Lam And  233240.0+455459233733.9+462729109.90-14.53 3.82  +1.01 +0.69 +0.57	 G8III-IV	   v+0.162-0.421 +.050+007SB1O < 19: 6.5 217.6AC   4*
8962	      BD+43 4508 222109 53202	    16877	    233238.6+435233233732.0+442545109.28-16.47 5.80  -0.06 -0.31	 B8V		    +0.015-0.010D+.004-011V	 50  0.9   0.5AB   3*
8963 75    PegBD+17 4952 2221331087323896  W	   KS Peg   233253.7+175048233756.8+182402 99.54-41.11 5.53   0.00 -0.03	 A1Vn		    +0.050+0.017      -016SB1	216  6.2  27.9	    *
8964	      BD+45 4288 222143 53210			    233302.4+453846233758.6+461159109.89-14.80 6.58  +0.66 +0.16	 G5		    +0.370-0.003 +.032-001
8965 17Iot AndBD+42 4720 222173 53216 891	   14646    233313.7+424252233808.2+431605109.03-17.62 4.29  -0.10 -0.29 -0.09	 B8V		    +0.029-0.001 +.011-001SB	 84
8966   The PheCD-4714651 222287231719	   W		    233405.9-471134233928.0-463816334.20-65.88 6.09  +0.24 +0.11	 A8V+F0V	    +0.028+0.033D+.008+014	     0.7   3.7	    *
8967 18    AndBD+49 4180 222304 356423897		    233417.4+495504233908.3+502818111.34-10.77 5.30  -0.06 -0.15	 B9V		    -0.017-0.002      +009SB	178
8968102Ome1AqrBD-15 6471 222345165818			    233435.9-144629233947.1-141318 67.04-68.89 5.00  +0.24		 F0IV		    +0.057-0.038 +.016-002SB	102
8969 17Iot PscBD+04 5035 222368128310 892I W	   14657    233448.3+050503233957.0+053735 92.46-52.96 4.13  +0.51  0.00 +0.31	 F7V		    +0.378-0.438 +.075+005V	  6  8.7  69.9	    *
8970	      BD+08 5095 222377128309			    233449.2+090724233955.1+094038 95.27-49.31 5.97  +0.20 +0.13 +0.09	 A2IVm		    +0.099-0.011      +000	 60
8971	      BD+74 1032 222386 10814			    233452.7+744419233910.4+751734118.30 13.08 5.95  +0.13 +0.12	 A3V		    +0.012+0.011      +003SB	125
8972	      BD+73 1047 222387 108173898		    233459.5+732655233921.1+740010117.93 11.83 5.98  +0.89		 G8III		    -0.012+0.004      +009SB
8973	      BD+36 5098 222399 73422	    16913	    233505.4+370603234002.8+373909107.64-23.09 6.53  +0.35 +0.08	 F2IV		    -0.013-0.084      -016V?	     4.4  14.9	    *
8974 35Gam CepBD+76  928 222404 10818 893I	   14656    233514.3+770427233920.8+773757118.99 15.31 3.21  +1.03 +0.94 +0.51	 K1III-IV	    -0.067+0.151 +.064-042V?   < 17		    *
8975   Mu  SclCD-3217621 2224332147011618	   14661    233523.4-323734234038.2-320423 10.86-73.86 5.31  +0.97 +0.66	 K1III		    -0.089-0.055 +.027+014
8976 19Kap AndBD+43 4522 222439 532641619   16916	    233528.8+434649234024.5+442002109.76-16.71 4.14  -0.08 -0.26 -0.07	 B9IVn		    +0.083-0.019 +.018-009V	184  7.6  46.8AB   3*

/Constellations/ConstellationFigures/dhmenz11/DHMENZEL.TXT
0 8960	3638025151 0	    move to lambda
1 8975	3244015301 1	    draw to kappa
1 8964	3243014121 1	    draw to iota
1 8761	3224034341 1	    draw to omicron
0 214	1739023171 0	    move to zeta

(4052): id0=8960
(4052): cmd0=0
(4052): name0	=   Mu	Scl
(4052): varname0=14661
(4052): figureFileID=0
(4052): constellationFigureStart[figureFileID]=0
(4052): id1=8975
(4052): cmd1=1
(4052): name1	=
(4052): varname1=
(4052): figureFileID=0
(4052): constellationFigureStart[figureFileID]=0
(4052): id1=8964
(4052): cmd1=1
(4052): name1	=
(4052): varname1=
(4052): i==3

   1	      BD+44 4550      3 36042	       46	    000001.1+444022000509.9+451345114.44-16.88 6.70  +0.07 +0.08	 A1Vn		    -0.012-0.018      -018	195  4.2  21.6AC   3
9110	      BD+60 2667 225289 10962		   V567 Cas 235955.9+604526000506.2+611851117.40-01.06 5.80  -0.09 -0.32	 B8IVpHgMn	    +0.015+0.005      +014V	 50		    *

ccc=9097
*/
