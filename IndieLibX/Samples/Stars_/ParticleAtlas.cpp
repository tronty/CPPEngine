/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "particleatlas.h"
//include "CMesh.h"
//include "LuaUtil.h"
//usinfg namespace LuaUtil;
#include <libnova/sidereal_time.h>
using namespace std;

__WORD__	    cParticleEmitter::m_RefCount=0;
ShaderID	   cParticleEmitter::m_pEffect=-1;
ShaderID cParticleEmitter::m_pShader=-1;
VertexFormatID cParticleEmitter::m_pDecl=-1;
bool cParticleEmitter::initVegetation=true;
bool cParticleEmitter::initVegetation2=true;
bool cParticleEmitter::initSky=true;
#ifdef __VERTEXCACHE__
CVertexCache* m_vertexCache=0;
#endif

extern D3DXFROMWINEVECTOR3 g_CameraPosition;
extern D3DXFROMWINEMATRIX g_CameraProjMatrix;
extern D3DXFROMWINEMATRIX g_CameraViewMatrix;
extern float g_CameraSpeed;
extern D3DXFROMWINEVECTOR3 vO;

inline __WORD__ FLOAT2DWORD(FLOAT f) { return *((__WORD__*)&f); }

char names[10][256]={
"Sun",
"Moon",
"Mercury",
"Venus",
"Mars",
"Jupiter",
"Saturn",
"Uranus",
"Neptunus",
"Pluto"
};

#define SKY_RADIUS 1.0f

extern D3DXFROMWINEVECTOR3 g_CameraPosition;
extern D3DXFROMWINEMATRIX I;
extern D3DXFROMWINEMATRIX g_CameraProjMatrix;
extern D3DXFROMWINEMATRIX g_CameraViewMatrix;
extern D3DXFROMWINEMATRIX mvp;
extern __DWORD__ stride;
extern D3DFROMWINEVIEWPORT9 viewport;
extern D3DXFROMWINEMATRIX matProj;
extern D3DXFROMWINEMATRIX matView;
extern D3DXFROMWINEMATRIX matWorld;

void cParticleEmitter::RenderStars()
{
	//LOG_FNLN;
	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();
	cParticle* particle=m_Particles->m_Next;
	while(particle)
	{
		 D3DXFROMWINEVECTOR3 p1=particle->m_vecPos;		
		D3DXFROMWINEVECTOR3 p2=CalculatePosition(p1.x, p1.y, JD);
	/*
	//LOG_FNLN;
	//LOG_PRINT("p1.x=%f\n", p1.x);
	//LOG_PRINT("p1.y=%f\n", p1.y);
	//LOG_PRINT("p2.x=%f\n", p2.x);
	//LOG_PRINT("p2.y=%f\n", p2.y);
	*/
		IRenderer::GetRendererInstance()->drawCircle(p2.x, p2.y, 1.1f, D3DXFROMWINEVECTOR4(1,1,1,1));

      		// Go to next particle
      		particle = particle->m_Next;
	}
}

void cParticleEmitter::applyGamma( Image3& image, float gamma )
		{
				ubyte *fDest  = image.getPixels();
					size_t size  = image.getWidth()*image.getHeight()*getBytesPerPixel(image.format);
					applyGamma( fDest, size, getBytesPerPixel(image.format), gamma );
		}

	void cParticleEmitter::applyGamma( unsigned char *buffer, size_t size, unsigned char bpp, float gamma )
	{
		bpp*=8;
		//LOG_PRINT("gamma gamma=%f\n",gamma);
		//LOG_PRINT("gamma bpp=%d\n",bpp);

		if( gamma == 1.0f )
			return;

		//NB only 24/32-bit supported
		if( bpp != 24 && bpp != 32 ) return;

		unsigned int stride = bpp >> 3;

		for( size_t i = 0, j = size / stride; i < j; i++, buffer += stride )
		{
			float r, g, b;

			r = (float)buffer[0];
			g = (float)buffer[1];
			b = (float)buffer[2];

			r = r * gamma;
			g = g * gamma;
			b = b * gamma;

			float scale = 1.0f, tmp;

			if( r > 255.0f && (tmp=(255.0f/r)) < scale )
				scale = tmp;
			if( g > 255.0f && (tmp=(255.0f/g)) < scale )
				scale = tmp;
			if( b > 255.0f && (tmp=(255.0f/b)) < scale )
				scale = tmp;

			r *= scale; g *= scale; b *= scale;

			buffer[0] = (unsigned char)r;
			buffer[1] = (unsigned char)g;
			buffer[2] = (unsigned char)b;
		}
	}

bool cParticleEmitter::Create(
			      D3DXFROMWINEVECTOR3 vecPosition,
			      __WORD__ EmitterType,
			      __WORD__ NumParticlesPerBuffer)
{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

   if(m_pDecl==0) initEffect();
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

if((EmitterType==EMITTER_VEGETATION)&&!initVegetation) return false;
	
if((EmitterType==EMITTER_ARBARO)&&!initVegetation2) return false;
	
if((EmitterType==EMITTER_SKY)&&!initSky) return false;
	

    //ObjectPos=new D3DXFROMWINEVECTOR3[NumParticlesPerBuffer];
    //ObjectPos=new D3DXFROMWINEVECTOR3[NumParticlesPerBuffer];
    //ObjectDes=new char[NumParticlesPerBuffer][512];
#ifdef __VERTEXCACHE__
	if(m_vertexCache==0)
	 m_vertexCache=new CVertexCache(g_pd3dDevice->m_caps.MaxVertexIndex,g_pd3dDevice->m_caps.MaxPrimitiveCount*3,sizeof(sShaderVertex),0,0);
#endif
	
  // Save emitter's position
  m_vecPosition = vecPosition;

  // Save type of emitter
  m_EmitterType = EmitterType;

  // Save # of particles in buffer
  if(EmitterType==EMITTER_SKY)
       m_NumParticles = 9107;
  else
      m_NumParticles = NumParticlesPerBuffer;
	

  // Create the vertex buffer
	m_VB.resize(m_NumParticles * 4);
	m_IB.resize(m_NumParticles * 6);
	

#ifdef __VERTEXCACHE__
pVertices=&m_VB[0];
	pIndices=&m_IB[0];
#endif
	
  // Create the index buffer and fill with face data

  for(__WORD__ i=0;i<m_NumParticles;i++) {
    m_IB[i*6+0] = i * 4 + 0;
    m_IB[i*6+1] = i * 4 + 1;
    m_IB[i*6+2] = i * 4 + 2;
    m_IB[i*6+3] = i * 4 + 3;
    m_IB[i*6+4] = i * 4 + 2;
    m_IB[i*6+5] = i * 4 + 1;
#ifdef __VERTEXCACHE__
    pIndices[i*6+0] = i * 4 + 0;
    pIndices[i*6+1] = i * 4 + 1;
    pIndices[i*6+2] = i * 4 + 2;
    pIndices[i*6+3] = i * 4 + 3;
    pIndices[i*6+4] = i * 4 + 2;
    pIndices[i*6+5] = i * 4 + 1;
#endif
  } 
	
	m_IB.resize(6*m_NumParticles);
	

//  int hr;

//}

   atlasTextureCount=0;
#if 0
   if(0)
   {
	iatlassize=768;
	isubtexturesize=256;

 ubyte *fDest = atlas0.create(FORMAT_RGBA8, iatlassize, iatlassize, 1, 1);

 Image3 tree4,tree1,tree2,tree3,grass1,grass2,grass3,grass4,grass5;
 //tree4.loadDDS("Media\\Textures\\Particles\\Particle_Palm.jpg");
 tree4.loadDDS("Media\\Textures\\Particles\\Particle_Tree2.dds");
 tree1.loadDDS("Media\\Textures\\Particles\\Particle_Tree1.dds");
 tree2.loadDDS("Media\\Textures\\Particles\\Particle_Tree2.dds");
 tree3.loadDDS("Media\\Textures\\Particles\\Particle_Tree3.dds");
 grass1.loadDDS("Media\\Textures\\Particles\\Particle_grass001.dds");
 grass2.loadDDS("Media\\Textures\\Particles\\Particle_grass002.dds");
 grass3.loadDDS("Media\\Textures\\Particles\\Particle_grass003.dds");
 grass4.loadDDS("Media\\Textures\\Particles\\Particle_grass001.dds");
 grass5.loadDDS("Media\\Textures\\Particles\\Particle_grass003.dds");

 stx_AddTextureToAtlas(atlas0,grass1);
 stx_AddTextureToAtlas(atlas0,grass4);

  float atlasgamma=1.00;
 float tree1gamma=1.00;
 float tree2gamma=1.00;
 float tree3gamma=1.00;
 float tree4gamma=1.00;
 float grass2gamma=1.00;
 float grass3gamma=1.00;
 float grass5gamma=1.00;
/*
 LuaObject tmp = SLua::state->GetGlobal("atlasgamma");
 if(tmp.IsNumber())
      atlasgamma=tmp.GetNumber();
 tmp = SLua::state->GetGlobal("tree1gamma");
 if(tmp.IsNumber())
      tree1gamma=tmp.GetNumber();
 tmp = SLua::state->GetGlobal("tree2gamma");
 if(tmp.IsNumber())
      tree2gamma=tmp.GetNumber();
 tmp = SLua::state->GetGlobal("tree3gamma");
 if(tmp.IsNumber())
      tree3gamma=tmp.GetNumber();
 tmp = SLua::state->GetGlobal("tree4gamma");
 if(tmp.IsNumber())
      tree4gamma=tmp.GetNumber();
 tmp = SLua::state->GetGlobal("grass2gamma");
 if(tmp.IsNumber())
      grass2gamma=tmp.GetNumber();
  tmp = SLua::state->GetGlobal("grass3gamma");
 if(tmp.IsNumber())
      grass3gamma=tmp.GetNumber();
  tmp = SLua::state->GetGlobal("grass5gamma");
 if(tmp.IsNumber())
      grass5gamma=tmp.GetNumber();
*/
 stx_AddTextureToAtlas(atlas0,tree4, tree1gamma );
  stx_AddTextureToAtlas(atlas0,tree1, tree2gamma );
  stx_AddTextureToAtlas(atlas0,tree2, tree3gamma );
  stx_AddTextureToAtlas(atlas0,tree3, tree4gamma );
  stx_AddTextureToAtlas(atlas0,grass2, grass2gamma );
  stx_AddTextureToAtlas(atlas0,grass3, grass3gamma );
  stx_AddTextureToAtlas(atlas0,grass5, grass5gamma );

  applyGamma( atlas0, atlasgamma );

  atlas0.saveDDS("Atlas.dds");

     D3DXFROMWINECreateTextureFromFileExA(
				"Atlas.dds",
				D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				/*D3DPOOL_MANAGED*/D3DPOOL_DEFAULT,
				D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), 0, 0,
				&m_pAtlasTexture);
	start=PARTICLE_TREE1;
		initVegetation=false;
   }
   else if(0)
   {
	iatlassize=4096;
	isubtexturesize=256;
	const int NUMTREETEXTURES=2;//16
char* g_strTreeModels[] =
{
	"Media\\TreeDefinitions\\wheat.x",
	"Media\\TreeDefinitions\\barley.x",
	"Media\\TreeDefinitions\\black_tupelo.x",
	"Media\\TreeDefinitions\\ca_black_oak.x",
	"Media\\TreeDefinitions\\desert_bush.x",
	"Media\\TreeDefinitions\\eastern_cottonwood.x",
	"Media\\TreeDefinitions\\european_larch.x",
	"Media\\TreeDefinitions\\fanpalm.x",
	"Media\\TreeDefinitions\\lombardy_poplar.x",
	"Media\\TreeDefinitions\\palm.x",
	"Media\\TreeDefinitions\\quaking_aspen.x",
	"Media\\TreeDefinitions\\rush.x",
	"Media\\TreeDefinitions\\sassafras.x",
	"Media\\TreeDefinitions\\shave-grass.x",
	"Media\\TreeDefinitions\\tamarack.x",
	"Media\\TreeDefinitions\\weeping_willow.x",
	//"Media\\TreeDefinitions\\test\\bottle_brush.x",
	//"Media\\TreeDefinitions\\test\\feather.x",
};

Image3 img,atlas0;
ubyte *fDest = atlas0.create(FORMAT_RGBA8, iatlassize, iatlassize, 1, 1);
for( __WORD__ i=0; i<NUMTREETEXTURES; i++ )
{
	CMesh mesh(g_strTreeModels[i]);
	mesh.bImpostor=1;
#if 1
	float SCALECOFFICIENT=1.73f;//???
	float g_fZ0=2.0f;
	D3DXFROMWINEVECTOR3 _min2, _max2, _center2;
	float _radius2;
	BYTE * vv = 0;
	__WORD__ nVert=mesh.m_pMesh->GetNumVertices();
	mesh.m_pVertexBuffer4->Lock( 0, nVert*mesh.sizeofvertex2, (void**)&vv, 0 );
	D3DXFROMWINEComputeBoundingBox(
			       (D3DXFROMWINEVECTOR3*)vv,
			       nVert,
			       mesh.sizeofvertex2,
			       &_min2, &_max2);
      D3DXFROMWINEComputeBoundingSphere(
		(D3DXFROMWINEVECTOR3*)vv,
		nVert,
		mesh.sizeofvertex2,
		 &_center2,
		 &_radius2);
	  mesh.m_pVertexBuffer4->Unlock();
	float g_scale = SCALECOFFICIENT / _radius2;

	D3DXFROMWINEVECTOR3 pos(0.0f,0.0f,g_fZ0);//???
#else
	D3DXFROMWINEVECTOR3 pos(0.0f,0.0f,1000.0f);//???
	float g_scale = 1.0f;
#endif
	D3DXFROMWINEVECTOR3 curCameraPos(0.0f,0.0f,0.0f);

	for( __WORD__ j=0; j<16; j++ )
	{
	     float angle=22.5f*j*D3DXFROMWINE_PI/180.0f;
	     float yaw=0.0f;
	     float pitch=angle;//???
	     mesh.rendertobillboard( curCameraPos,pos,yaw,pitch,0.0f,g_scale,g_scale,g_scale);
	     Image3 img;
	     img.loadDDS("Impostor.dds");
	     stx_AddTextureToAtlas(atlas0,img);
	}
//atlas0.saveDDS("VegetationAtlas.dds");
}
atlas0.saveDDS("VegetationAtlas.dds");

     D3DXFROMWINECreateTextureFromFileExA(
				"VegetationAtlas.dds",
				D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), 0, 0,
				&m_pAtlasTexture);
	start=PARTICLE_ARBARO1;
		initVegetation2=false;
   }
#endif
   //else if((EmitterType==EMITTER_SKY)&&initSky)
   {
	
	 iatlassize=1024;
	isubtexturesize=256;
	


#if 1
 ubyte *fDest = atlas0.create(FORMAT_RGBA8, iatlassize, iatlassize, 1, 1);
	

 Image3 sun,moon,star,mercury,venus,mars,jupiter,saturn,uranus,neptune,pluto;

#if 1
 sun.loadImageLibImage("/Stars/SolarSystemDisks/Sun.jpg");
	
 moon.loadImageLibImage("/Stars/SolarSystemDisks/Moon.jpg");
	
 star.loadImageLibImage("/Stars/SolarSystemDisks/Star.jpg");
	
 mercury.loadImageLibImage("/Stars/SolarSystemDisks/mercury_mariner10.jpg");
	
 venus.loadImageLibImage("/Stars/SolarSystemDisks/venus_galileo.jpg");
	
 mars.loadImageLibImage("/Stars/SolarSystemDisks/mars_mgs.jpg");
	
 jupiter.loadImageLibImage("/Stars/SolarSystemDisks/jupiter_cassini.jpg");
	
 saturn.loadImageLibImage("/Stars/SolarSystemDisks/saturn_cassini.jpg");
	
 uranus.loadImageLibImage("/Stars/SolarSystemDisks/uranus_voyager.jpg");
	
 neptune.loadImageLibImage("/Stars/SolarSystemDisks/neptune_voyager.jpg");
#if 1	
 pluto.loadImageLibImage("/Stars/SolarSystemDisks/pluto_HST.jpg");
#else
 pluto.loadImageLibImage("/Stars/SolarSystemDisks/pluto-infrared.jpg.optimal.jpg");
#endif
#else
sun.loadImageLibImage("/Stars/SolarSystemDisks/sun.jpg");
 moon.loadImageLibImage("/Stars/SolarSystemDisks/moon.jpg");
 star.loadImageLibImage("/Stars/SolarSystemDisks/star.jpg");
 mercury.loadImageLibImage("/Stars/SolarSystemDisks/mercury.jpg");
 venus.loadImageLibImage("/Stars/SolarSystemDisks/venus.jpg");
 mars.loadImageLibImage("/Stars/SolarSystemDisks/mars.jpg");
 jupiter.loadImageLibImage("/Stars/SolarSystemDisks/jupiter.jpg");
 saturn.loadImageLibImage("/Stars/SolarSystemDisks/saturn.jpg");
 uranus.loadImageLibImage("/Stars/SolarSystemDisks/uranus.jpg");
 neptune.loadImageLibImage("/Stars/SolarSystemDisks/neptune.jpg");
 pluto.loadImageLibImage("/Stars/SolarSystemDisks/pluto.jpg");
 #endif

#if 0
 sun.convert(FORMAT_RGBA8);
	
 moon.convert(FORMAT_RGBA8);
	
 star.convert(FORMAT_RGBA8);
	
 mercury.convert(FORMAT_RGBA8);
	
 venus.convert(FORMAT_RGBA8);
	
 mars.convert(FORMAT_RGBA8);
	
 jupiter.convert(FORMAT_RGBA8);
	
 saturn.convert(FORMAT_RGBA8);
	
 uranus.convert(FORMAT_RGBA8);
	
 neptune.convert(FORMAT_RGBA8);
	
 pluto.convert(FORMAT_RGBA8);
#endif	

 stx_AddTextureToAtlas(atlas0,sun);
	
 stx_AddTextureToAtlas(atlas0,moon);
	
 stx_AddTextureToAtlas(atlas0,star);
	
 stx_AddTextureToAtlas(atlas0,mercury);
	
 stx_AddTextureToAtlas(atlas0,venus);
	
 stx_AddTextureToAtlas(atlas0,mars);
	
 stx_AddTextureToAtlas(atlas0,jupiter);
	
 stx_AddTextureToAtlas(atlas0,saturn);
	
 stx_AddTextureToAtlas(atlas0,uranus);
	
 stx_AddTextureToAtlas(atlas0,neptune);
	
 stx_AddTextureToAtlas(atlas0,pluto);
	
 stx_AddTextureToAtlas(atlas0,sun);
	
 stx_AddTextureToAtlas(atlas0,sun);
	
 stx_AddTextureToAtlas(atlas0,sun);
	
 stx_AddTextureToAtlas(atlas0,sun);
	
 stx_AddTextureToAtlas(atlas0,sun);

 SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
#if 1
   atlas0.saveImage("/Stars/SkyAtlas.dds");
	m_pAtlasTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/Stars/SkyAtlas.dds", false, ss);
#else
	m_pAtlasTexture=IRenderer::GetRendererInstance()->addTexture(&atlas0,false, ss);
#endif	

 #endif
	STX_PRINT("488\n");

	start=PARTICLE_SUN;
	 initSky=false;
	
   }
  // Increase class reference count
  m_RefCount++;
	       //ZZZ
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

  // Return success
  return true;
}

int cParticleEmitter::initEffect()
{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	int hr=true;

const char* shdTxt="[Vertex shader]\n"
"struct vertex\n"
"{\n"
"	float3 position : POSITION;\n"
"	float4 diffuse	: COLOR0;\n"
"	float2 texcoord : TEXCOORD0;\n"
"	float2 offset	: TEXCOORD1;\n"
"	float4 allowedRotDir : TEXCOORD2;\n"
"};\n"
"\n"
"struct fragment\n"
"{\n"
"	float4 position : POSITION;\n"
"	float4 color0	: COLOR0;\n"
"	float2 texcoord : TEXCOORD0;\n"
"};\n"
"uniform float4x4 viewProj;\n"
"uniform float4 right;\n"
"uniform float4 up;\n"
"uniform float4 xCamPos;\n"
"fragment main( vertex IN)\n"
"{\n"
"	fragment Out=(fragment)0;\n"
"	float4 v = float4( IN.position.x,\n"
"			       IN.position.y,\n"
"					   IN.position.z,\n"
"					   1.0f );\n"
"	float4 r1;\n"
"	if(IN.allowedRotDir.w==0)\n"
"	{\n"
"	r1=IN.offset.y*up+IN.offset.x*right+v;\n"	
"	}\n"
"	else\n"
"	{\n"
"    float3 center = v.xyz;//mul(inPos, xWorld);\n"
"    float3 Forward=normalize(xCamPos.xyz-center);\n"
"	float3 upVector = -IN.allowedRotDir.xyz;\n"
"	float3 sideVector=normalize(cross(upVector,Forward));\n"
"	upVector=normalize(cross(sideVector,Forward));\n"
"    float3 finalPosition = center;\n"
"	finalPosition+=IN.offset.x*sideVector;\n"
"	finalPosition+=IN.offset.y*upVector;\n"
"    r1 = float4(finalPosition, 1);\n"
"	}\n"
"	OUT.position = mul( viewProj, r1 );\n"
"	OUT.color0 = IN.diffuse;\n"
"	OUT.texcoord = IN.texcoord;\n"
"	return OUT;\n"
"}\n"
"[Fragment shader]\n"
"struct fragment\n"
"{\n"
"	float4 position : POSITION;\n"
"	float4 color0	: COLOR0;\n"
"	float2 texcoord : TEXCOORD0;\n"
"};\n"
"sampler2D Sampler;\n"
"float4 brightness=float4(1,1,1,1);\n"
"float4 main( fragment IN )\n"
"{\n"
"	float4 OUT=(float4)0;\n"
"	OUT.color = tex2D( Sampler/*Sampler*/, IN.texcoord );\n"		
"	return OUT;\n"
"}\n";

  	m_pEffect = IRenderer::GetRendererInstance()->addShader(shdTxt, "main", "main");

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	m_pDecl = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_pEffect);

	return hr;
}

void cParticleEmitter::init()
  {

	return;
    //ZZZ
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	cParticle *Particle = m_Particles;


    // Set a count for flushing buffer
    __WORD__ Num = 0;

    // Lock the vertex buffer for use
    sShaderVertex *Ptr;

#ifdef __VERTEXCACHE__
    sShaderVertex *pVertices2=pVertices;
#else
	sShaderVertex pVertices2[4];
#endif

	D3DXFROMWINEVECTOR4 DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);


    // Loop for all particles
    while(Particle != 0) {

	// Copy over particle data
	float HalfSize = Particle->m_Size * 0.5f;

	pVertices2[0].vecPos	 = Particle->m_vecPos;
	pVertices2[0].vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, HalfSize);
	pVertices2[1].vecPos	 = Particle->m_vecPos;
	pVertices2[1].vecOffset = D3DXFROMWINEVECTOR2(HalfSize, HalfSize);
	pVertices2[2].vecPos	 = Particle->m_vecPos;
	pVertices2[2].vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize);
	pVertices2[3].vecPos	 = Particle->m_vecPos;
	pVertices2[3].vecOffset = D3DXFROMWINEVECTOR2(HalfSize, -HalfSize);

	pVertices2[0].Diffuse	 = DiffuseColor;
	pVertices2[1].Diffuse	 = DiffuseColor;
	pVertices2[2].Diffuse	 = DiffuseColor;
	pVertices2[3].Diffuse	 = DiffuseColor;
#if 0
		pVertices2[0].Diffuse.a  = ((Particle->m_Color>>24)&0xff);
		pVertices2[0].Diffuse.r  = ((Particle->m_Color>>16)&0xff);
		pVertices2[0].Diffuse.g  = ((Particle->m_Color>>8)&0xff);
		pVertices2[0].Diffuse.b  = (Particle->m_Color&0xff);
		pVertices2[1].Diffuse.a  = ((Particle->m_Color>>24)&0xff);
		pVertices2[1].Diffuse.r  = ((Particle->m_Color>>16)&0xff);
		pVertices2[1].Diffuse.g  = ((Particle->m_Color>>8)&0xff);
		pVertices2[1].Diffuse.b  = (Particle->m_Color&0xff);
		pVertices2[2].Diffuse.a  = ((Particle->m_Color>>24)&0xff);
		pVertices2[2].Diffuse.r  = ((Particle->m_Color>>16)&0xff);
		pVertices2[2].Diffuse.g  = ((Particle->m_Color>>8)&0xff);
		pVertices2[2].Diffuse.b  = (Particle->m_Color&0xff);
		pVertices2[3].Diffuse.a  = ((Particle->m_Color>>24)&0xff);
		pVertices2[3].Diffuse.r  = ((Particle->m_Color>>16)&0xff);
		pVertices2[3].Diffuse.g  = ((Particle->m_Color>>8)&0xff);
		pVertices2[3].Diffuse.b  = (Particle->m_Color&0xff);
#endif

		pVertices2[0].allowedRotDir=Particle->m_allowedRotDir;
		pVertices2[1].allowedRotDir=Particle->m_allowedRotDir;
		pVertices2[2].allowedRotDir=Particle->m_allowedRotDir;
		pVertices2[3].allowedRotDir=Particle->m_allowedRotDir;

	int ix = Particle->m_Type / (iatlassize/isubtexturesize);
	int iy = Particle->m_Type-ix*(iatlassize/isubtexturesize);
	pVertices2[0].uv  = D3DXFROMWINEVECTOR2(ix*isubtexturesize/float(iatlassize),iy*isubtexturesize/float(iatlassize));
	pVertices2[1].uv  = D3DXFROMWINEVECTOR2((ix+1)*isubtexturesize/float(iatlassize),iy*isubtexturesize/float(iatlassize));
	pVertices2[2].uv  = D3DXFROMWINEVECTOR2(ix*isubtexturesize/float(iatlassize),(iy+1)*isubtexturesize/float(iatlassize));
	pVertices2[3].uv  = D3DXFROMWINEVECTOR2((ix+1)*isubtexturesize/float(iatlassize),(iy+1)*isubtexturesize/float(iatlassize));

	Ptr[0]=pVertices2[0];
	Ptr[1]=pVertices2[1];
	Ptr[2]=pVertices2[2];
	Ptr[3]=pVertices2[3];

#ifdef __VERTEXCACHE__
	pVertices2+=4;
#endif
	Ptr+=4;

	// Increase vertex count and flush buffer if full
	Num++;

      // Go to next particle
      Particle = Particle->m_Next;
    }


  }

void cParticleEmitter::Free()
{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

  // Decrease reference count and free resources if needed
  if(m_RefCount)
    m_RefCount--;
  if(!m_RefCount) {

     //if(m_pAtlasTexture)m_pAtlasTexture->Release();
    // Release shader data
	//if(m_pEffect)m_pEffect->Release();

	//if(m_pDecl)m_pDecl->Release();
  }

  // Clear particle data
  m_NumParticles = 0;
  delete m_Particles;
  m_Particles = 0;
#ifdef __VERTEXCACHE__
  delete m_vertexCache;
  m_VB.clear();
  m_IB.clear();
#endif
}

void cParticleEmitter::Add(__WORD__ Type, D3DXFROMWINEVECTOR3& vecPos, float Size, __WORD__ Color, __WORD__ Life, D3DXFROMWINEVECTOR3 vecVelocity, D3DXFROMWINEVECTOR4 allowedRotDir,char* description)
{
  // Allocate a particle object and add to head of list
  cParticle *Particle = new cParticle();
  Particle->m_Prev = 0;
  if((Particle->m_Next = m_Particles))
    m_Particles->m_Prev = Particle;
  m_Particles = Particle;

  // Set particle data
  Particle->m_Type   = Type-start;

  Particle->m_vecPos = (vecPos);

  Particle->m_Size   = Size;
  Particle->m_Color  = Color;
  Particle->m_Life   = Life;
  Particle->m_vecVelocity = (vecVelocity);
  Particle->m_allowedRotDir=allowedRotDir;
  if(description)
  strcpy(Particle->m_description,description);
  else
  strcpy(Particle->m_description,"");
}

void cParticleEmitter::ClearAll()
{
  // Clear particle data
  delete m_Particles;
  m_Particles = 0;
}

bool cParticleEmitter::Begin(/*D3DXFROMWINEMATRIX *matView, D3DXFROMWINEMATRIX *matProj,D3DXFROMWINEVECTOR4& xCamPos*/)
{
#if 0
    // Disable lighting
    IRenderer::GetRendererInstance()->SetRenderState(D3DRS_LIGHTING, false));

    // Enable alpha testing for transparent blits
    IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHATESTENABLE, true));
    IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHAREF, 0x08));
    IRenderer::GetRendererInstance()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));

		IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
#endif
		D3DXFROMWINEMATRIX matView, matProj;
		matView=g_CameraViewMatrix;
		matProj=g_CameraProjMatrix;
		D3DXFROMWINEVECTOR4 xCamPos(g_CameraPosition.x,g_CameraPosition.y,g_CameraPosition.z,1.0f);

    m_CountPolys=0;
  // Set vertex shader, declaration, and stream sources

  IRenderer::GetRendererInstance()->setShader(m_pEffect);
  IRenderer::GetRendererInstance()->setVertexFormat(m_pDecl);

  // Get view * proj transformation, transpose, and store it
  D3DXFROMWINEMATRIX matVP = matView * matProj;
  D3DXFROMWINEMatrixTranspose(&matVP, &matVP);

  IRenderer::GetRendererInstance()->setShaderConstant4x4f("viewProj", matVP );
  IRenderer::GetRendererInstance()->setTexture("Sampler", m_pAtlasTexture);

  // Get right and up vectors from view transformation and store as constants
  D3DXFROMWINEVECTOR4 vecRight, vecUp;
  D3DXFROMWINEVECTOR4 v1(matView.m[0][0], matView.m[1][0], matView.m[2][0], 0.0f);
  D3DXFROMWINEVECTOR4 v2(matView.m[0][1], matView.m[1][1], matView.m[2][1], 0.0f);
  D3DXFROMWINEVec4Normalize(&vecRight, &v1);
  D3DXFROMWINEVec4Normalize(&vecUp,    &v2);

  IRenderer::GetRendererInstance()->setShaderConstant4f( "xCamPos", xCamPos );
  IRenderer::GetRendererInstance()->setShaderConstant4f( "right", vecRight );
  IRenderer::GetRendererInstance()->setShaderConstant4f( "up", vecUp );

  return true;
}

void cParticleEmitter::Render()
{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

  __WORD__ LastTexture = -1;

  // MUST CALL BEGIN BEFORE CALLING RENDER!!!
	
	//LOG_PRINT("\n");
        
	#ifdef __VERTEXCACHE__
	m_vertexCache->Draw(m_NumParticles*4, m_NumParticles*2,pIndices,pVertices);
	#else
	// Set the stream sources
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, m_NumParticles*4, m_NumParticles*2, &m_IB[0], &m_IB[0], CONSTANT_INDEX2, &m_VB[0], &m_VB[0], sizeof(sShaderVertex));
	#endif

  m_CountPolys+=m_NumParticles*2;
}

void cParticleEmitter::End()
{
    // IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
    // Disable alpha testing and alpha blending
}

void cParticleEmitter::print_date (char * title, struct ln_zonedate* date)
{

	//LOG_PRINT("\n%s\n",title);
	//LOG_PRINT(" Year	  : %d\n", date->years);
	//LOG_PRINT(" Month   : %d\n", date->months);
	//LOG_PRINT(" Day	  : %d\n", date->days);
	//LOG_PRINT(" Hours   : %d\n", date->hours);
	//LOG_PRINT(" Minutes : %d\n", date->minutes);
	//LOG_PRINT(" Seconds : %f\n", date->seconds);
}

void cParticleEmitter::EqutoHoriz(char* name,double JD,struct ln_equ_posn object, struct ln_lnlat_posn observer)
{

	struct ln_hrz_posn hrz;
	ln_get_hrz_from_equ (&object, &observer, JD, &hrz);
	//failed += test_result ("(Transforms) Equ to Horiz ALT ", hrz.alt, 38.9213888888, 0.00000001);
	//failed += test_result ("(Transforms) Equ to Horiz AZ ", hrz.az, 180.0, 0.00000001);
	//LOG_PRINT("%s Horiz ALT %f\n",name, hrz.alt);
	//LOG_PRINT("%s Horiz AZ %f\n",name, hrz.az);
}
D3DXFROMWINEVECTOR4 g_allowedRot(0.0f, 0.0f, 0.0f,1.0f);
int cParticleEmitter::sun (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz)
{

	struct ln_helio_posn pos;

	/* geometric coordinates */
	ln_get_solar_geom_coords (JD, &pos);
	//LOG_PRINT("Solar Coords longitude (deg) %f\n", pos.L);
	//LOG_PRINT("	     latitude (deg) %f\n", pos.B);
	//LOG_PRINT("	     radius vector (AU) %f\n", pos.R);

	/* ra, dec */
	ln_get_solar_equ_coords (JD, &equ);
	//LOG_PRINT("Solar Position RA %f\n", equ.ra);
	//LOG_PRINT("	     DEC %f\n", equ.dec);

	//EqutoHoriz("Sun",JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	solarsystempositions[0]=CalculatePosition(equ.ra,equ.dec,JD);
    D3DXFROMWINEVECTOR4 v(0.0f, 1.0f, 0.0f,0.0f);
	Add(PARTICLE_SUN,
							solarsystempositions[0],
							50.0f, 0xFFFFFFFF, 0, vO,v,"Sun");
	#if 0
	/* rise, set and transit */
	if (ln_get_solar_rst (JD, &observer, &rst) == 1)
		//LOG_PRINT("Sun is circumpolar\n");
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}
	#endif
	return 0;
}
int cParticleEmitter::moon (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz)
{

	struct ln_rect_posn moon;
	struct ln_lnlat_posn ecl;

	/* get the lunar geopcentric position in km, earth is at 0,0,0 */
	ln_get_lunar_geo_posn (JD, &moon, 0);
	//LOG_PRINT("lunar x %f  y %f  z %f\n",moon.X, moon.Y, moon.Z);

	/* Long Lat */
	ln_get_lunar_ecl_coords (JD, &ecl, 0);
	//LOG_PRINT("lunar int %f	lat %f\n",ecl.lng, ecl.lat);

	/* RA, DEC */
	ln_get_lunar_equ_coords (JD, &equ);
	//LOG_PRINT("lunar RA %f	Dec %f\n",equ.ra, equ.dec);

	//EqutoHoriz("Moon",JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	solarsystempositions[1]=CalculatePosition(equ.ra,equ.dec,JD);
	Add(PARTICLE_MOON,
							solarsystempositions[1],
							50.0f, 0xFFFFFFFF, 0, vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f),"Moon");
	#if 0
	/* moon earth distance */
	//LOG_PRINT("lunar distance km %f\n", ln_get_lunar_earth_dist(JD));

	/* lunar disk, phase and bright limb */
	//LOG_PRINT("lunar disk %f\n", ln_get_lunar_disk(JD));
	//LOG_PRINT("lunar phase %f\n", ln_get_lunar_phase(JD));
	//LOG_PRINT("lunar bright limb %f\n", ln_get_lunar_bright_limb(JD));

	/* rise, set and transit time */
	if (ln_get_lunar_rst (JD, &observer, &rst) == 1)
		//LOG_PRINT("Moon is circumpolar\n");
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}

	/* rise, set and transit time */
	if (ln_get_lunar_rst (JD - 24, &observer, &rst) == 1)
		//LOG_PRINT("Moon is circumpolar\n");
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}

	/* rise, set and transit time */
	if (ln_get_lunar_rst (JD - 25, &observer, &rst) == 1)
		//LOG_PRINT("Moon is circumpolar\n");
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}
	#endif
	return 0;
}
int cParticleEmitter::planet (double JD,struct ln_lnlat_posn observer,PARTICLES body,struct ln_equ_posn equ,struct ln_hrz_posn hrz,double mag)
{

	struct ln_helio_posn pos;
	struct lnh_equ_posn hequ;
	double au;

	char name[256];
	switch(body)
	{
		case PARTICLE_MERCURY:
			stx_snprintf(name, 256, "Mercury");
			break;
		case PARTICLE_VENUS:
			stx_snprintf(name, 256, "Venus");
			break;
		case PARTICLE_EARTH:
			stx_snprintf(name, 256, "Earth");
			break;
		case PARTICLE_MARS:
			stx_snprintf(name, 256, "Mars");
			break;
		case PARTICLE_JUPITER:
			stx_snprintf(name, 256, "Jupiter");
			break;
		case PARTICLE_SATURN:
			stx_snprintf(name, 256, "Saturn");
			break;
		case PARTICLE_URANUS:
			stx_snprintf(name, 256, "Uranus");
			break;
		case PARTICLE_NEPTUNE:
			stx_snprintf(name, 256, "Neptune");
			break;
		case PARTICLE_PLUTO:
			stx_snprintf(name, 256, "Pluto");
			break;
	}

	/* longitude, latitude and radius vector */
		switch(body)
		{
			case PARTICLE_MERCURY:
				ln_get_mercury_helio_coords(JD, &pos);
				break;
			case PARTICLE_VENUS:
				ln_get_venus_helio_coords(JD, &pos);
				break;
			case PARTICLE_EARTH:
				ln_get_earth_helio_coords(JD, &pos);
				break;
			case PARTICLE_MARS:
				ln_get_mars_helio_coords(JD, &pos);
				break;
			case PARTICLE_JUPITER:
				ln_get_jupiter_helio_coords(JD, &pos);
				break;
			case PARTICLE_SATURN:
				ln_get_saturn_helio_coords(JD, &pos);
				break;
			case PARTICLE_URANUS:
				ln_get_uranus_helio_coords(JD, &pos);
				break;
			case PARTICLE_NEPTUNE:
				ln_get_neptune_helio_coords(JD, &pos);
				break;
			case PARTICLE_PLUTO:
				ln_get_pluto_helio_coords(JD, &pos);
				break;
		}
	STX_PRINT("%s L %f B %f R %f\n",name, pos.L, pos.B, pos.R);

	/* RA, DEC */
		switch(body)
		{
			case PARTICLE_MERCURY:
				ln_get_mercury_equ_coords (JD, &equ);
				break;
			case PARTICLE_VENUS:
				ln_get_venus_equ_coords (JD, &equ);
				break;
			case PARTICLE_EARTH:
				break;
			case PARTICLE_MARS:
				ln_get_mars_equ_coords (JD, &equ);
				break;
			case PARTICLE_JUPITER:
				ln_get_jupiter_equ_coords (JD, &equ);
				break;
			case PARTICLE_SATURN:
				ln_get_saturn_equ_coords (JD, &equ);
				break;
			case PARTICLE_URANUS:
				ln_get_uranus_equ_coords (JD, &equ);
				break;
			case PARTICLE_NEPTUNE:
				ln_get_neptune_equ_coords (JD, &equ);
				break;
			case PARTICLE_PLUTO:
				ln_get_pluto_equ_coords (JD, &equ);
				break;
		}
	ln_equ_to_hequ (&equ, &hequ);
	STX_PRINT("%s RA %d:%d:%f Dec %d:%d:%f\n",name, hequ.ra.hours, hequ.ra.minutes, hequ.ra.seconds, hequ.dec.degrees, hequ.dec.minutes, hequ.dec.seconds);
/*
	struct ln_hrz_posn hrz;
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	STX_PRINT("%s Horiz ALT %f\n",name, hrz.alt);
	STX_PRINT("%s Horiz AZ %f\n",name, hrz.az);
*/
	//EqutoHoriz(name,JD,equ, observer);
	ln_get_hrz_from_equ (&equ, &observer, JD, &hrz);
	int ii=0;
		switch(body)
		{
			case PARTICLE_MERCURY:
				au = ln_get_mercury_magnitude (JD);
				ii=2;
				break;
			case PARTICLE_VENUS:
				au = ln_get_venus_magnitude (JD);
				ii=3;
				break;
			case PARTICLE_EARTH:
				au = 0;
				ii=0;
				break;
			case PARTICLE_MARS:
				au = ln_get_mars_magnitude (JD);
				ii=4;
				break;
			case PARTICLE_JUPITER:
				au = ln_get_jupiter_magnitude (JD);
				ii=5;
				break;
			case PARTICLE_SATURN:
				au = ln_get_saturn_magnitude (JD);
				ii=6;
				break;
			case PARTICLE_URANUS:
				au = ln_get_uranus_magnitude (JD);
				ii=7;
				break;
			case PARTICLE_NEPTUNE:
				au = ln_get_neptune_magnitude (JD);
				ii=8;
				break;
			case PARTICLE_PLUTO:
				au = ln_get_pluto_magnitude (JD);
				ii=9;
				break;
		}
		mag=au;
	STX_PRINT("%s -> magnitude %f\n",name,au);

	solarsystempositions[ii]=CalculatePosition(equ.ra,equ.dec,JD);
	Add(body,
							solarsystempositions[ii],
							50.0f, 0xFFFFFFFF, 0, vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f),name);
	#if 0

	/* Earth - Mars dist AU */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_earth_dist (JD);
				break;
			case VENUS:
				au = ln_get_venus_earth_dist (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_earth_dist (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_earth_dist (JD);
				break;
			case SATURN:
				au = ln_get_saturn_earth_dist (JD);
				break;
			case URANUS:
				au = ln_get_uranus_earth_dist (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_earth_dist (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_earth_dist (JD);
				break;
		}
	STX_PRINT("%s -> Earth dist (AU) %f\n",name,au);

	/* Sun - Mars Dist AU */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_solar_dist (JD);
				break;
			case VENUS:
				au = ln_get_venus_solar_dist (JD);
				break;
			case EARTH:
				au = ln_get_earth_solar_dist (JD);
				break;
			case MARS:
				au = ln_get_mars_solar_dist (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_solar_dist (JD);
				break;
			case SATURN:
				au = ln_get_saturn_solar_dist (JD);
				break;
			case URANUS:
				au = ln_get_uranus_solar_dist (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_solar_dist (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_solar_dist (JD);
				break;
		}
	STX_PRINT("%s -> Sun dist (AU) %f\n",name,au);

	/* Mars disk, magnitude and phase */
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_disk (JD);
				break;
			case VENUS:
				au = ln_get_venus_disk (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_disk (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_disk (JD);
				break;
			case SATURN:
				au = ln_get_saturn_disk (JD);
				break;
			case URANUS:
				au = ln_get_uranus_disk (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_disk (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_disk (JD);
				break;
		}
	STX_PRINT("%s -> illuminated disk %f\n",name,au);
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_magnitude (JD);
				break;
			case VENUS:
				au = ln_get_venus_magnitude (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_magnitude (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_magnitude (JD);
				break;
			case SATURN:
				au = ln_get_saturn_magnitude (JD);
				break;
			case URANUS:
				au = ln_get_uranus_magnitude (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_magnitude (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_magnitude (JD);
				break;
		}
	STX_PRINT("%s -> magnitude %f\n",name,au);
		switch(body)
		{
			case MERCURY:
				au = ln_get_mercury_phase (JD);
				break;
			case VENUS:
				au = ln_get_venus_phase (JD);
				break;
			case EARTH:
				au = 0;
				break;
			case MARS:
				au = ln_get_mars_phase (JD);
				break;
			case JUPITER:
				au = ln_get_jupiter_phase (JD);
				break;
			case SATURN:
				au = ln_get_saturn_phase (JD);
				break;
			case URANUS:
				au = ln_get_uranus_phase (JD);
				break;
			case NEPTUNE:
				au = ln_get_neptune_phase (JD);
				break;
			case PLUTO:
				au = ln_get_pluto_phase (JD);
				break;
		}
	STX_PRINT("%s -> phase %f\n",name,au);

	bool r=false;
	switch(body)
	{
		case MERCURY:
			r=ln_get_mercury_rst (JD, &observer, &rst);
			break;
		case VENUS:
			r=ln_get_venus_rst (JD, &observer, &rst);
			break;
		case EARTH:
			r=false;
			break;
		case MARS:
			r=ln_get_mars_rst (JD, &observer, &rst);
			break;
		case JUPITER:
			r=ln_get_jupiter_rst (JD, &observer, &rst);
			break;
		case SATURN:
			r=ln_get_saturn_rst (JD, &observer, &rst);
			break;
		case URANUS:
			r=ln_get_uranus_rst (JD, &observer, &rst);
			break;
		case NEPTUNE:
			r=ln_get_neptune_rst (JD, &observer, &rst);
			break;
		case PLUTO:
			r=ln_get_pluto_rst (JD, &observer, &rst);
			break;
	}
		/* rise, set and transit time */
	if (r == 1)
		STX_PRINT("%s is circumpolar\n",name);
	else {
		ln_get_local_date (rst.rise, &rise);
		ln_get_local_date (rst.transit, &transit);
		ln_get_local_date (rst.set, &set);
		print_date ("Rise", &rise);
		print_date ("Transit", &transit);
		print_date ("Set", &set);
	}
	#endif
	return 0;
}
D3DXFROMWINEVECTOR3 cParticleEmitter::CalculatePosition(double aequra, double aequdec, double JD)
{

	float f=65.017f*float(D3DXFROMWINE_PI)/180.0f;
	float r=1000.0f;
	D3DXFROMWINEVECTOR3 position;

	//aequdec=-1.0f*aequdec+90.0f;
	//aequra/=2.0f;//???

	float equdec=float(aequdec)*float(D3DXFROMWINE_PI)/180.0f;
	float equra=float(aequra)*float(D3DXFROMWINE_PI)/180.0f;

	float theta2=equdec;//???
	float varphi2=equra;//???
if(0)
{
	//double t=0;//*D3DXFROMWINE_PI/180.0;
	double t=ln_get_mean_sidereal_time (JD);
	//double t=ln_get_apparent_sidereal_time (JD);

	float H = float(t - equra);//Local Hour Angle
	float a = asinf(sinf(equdec)*sinf(f) + cosf(equdec)*cosf(f)*cosf(H));//altitude
	float A = asinf(- sinf(H)*cosf(equdec) / cosf(a));//azimuth
	theta2=a;//???
	varphi2=A;//???
}

	#if 0
	position.x=r*cosf(varphi)*sinf(theta)+g_CameraPosition.x;
    position.z=r*cosf(theta)+g_CameraPosition.z;
    position.y=r*sinf(varphi)*sinf(theta)+g_CameraPosition.y;
    #elif 0
	position.x=r*cosf(varphi)*sinf(theta)+g_CameraPosition.x;
    position.y=r*sinf(varphi)*sinf(theta)+g_CameraPosition.y;
    position.z=r*cosf(theta)+g_CameraPosition.z;
    #elif 0
	position.x=r*sinf(varphi2)*cosf(theta2)+g_CameraPosition.x;
	position.z=r*sinf(varphi2)*sinf(theta2)+g_CameraPosition.z;
	position.y=r*cosf(varphi2)+g_CameraPosition.y;
#elif 0
	position.x= r * cosf(varphi2) * cosf(theta2)+g_CameraPosition.x;
	position.y= r * cosf(varphi2) * sinf(theta2)+g_CameraPosition.y;
	position.z= r * sinf(varphi2)+g_CameraPosition.z;
#elif 1
	position.x = r * cosf(theta2) * cosf(varphi2)+g_CameraPosition.x;
	position.z = r * cosf(theta2) * sinf(varphi2)+g_CameraPosition.z;
	position.y = r * sinf(theta2)+g_CameraPosition.y;// = should be identical to starting cZ value
#elif 0
	x=cosf(theta)*sinf(phi);
	y=sinf(theta)*sinf(phi);
			  z=cos(phi);
#endif
	      /*
	//LOG_PRINT("position.x=%f\n",position.x);
	//LOG_PRINT("position.y=%f\n",position.y);
	//LOG_PRINT("position.z=%f\n",position.z);
	    */
	return position;

}
cParticleEmitter::CatalogElement catalog[NSTARS];
D3DXFROMWINEVECTOR3 catalogV3[NSTARS];
D3DXFROMWINEVECTOR3 catalogV3_[NSTARS];
void cParticleEmitter__DrawStars()
{
	for(unsigned int i=0;i<NSTARS;i++)
	{
		D3DXFROMWINEVECTOR3 pos=catalogV3_[i];
		IRenderer::GetRendererInstance()->drawCircle(pos.x,pos.y,1.0f,D3DXFROMWINEVECTOR4(1,1,1,1));
	}
}

//char testline[]="   1 	 BD+44 4550	 3 36042	  46	       000001.1+444022000509.9+451345114.44-16.88 6.70	+0.07 +0.08	    A1Vn	       -0.012-0.018	 -018	   195	4.2  21.6AC   3";
/*
ra=4.58333*10-3
dec=44.67277778

ra=1.29125
dec=45.22916667
mag=6.70
*/
void cParticleEmitter::ReadCatalog()
{
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	double JD;
	/* get Julian day from local time */
	JD = ln_get_julian_from_sys();
	
	//LOG_PRINT("JD %f\n", JD);

	static float magmin=1.0f;
	static float magmax=1.0f;
	int equinoxB1900=60;
	int equinoxJ2000=75;
	int equinox=equinoxJ2000;
	int c=0;
	int c2=0;
	char str[2000];
	

	std::string pName=stx_convertpath("/Stars/catalog");

	fstream file_op(pName.c_str(), ios::in);
	

	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);//ZZZ
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	while(!file_op.eof())
	{
	      file_op.getline(str,2000);
			  //std::cout <<str;
			  c++;
//			  strcpy(str,testline);


			  char buf[11];
			  char name0[11];
			  char varname[11];

			  name0[0]='\0';
			  varname[0]='\0';

			  buf[0]=str[4];
			  buf[1]=str[5];
			  buf[2]=str[6];
			  buf[3]=str[7];
			  buf[4]=str[8];
			  buf[5]=str[9];
			  buf[6]=str[10];
			  buf[7]=str[11];
			  buf[8]=str[12];
			  buf[9]=str[13];
			  buf[10]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  strcpy(name0,buf);

			  buf[0]=str[51];
			  buf[1]=str[52];
			  buf[2]=str[53];
			  buf[3]=str[54];
			  buf[4]=str[55];
			  buf[5]=str[56];
			  buf[6]=str[57];
			  buf[7]=str[58];
			  buf[8]=str[59];
			  buf[9]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  strcpy(varname,buf);

			  buf[0]=str[equinox];
			  buf[1]=str[equinox+1];
			  buf[2]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  int rah;
			  int r=sscanf (buf,"%d",&rah);
	
			  if(r!=1)continue;//LOG_PRINT("h r!=1:%d\n",c);
	
			  buf[0]=str[equinox+2];
			  buf[1]=str[equinox+3];
			  buf[2]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  int ram;
			  r=sscanf (buf,"%d",&ram);
	
			  if(r!=1)continue;//LOG_PRINT("m r!=1:%d\n",c);
	
			  buf[0]=str[equinox+4];
			  buf[1]=str[equinox+5];
			  buf[2]=str[equinox+6];
			  buf[3]=str[equinox+7];
			  buf[4]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  float ras;
			  r=sscanf (buf,"%f",&ras);
	
			  if(r!=1)continue;//LOG_PRINT("s r!=1:%d\n",c);
	
			  float x=(((ras/60.0f)+float(ram))/60.0f+float(rah))*15.0f;

/*
			//LOG_PRINT("rah=%d\n",rah);
			//LOG_PRINT("ram=%d\n",ram);
			//LOG_PRINT("ras=%f\n",ras);
*/
			  buf[0]=str[equinox+8];
			  buf[1]=str[equinox+9];
			  buf[2]=str[equinox+10];
			  buf[3]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  int ded;
			  r=sscanf (buf,"%d",&ded);
	
			  if(r!=1)continue;//LOG_PRINT("d r!=1:%d\n",c);
	
			  buf[0]=str[equinox+11];
			  buf[1]=str[equinox+12];
			  buf[2]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  int dem;
			  r=sscanf (buf,"%d",&dem);
	
			  if(r!=1)continue;//LOG_PRINT("am r!=1:%d\n",c);
	
			  buf[0]=str[equinox+13];
			  buf[1]=str[equinox+14];
			  buf[2]='\0';
//			//LOG_PRINT("buf=%s\n",buf);

			  int des;
			  r=sscanf (buf,"%d",&des);
	
			  if(r!=1)continue;//LOG_PRINT("as r!=1:%d\n",c);
	
			  float y=((float(des)/60.0f)+float(dem))/60.0f*((ded > 0) ? 1 : ((ded < 0) ? -1 : 0))+float(ded);
/*
			//LOG_PRINT("ded=%d\n",ded);
			//LOG_PRINT("dem=%d\n",dem);
			//LOG_PRINT("des=%d\n",des);
*/
			  buf[0]=str[103];
			  buf[1]=str[104];
			  buf[2]=str[105];
			  buf[3]=str[106];
			  buf[4]=str[107];
			  buf[5]='\0';
			  float mag;
			  r=sscanf (buf,"%f",&mag);
	
			  if(r!=1)continue;//LOG_PRINT("mag r!=1:%d\n",c);
	

			  //magmin=0.010000
			  //magmax=7.960000
			  float z=(1.0f-(mag-0.01f)/7.95f)*7.5f+2.5f;//*43.0f;
			  catalogV3[c-1].x=x;
			  catalogV3[c-1].y=y;
			catalog[c-1].ra=x;
			catalog[c-1].dec=y;
			catalog[c-1].mag=mag;
			catalog[c-1].name[0]='\0';
			catalog[c-1].varname[0]='\0';
			  strcpy(catalog[c-1].name,name0);
			  strcpy(catalog[c-1].varname,varname);
/*
			//LOG_PRINT("name=%s\n",catalog[c].name);
			//LOG_PRINT("varname=%s\n",catalog[c].varname);
			//LOG_PRINT("c=%d\n",c);
			//LOG_PRINT("x=%f\n",x);
			//LOG_PRINT("y=%f\n",y);
			//LOG_PRINT("mag=%f\n",mag);

			exit(0);
			*/
//			  float z=mag;//*43.0f;
			  magmin=std::min(magmin,mag);
			   magmax=std::max(magmax,mag);
			//g_quadVertices[c++]=Vertex(x,y,z);
			  //ln_equ_posn equ;
			  //equ.dec=y;
			  //equ.ra=x;
        D3DXFROMWINEVECTOR3 v1=CalculatePosition(x,y,JD);
        D3DXFROMWINEVECTOR4 v2(0.0f, 1.0f, 0.0f,0.0f);
        Add(PARTICLE_STAR,
							v1,//D3DXFROMWINEVECTOR3(x,y,z),
							z, 0xFFFFFFFF, 0, vO, v2, str);
	

#if 0
	if(*((int*)m_pEffect)==0x20202020)
	{
	
	  //LOG_PRINT("m_pEffect==0x20202020\n");
	  //LOG_PRINT("c-1=%d\n",c-1);
	  //LOG_PRINT("c2=%d\n",c2);
	
	  exit(0);
	}
#endif
	
	//std::cout << x << " " << y << " " << z << " "<< endl;
	c2++;

	}
	
	file_op.close();
	

	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	//LOG_PRINT("magmin=%f\n",magmin);
	//LOG_PRINT("magmax=%f\n",magmax);

	//LOG_PRINT("c2=%d\n",c2);
	//LOG_PRINT("c-1=%d\n",c-1);

/*
   1	      BD+44 4550      3 36042	       46	    000001.1+444022000509.9+451345114.44-16.88 6.70  +0.07 +0.08	 A1Vn		    -0.012-0.018      -018	195  4.2  21.6AC   3
9110	      BD+60 2667 225289 10962		   V567 Cas 235955.9+604526000506.2+611851117.40-01.06 5.80  -0.09 -0.32	 B8IVpHgMn	    +0.015+0.005      +014V	 50		    *
*/
	
	int i=0;
	//LOG_PRINT("i   =%d\n",i);
	//LOG_PRINT("name   =%s\n",cParticleEmitter::catalog[i].name);
	//LOG_PRINT("varname=%s\n",cParticleEmitter::catalog[i].varname);

	//LOG_PRINT("ra	 =%f\n",cParticleEmitter::catalog[i].ra);
	//LOG_PRINT("dec	 =%f\n",cParticleEmitter::catalog[i].dec);
	//LOG_PRINT("mag	 =%f\n",cParticleEmitter::catalog[i].mag);

	i=9109;
	//LOG_PRINT("i   =%d\n",i);
	//LOG_PRINT("name   =%s\n",cParticleEmitter::catalog[i].name);
	//LOG_PRINT("varname=%s\n",cParticleEmitter::catalog[i].varname);

	//LOG_PRINT("ra	 =%f\n",cParticleEmitter::catalog[i].ra);
	//LOG_PRINT("dec	 =%f\n",cParticleEmitter::catalog[i].dec);
	//LOG_PRINT("mag	 =%f\n",cParticleEmitter::catalog[i].mag);
	//exit(0);

	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

	struct ln_lnlat_posn observer;
		/* observers location (Edinburgh), used to calc rst */
	observer.lat = 65.017;
	observer.lng = 25.467;

	ln_equ_posn equ;
	ln_hrz_posn hrz;
	double mag=0;
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

	ZeroMemory(&equ,sizeof(ln_equ_posn));
	ZeroMemory(&hrz,sizeof(ln_hrz_posn));
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

	sun (JD,observer,equ,hrz);
	moon (JD,observer,equ,hrz);
	planet (JD,observer,PARTICLE_MERCURY,equ,hrz,mag);
	planet (JD,observer,PARTICLE_VENUS,equ,hrz,mag);
	//planet (JD,observer,PARTICLE_EARTH,equ,hrz,mag);
	planet (JD,observer,PARTICLE_MARS,equ,hrz,mag);
	planet (JD,observer,PARTICLE_JUPITER,equ,hrz,mag);
	planet (JD,observer,PARTICLE_SATURN,equ,hrz,mag);
	planet (JD,observer,PARTICLE_URANUS,equ,hrz,mag);
	planet (JD,observer,PARTICLE_NEPTUNE,equ,hrz,mag);
	planet (JD,observer,PARTICLE_PLUTO,equ,hrz,mag);
	

	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

		//if(equinox==equinoxJ2000) LOG_PRINT("equinoxJ2000\n");
	
		//if(equinox==equinoxB1900) LOG_PRINT("equinoxB1900\n");
		//LOG_PRINT("%d+11\n",c);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	

	return;
}
void cParticleEmitter::ReadConstellationLines(const char* fn)
{
/*
"; Andromeda\n"

"; Triangulum Australe\n"

"9275036411 0	     move to alpha\n"


	bool started=false;
  char * pch;
  pch = strtok (str," \t");
  while (pch != 0)
  {
    if(strcmp(pch,"Andomeda")==0) started=true;
    if(started)
    {
	}
    pch = strtok (0, " \t");
  }

*/
}
	void cParticleEmitter::render()
	{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);

	  cParticleEmitter::Begin();
					#ifdef __VERTEXCACHE__
					cParticleEmitter::m_vertexCache->Start();
					#endif
	  Render();
					#ifdef __VERTEXCACHE__
					cParticleEmitter::m_vertexCache->Flush();
					#endif
					cParticleEmitter::End();	}

//
// Line-sphere intersection.
// p=(line origin position - sphere position),
// d=line direction,
// r=sphere radius,
// i1=first intersection distance,
// i2=second intersection distance
// i1<=i2
//
// returns true if intersection found,false otherwise.
//
bool cParticleEmitter::LineSphereIntersect(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d,double r, double &i1, double &i2){
	double det,b;
	b = -D3DXFROMWINEVec3Dot(&p,&d);
	det=(b*b) - (p.x*p.x + p.y*p.y + p.z*p.z) + r*r;
	if (det<0){
		return false;
	}//else
	det= sqrt(det);
	i1= b - det;
	i2= b + det;
	return true;

};
//
// Line-sphere intersection.
// p=(line origin position - sphere position),
// d=line direction,
// (line equation is p+d*t)
// r=sphere radius,
// i1=first intersection,
// i2=second intersection
// i1<=i2
//
// returns true if intersection found,false otherwise.
//
bool cParticleEmitter::LineSphereIntersectNN(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d, double r, double &i1, double &i2){
 double det,b;
 b = -D3DXFROMWINEVec3Dot(&p,&d);
 double dd=D3DXFROMWINEVec3Dot(&d,&d);
 double scale = 1.0/dd;
 b*=scale;
 det=(b*b) - ((p.x*p.x + p.y*p.y + p.z*p.z) + r*r)*scale;
 if (det<0){
  return false;
 }//else
 det= sqrt(det);
 i1= b - det;
 i2= b + det;
 return true;
};
#if 0
int cParticleEmitter::Pick()
{
if((m_EmitterType==EMITTER_VEGETATION)&&initVegetation) return false;
if((m_EmitterType==EMITTER_ARBARO)&&initVegetation2) return false;
if((m_EmitterType==EMITTER_SKY)&&initSky) return false;
    int hr=D3D_OK;
    D3DXFROMWINEVECTOR3 vPickRayDir;
    D3DXFROMWINEVECTOR3 vPickRayOrig;

    // Get the pick ray from the mouse position
	D3DXFROMWINEMATRIXA16 matProj;
	//m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	matProj=g_CameraProjMatrix();

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	//LOG_PRINT("x=%d\n",ptCursor.x);
	//LOG_PRINT("y=%d\n",ptCursor.y);

	ScreenToClient( g_pd3dDevice->m_hWnd, &ptCursor );

	//LOG_PRINT("x=%d\n",ptCursor.x);
	//LOG_PRINT("y=%d\n",ptCursor.y);
	// Compute the vector of the pick ray in screen space
	D3DXFROMWINEVECTOR3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / g_pd3dDevice->width	      ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / g_pd3dDevice->height ) - 1 ) / matProj._22;
	v.z =  1.0f;

	// Get the inverse of the composite view and world matrix
	D3DXFROMWINEMATRIXA16 matView, matWorld, m,m2;
	//m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	//m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	matView=g_CameraViewMatrix();
	D3DXFROMWINEMatrixIdentity(&matWorld);

	m = matWorld * matView;
	m2 = matView*matProj;
	D3DXFROMWINEMatrixInverse( &m, 0, &m );

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

//	float fLeft, fRight, fBottom, fTop, fBack, fFront;
 D3DXFROMWINEVECTOR3 vLine[2];
 vLine[0]=vPickRayOrig;
 vLine[1]=vPickRayOrig+vPickRayDir*1000000.0f;

	 //LOG_PRINT("vLine[0].x=%f\n",vLine[0].x);
	//LOG_PRINT("vLine[0].y=%f\n",vLine[0].y);
	//LOG_PRINT("vLine[0].z=%f\n",vLine[0].z);
	 //LOG_PRINT("vLine[1].x=%f\n",vLine[1].x);
	//LOG_PRINT("vLine[1].y=%f\n",vLine[1].y);
	//LOG_PRINT("vLine[1].z=%f\n",vLine[1].z);

	cParticle* particle=m_Particles->m_Next;
	while(particle)
	{
		D3DXFROMWINEVECTOR3* pCenter0 = &particle->m_vecPos;
	D3DXFROMWINEVECTOR3 pCenter;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&matWorld,g_Camera.m_yaw,g_Camera.m_pitch,0.0f);
	Vec3TransformCoord(&pCenter,pCenter0,&matWorld);//m2);
/*
	float size=0.1f*particle->m_Size;
	// Find the Left, Right, Front and Back of this Node's AABB.
	fLeft = pCenter.x - size;
	fRight = pCenter.x + size;
	fBottom = pCenter.y - size;
	fTop = pCenter.y + size;
	// Be careful here, depth is different in DirectX's Left handed coordinate system.
	fBack = pCenter.z - size;
	fFront = pCenter.z + size;

	bool g_bOctreeCollisionDetection=true;//???
	// If BOTH Vertices of the Line are not in this Node, than there can not possibly
	// be an intersection, return false.
	if ( g_bOctreeCollisionDetection &&
		 (( vLine[0].x < fLeft || vLine[0].x > fRight ) ||
		 ( vLine[0].y < fBottom || vLine[0].y > fTop ))){// ||
		 //( vLine[0].z < fBack || vLine[0].z > fFront ))
		 //&&
		 //(( vLine[1].x < fLeft || vLine[1].x > fRight ) ||
		 //( vLine[1].y < fBottom || vLine[1].y > fTop ))){// ||
		 //( vLine[1].z < fBack || vLine[1].z > fFront )) ){
		 //LOG_PRINT("77\n");*/
	double i1=0;
	double i2=0;
	D3DXFROMWINEVECTOR3 v=g_Camera._pos-pCenter;
		if(!LineSphereIntersectNN(/*vPickRayOrig*/v, vPickRayDir,10, i1, i2))
		{
			particle=particle->m_Next;
			continue;
	}
	else
	{
		//LOG_PRINT("%s\n",particle->m_description);
				//LOG_PRINT("particle.x=%f\n",particle->m_vecPos.x);
				//LOG_PRINT("particle.y=%f\n",particle->m_vecPos.y);
				//LOG_PRINT("particle.z=%f\n",particle->m_vecPos.z);
		break;
	}

	}

    return hr;
}

#endif
