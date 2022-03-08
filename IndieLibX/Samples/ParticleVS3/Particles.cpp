/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "Particles.h"

//inline __DWORD__ FLOAT2DWORD(FLOAT f) { return *((__DWORD__*)&f); }

// Clear static variables to their defaults
ShaderID       cParticleEmitter::m_pShader   = -1;
VertexFormatID cParticleEmitter::m_pDecl     = -1;
TextureID cParticleEmitter::texID=-1;
unsigned int cParticleEmitter::N=0;

cParticleEmitter::cParticleEmitter()
{
  // Clear particle data
  m_NumParticles = 0;
  m_Particles    = 0;
}

cParticleEmitter::~cParticleEmitter()
{
  Free();  // Free particles
}

bool cParticleEmitter::Create(D3DXFROMWINEVECTOR3 *vecPosition,
                              __DWORD__ EmitterType,
                              __DWORD__ NumParticlesPerBuffer)
{
  // Save emitter's position
  m_vecPosition = (*vecPosition);

  // Save type of emitter
  m_EmitterType = EmitterType;

  // Save # of particles in buffer
  m_NumParticles = NumParticlesPerBuffer;
if(-1==m_pShader) {
	m_pShader=IRenderer::GetRendererInstance()->addShaderFromFile("/ParticleVS/Particles3.hlsl", "main", "main");
	#if 0
{//LOG_FNLN;//LOG_PRINT("m_pShader=%x\n", m_pShader);}
	#endif
	}
if(-1==m_pDecl) {
	// Vertex declaration
	FormatDesc g_VSParticleDecl[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		//0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	m_pDecl = IRenderer::GetRendererInstance()->addVertexFormat(g_VSParticleDecl, elementsOf(g_VSParticleDecl),m_pShader);
	#if 0
{//LOG_FNLN;//LOG_PRINT("m_pDecl=%x\n", m_pDecl);}
	#endif
 } 

  // Load textures if none already loaded
  if(texID==-1) {
        SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();
	if(!stx_fileExists("/particles_atlas.png"))
	{
    //m_pTextures = new TextureID[NUM_PARTICLE_TYPES];
			const char* filename[] =
			{
				"/AdvAniCh12/Particle_Smoke.png",
                                "/dx8sdk/tree01S.png",
                                "/dx8sdk/tree02S.png",
                                "/dx8sdk/tree35S.png",
                                "/textures/NVIDIA_Corporation/2d/Palm.png",
                                "/textures/ATI_SDK/grass001.png",
                                "/textures/ATI_SDK/grass002.png",
                                "/AdvAniCh12/Particle_People1.png",
                                "/AdvAniCh12/Particle_People2.png",
				//"/Orcs/Firetex.png",
				"/Smoke/fireBase.png",
				"/DeferredParticles/DeferredParticle.png",
				"/12_Explosion_Particle_Effect/explosion.png",
	//"/textures/NVIDIA_Corporation/1D/fire_gradient3.png",
	//"/textures/NVIDIA_Corporation/1D/FireGrade.png",
	"/Smoke/FireTransferFunction.png",
	//"/textures/ATI_SDK/Spectrum.png",
	//"/textures/NVIDIA_Corporation/2d/flame.png",
	"/new/R2VB_VTF/R2VB-Sort/FirePalette.png",
	//"/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds",// black ??? TRILINEAR, CLAMP
			};

	for(unsigned int i=0;i<elementsOf(filename);i++)
		av.push_back(filename[i]);

	N=sqrtf(av.size());
	if(N*N<av.size())
		N++;

	Image3 atlas;
	stx_CreateAtlas(atlas, av);
	texID=IRenderer::GetRendererInstance()->addTexture(&atlas, false, ss, 0);
	if(0)
	{
		std::string fn=stx_convertpath("/particles_atlas.png");
		//atlas.compress();
		atlas.saveImageLibImage(fn.c_str(), false);
	}
	}
	else
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/particles_atlas.png", false, ss);
  }

  // Return success
  return true;
}

void cParticleEmitter::Free()
{
  // Clear particle data
  m_NumParticles = 0;
  delete m_Particles;
  m_Particles = 0;
}

void cParticleEmitter::Add(__DWORD__ Type, D3DXFROMWINEVECTOR3 *vecPos, float Size, __DWORD__ Color, __DWORD__ Life, D3DXFROMWINEVECTOR3 *vecVelocity)
{
  // Allocate a particle object and add to head of list
  cParticle *Particle = new cParticle();
#if 0
  Particle->m_Prev = 0;
  if((Particle->m_Next = m_Particles))
    m_Particles->m_Prev = Particle;
  m_Particles = Particle;
#endif
  // Set particle data
  Particle->m_Type   = Type;
  Particle->m_vecPos = (*vecPos);
  Particle->m_Size   = Size;
  //Particle->m_Color  = Color;
  Particle->m_Life   = Life;
  Particle->m_vecVelocity = (*vecVelocity);

	unsigned int iatlassize=ATLAS_TEXTURE_SIZE;
	//LOG_PRINT("iatlassize=%d\n", iatlassize);
	//LOG_PRINT("N=%d\n", N);
	unsigned int isubtexturesize=iatlassize/N;
	int ix = Type / (iatlassize/isubtexturesize);
	int iy = Type-ix*(iatlassize/isubtexturesize);
	float fx = 0.25f;
	float fy = 0.25f;
	#if 0
	LOG_PRINT("Type=%d\n", Type);
	LOG_PRINT("ix=%d\n", ix);
	LOG_PRINT("iy=%d\n", iy);
	LOG_PRINT("fx=%f\n", fx);
	LOG_PRINT("fy=%f\n", fy);
	#endif
	sShaderVertex v;
        // Copy over particle data
        float HalfSize = Particle->m_Size * 0.5f;
        v.vecPos    = Particle->m_vecPos;
        v.vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, HalfSize);
        //v.Diffuse   = Particle->m_Color;
        v.uv         = D3DXFROMWINEVECTOR2((ix)*fx,(iy)*fy);
	VB.push_back(v);
        v.vecPos    = Particle->m_vecPos;
        v.vecOffset = D3DXFROMWINEVECTOR2(HalfSize, HalfSize);
        //v.Diffuse   = Particle->m_Color;
        v.uv         = D3DXFROMWINEVECTOR2((ix+1)*fx,(iy)*fy);
	VB.push_back(v);
        v.vecPos    = Particle->m_vecPos;
        v.vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize);
        //v.Diffuse   = Particle->m_Color;
        v.uv         = D3DXFROMWINEVECTOR2((ix)*fx,(iy+1)*fy);
	VB.push_back(v);
        v.vecPos    = Particle->m_vecPos;
        v.vecOffset = D3DXFROMWINEVECTOR2(HalfSize, -HalfSize);
        //v.Diffuse   = Particle->m_Color;
        v.uv         = D3DXFROMWINEVECTOR2((ix+1)*fx,(iy+1)*fy);
	VB.push_back(v);
    IB.push_back((VB.size()-4) * 4 + 0);
    IB.push_back((VB.size()-4) * 4 + 1);
    IB.push_back((VB.size()-4) * 4 + 2);
    IB.push_back((VB.size()-4) * 4 + 3);
    IB.push_back((VB.size()-4) * 4 + 2);
    IB.push_back((VB.size()-4) * 4 + 1);
}

void cParticleEmitter::ClearAll()
{
  // Clear particle data
  delete m_Particles;
  m_Particles = 0;
}

void cParticleEmitter::Render(D3DXFROMWINEMATRIX &amatView, D3DXFROMWINEMATRIX &amatProj)
{
#if 1
	D3DXFROMWINEMATRIX matView=amatView;//IRenderer::GetRendererInstance()->Getvv();
	D3DXFROMWINEMATRIX matProj=amatProj;//IRenderer::GetRendererInstance()->Getpp();
#else
	D3DXFROMWINEMATRIX matView=IRenderer::GetRendererInstance()->Getvv();
	D3DXFROMWINEMATRIX matProj=IRenderer::GetRendererInstance()->Getpp();
#endif
#if 1
  matVP = (matView) * (matProj);
  D3DXFROMWINEMatrixTranspose(&matVP, &matVP);
#else
  matVP = (matProj)*(matView);
#endif

  // Get right and up vectors from view transformation and store as constants
  D3DXFROMWINEVECTOR4 vr(matView.m[0][0], matView.m[1][0], matView.m[2][0], 0.0f);
  D3DXFROMWINEVECTOR4 vu(matView.m[0][1], matView.m[1][1], matView.m[2][1], 0.0f);

  D3DXFROMWINEVec4Normalize(&vecRight, &vr);
  D3DXFROMWINEVec4Normalize(&vecUp,    &vu);

  // Set vertex shader, declaration, and stream sources
  IRenderer::GetRendererInstance()->setShader(m_pShader);
  IRenderer::GetRendererInstance()->setVertexFormat(m_pDecl);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant4f("right", vecRight);
  IRenderer::GetRendererInstance()->setShaderConstant4f("up", vecUp);

  IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

  IRenderer::GetRendererInstance()->setTexture("Sampler", texID);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		VB.size(),
		IB.size()/3,
		&IB[0],
		&IB[0],
		CONSTANT_INDEX2,
		&VB[0],
		&VB[0],
		sizeof(sShaderVertex), iDrawFlag);
}

void cParticleEmitter::Process(__DWORD__ Elapsed)
{
#if 0
  cParticle *Particle = m_Particles;

  // Loop through all particles
  while(Particle != 0) {

    // Remember next particle
    cParticle *NextParticle = Particle->m_Next;
    
    // Set flag to remove from list
    bool RemoveFromList = false;

    // Decrease life of particle, but not if life == 0
    if(Particle->m_Life) {
      if(Particle->m_Life <= Elapsed)
        RemoveFromList = true;
      else
        Particle->m_Life -= Elapsed;
    }

    // Calculate scalar to use for velocity
    float Scalar = (float)Elapsed / 1000.0f;

    // Add velocity to particle positions
    Particle->m_vecPos += (Particle->m_vecVelocity * Scalar);
#if 0
    // Remove particle from list if flagged
    if(RemoveFromList == true) {

      // Have previous particle skip past one being deleted
      // or set new root if particle to be removed is the root
      if(Particle->m_Prev)
        Particle->m_Prev->m_Next = Particle->m_Next;
      else
        m_Particles = Particle->m_Next;

      // Set next particle's previous pointer
      if(Particle->m_Next)
        Particle->m_Next->m_Prev = Particle->m_Prev;

      // Delete particle
      Particle->m_Prev = 0;
      Particle->m_Next = 0;
      delete Particle;
    }
#endif

    // Go to next particle
    Particle = NextParticle;
  }
#endif
}


////////////////////////////////////////////////////////////////////////////
//
// Specialized particle functions
//
////////////////////////////////////////////////////////////////////////////
void cParticleEmitter::HandleSmoke(D3DXFROMWINEVECTOR3 *vecPos, __DWORD__ Elapsed)
{
  static __DWORD__ Timer = 0;

  // Update smoke timer and add a smoke particle as needed
  Timer += Elapsed;
  if(Timer > 66) {
    Timer = 0;

    // Pick a random direction to move particle
    float rot = (float)((stx_rand() % 628) / 100.0f);
  D3DXFROMWINEVECTOR3 v1(vecPos->x, 5.0f, vecPos->z);
  D3DXFROMWINEVECTOR3 v2((float)cos(rot)*20.0f, 0.0f, (float)sin(rot)*20.0f);
    Add(PARTICLE_SMOKE, &v1,
        10.0f, 0xFF222222, 1000, 
        &v2);
  }
}

void cParticleEmitter::HandleDucking(D3DXFROMWINEVECTOR3 *vecPos)
{
#if 0
  // Change people's particle types if chopper is close to them
  cParticle *Particle = m_Particles;
  while(Particle) {

    // Calculate distance from position to person (only on x/z axes)
    D3DXFROMWINEVECTOR3 vecDiff = Particle->m_vecPos - (*vecPos);
    float Dist = vecDiff.x*vecDiff.x + vecDiff.z*vecDiff.z;

    // If too close, make person duck, otherwise stand them up
    if(Dist < (40.0f * 40.0f))
      Particle->m_Type = PARTICLE_PEOPLE2;
    else
      Particle->m_Type = PARTICLE_PEOPLE1;

    // Go to next particle
    Particle = Particle->m_Next;
  }
#endif
}
/*


	// Load the textures for the foliage and create a texture atlas.
	Image3 atlas, grass0, grass1, leaves;
	std::string fn="/new/R2VB_VTF/R2VB-Sort/grass001.png";
	grass0.loadImageLibImage(fn.c_str());
	fn="/new/R2VB_VTF/R2VB-Sort/grass002.png";
	grass1.loadImageLibImage(fn.c_str());
	fn="/new/R2VB_VTF/R2VB-Sort/MapleLeaves.png";	
	leaves.loadImageLibImage(fn.c_str());

#if 1
	// Much easier to deal with uncompressed ...
	grass0.decompress();
	grass1.decompress();
	leaves.decompress();
#endif
	ubyte *fDest = atlas.create(FORMAT_RGBA8, 1024, 1024, 1, 1);
	ubyte *gSrc0 = grass0.getPixels();
	ubyte *gSrc1 = grass1.getPixels();
	ubyte *lSrc  = leaves.getPixels();
	for (int y = 0; y < 512; y++){
		stx_memcpy(fDest + 1024 * 4 * y,           gSrc0 + 512 * 4 * y, 512 * 4);
		stx_memcpy(fDest + 1024 * 4 * y + 512 * 4, lSrc  + 512 * 4 * y, 512 * 4);
	}
	for (int y = 0; y < 128; y++){
		stx_memcpy(fDest + 1024 * 4 * (512 + y), gSrc1 + 128 * 4 * y, 128 * 4);
	}
	foliage = IRenderer::GetRendererInstance()->addTexture(&atlas, false, IRenderer::GetRendererInstance()->GettrilinearAniso());

*/


