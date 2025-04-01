/*
  Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include "Particles.h"

//inline __WORD__ FLOAT2DWORD(FLOAT f) { return *((__WORD__*)&f); }

// Clear static variables to their defaults
ShaderID       cParticleEmitter::m_pShader   = -1;
VertexFormatID cParticleEmitter::m_pDecl     = -1;

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
                              __WORD__ EmitterType,
                              __WORD__ NumParticlesPerBuffer)
{
  // Save emitter's position
  m_vecPosition = (*vecPosition);

  // Save type of emitter
  m_EmitterType = EmitterType;

  // Save # of particles in buffer
  m_NumParticles = NumParticlesPerBuffer;

  // Load textures if none already loaded
  if(1) {

    //m_pTextures = new TextureID[NUM_PARTICLE_TYPES];
        SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();
	switch(0)
	{
	case 0:
	m_pTextures[0]=IRenderer::GetRendererInstance()->addImageLibTexture(
				"/AdvAniCh12/Particle_Smoke.png",
        			false,
				ss);
		break;
	case 1:
	m_pTextures[0]=IRenderer::GetRendererInstance()->addImageLibTexture(
				"/Orcs/Firetex.png",
        			false,
				IRenderer::GetRendererInstance()->Getlinear());
		break;
	case 2:
	m_pTextures[0]=IRenderer::GetRendererInstance()->addImageLibTexture(
				"/DeferredParticles/DeferredParticle.png",
        			false,
				IRenderer::GetRendererInstance()->Getlinear());
		break;
	case 3:
	m_pTextures[0]=IRenderer::GetRendererInstance()->addImageLibTexture(
				"/12_Explosion_Particle_Effect/explosion.png",
        			false,
				IRenderer::GetRendererInstance()->Getlinear());
		break;
	};

	m_pTextures[1]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/dx8sdk/tree01S.png",
                                false,
				ss);

	m_pTextures[2]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/dx8sdk/tree02S.png",
                                false,
				ss);

	m_pTextures[3]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/dx8sdk/tree35S.png",
                                false,
				ss);

	m_pTextures[4]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/textures/NVIDIA_Corporation/2d/Palm.png",
                                false,
				ss);

	m_pTextures[5]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/textures/ATI_SDK/grass001.png",
                                false,
				ss);

	m_pTextures[6]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/textures/ATI_SDK/grass002.png",
                                false,
				ss);
	m_pTextures[7]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/AdvAniCh12/Particle_People1.png",
                                false,
				ss);

	m_pTextures[8]=IRenderer::GetRendererInstance()->addImageLibTexture(
                                "/AdvAniCh12/Particle_People2.png",
				false,
				ss);
	
	#if 0
		LOG_FNLN;
		for(unsigned int i=0;i<NUM_PARTICLE_TYPES;i++)
			LOG_PRINT("m_pTextures[%d]=%x\n", i, m_pTextures[i]);
	#endif
  }
if(-1==m_pShader) {
	m_pShader=IRenderer::GetRendererInstance()->addShaderFromFile("/ParticleVS/Particles.hlsl", "main", "main");
	#if 0
{LOG_FNLN;LOG_PRINT("m_pShader=%x\n", m_pShader);}
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
{LOG_FNLN;LOG_PRINT("m_pDecl=%x\n", m_pDecl);}
	#endif
 } 
  // Create the vertex buffer
  VB.resize(m_NumParticles * 4);

  // Create the index buffer and fill with face data
  IB.resize(m_NumParticles * 6);
  for(__WORD__ i=0;i<m_NumParticles;i++) {
    IB[i*6+0] = i * 4 + 0;
    IB[i*6+1] = i * 4 + 1;
    IB[i*6+2] = i * 4 + 2;
    IB[i*6+3] = i * 4 + 3;
    IB[i*6+4] = i * 4 + 2;
    IB[i*6+5] = i * 4 + 1;
  }

	LOG_PRINT("VB.size()=%d\n", VB.size());
	LOG_PRINT("IB.size()=%d\n", IB.size());

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

void cParticleEmitter::Add(__WORD__ Type, D3DXFROMWINEVECTOR3 *vecPos, float Size, __WORD__ Color, __WORD__ Life, D3DXFROMWINEVECTOR3 *vecVelocity)
{
  // Allocate a particle object and add to head of list
  cParticle *Particle = new cParticle();
  Particle->m_Prev = 0;
  if((Particle->m_Next = m_Particles))
    m_Particles->m_Prev = Particle;
  m_Particles = Particle;

  // Set particle data
  Particle->m_Type   = Type;
  Particle->m_vecPos = (*vecPos);
  Particle->m_Size   = Size;
  //Particle->m_Color  = Color;
  Particle->m_Life   = Life;
  Particle->m_vecVelocity = (*vecVelocity);
}

void cParticleEmitter::ClearAll()
{
  // Clear particle data
  delete m_Particles;
  m_Particles = 0;
}

bool cParticleEmitter::Begin(D3DXFROMWINEMATRIX &amatView, D3DXFROMWINEMATRIX &amatProj)
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
  return true;
}

void cParticleEmitter::Render(D3DXFROMWINEMATRIX &amatView, D3DXFROMWINEMATRIX &amatProj)
{
	Begin(amatView, amatProj);
  __WORD__ LastTexture = -1;

  // MUST CALL BEGIN BEFORE CALLING RENDER!!!

  // Go through each type of particle to draw
  // Chance to optimize - speed this up
  for(__WORD__ i=0;i<NUM_PARTICLE_TYPES;i++) {

    // Start at first particle
    cParticle *Particle = m_Particles;

    // Set a count for flushing buffer
    __WORD__ Num = 0;
    VB.resize(m_NumParticles * 4);
    // Loop for all particles
    while(Particle != 0) {

      // Do types match?
      if(Particle->m_Type == i) {

        /* Set the type's texture (if not done already)
        if(i != LastTexture) {
          LastTexture = i;
          IRenderer::GetRendererInstance()->setTexture("Sampler", m_pTextures[i]);
        }
	*/

	//LOG_PRINT("VB.size()=%d, Num=%d\n", VB.size(), Num);

        // Copy over particle data
        float HalfSize = Particle->m_Size * 0.5f;
        VB[4*Num+0].vecPos    = Particle->m_vecPos;
        VB[4*Num+0].vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, HalfSize);
        //VB[4*Num+0].Diffuse   = Particle->m_Color;
        VB[4*Num+0].uv         = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
        VB[4*Num+1].vecPos    = Particle->m_vecPos;
        VB[4*Num+1].vecOffset = D3DXFROMWINEVECTOR2(HalfSize, HalfSize);
        //VB[4*Num+1].Diffuse   = Particle->m_Color;
        VB[4*Num+1].uv         = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
        VB[4*Num+2].vecPos    = Particle->m_vecPos;
        VB[4*Num+2].vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize);
        //VB[4*Num+2].Diffuse   = Particle->m_Color;
        VB[4*Num+2].uv         = D3DXFROMWINEVECTOR2(0.0f, 1.0f);
        VB[4*Num+3].vecPos    = Particle->m_vecPos;
        VB[4*Num+3].vecOffset = D3DXFROMWINEVECTOR2(HalfSize, -HalfSize);
        //VB[4*Num+3].Diffuse   = Particle->m_Color;
        VB[4*Num+3].uv         = D3DXFROMWINEVECTOR2(1.0f, 1.0f);

        // Increase vertex count and flush buffer if full
        Num++;
        if(Num >= m_NumParticles) {
	#if 0
{LOG_FNLN;LOG_PRINT("setShader\n");LOG_PRINT("setVertexFormat\n");}
	#endif
  // Set vertex shader, declaration, and stream sources
  IRenderer::GetRendererInstance()->setShader(m_pShader);
  IRenderer::GetRendererInstance()->setVertexFormat(m_pDecl);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant4f("right", vecRight);
  IRenderer::GetRendererInstance()->setShaderConstant4f("up", vecUp);

	#if 0

  stx_Dump4x4f(matVP);
  stx_Dump4f(vecRight);
  stx_Dump4f(vecUp);
{LOG_FNLN;LOG_PRINT("setShaderConstant4x4f\n");LOG_PRINT("setShaderConstant4f\n");LOG_PRINT("setShaderConstant4f\n");}
	#endif
  #if 1
  
  IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
  IRenderer::GetRendererInstance()->setTexture("Sampler", m_pTextures[i]);

          
    VB.resize(Num * 4);
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		Num*4,
		Num*2,
		&IB[0],
		&IB[0],
		CONSTANT_INDEX2,
		&VB[0],
		&VB[0],
		sizeof(sShaderVertex), 0);

	#if 0
{LOG_FNLN;LOG_PRINT("setBlendState\n");LOG_PRINT("setDepthState\n");LOG_PRINT("setTexture\n");LOG_PRINT("DrawIndexedPrimitiveUP\n");}
	#endif

	#if 0
	LOG_FNLN;
	LOG_PRINT("m_pShader=%x\n", m_pShader);
  	LOG_PRINT("m_pDecl=%x\n", m_pDecl);
	LOG_PRINT("GetblendSrcAlpha()=%x\n", IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	LOG_PRINT("GetnoDepthTest()=%x\n", IRenderer::GetRendererInstance()->GetnoDepthTest());
	LOG_PRINT("m_pTextures[%d]=%x\n", i, m_pTextures[i]);
	LOG_PRINT("Num=%d\n", Num);
	LOG_PRINT("VB.size()=%d\n", VB.size());
	LOG_PRINT("IB.size()=%d\n", IB.size());
	#endif

          // Clear vertex count
          Num=0;
        }
      }

      // Go to next particle
      Particle = Particle->m_Next;
    }

    // Render any polygons left
    if(Num) {
	#if 0
{LOG_FNLN;LOG_PRINT("setShader\n");LOG_PRINT("setVertexFormat\n");}
	#endif
  // Set vertex shader, declaration, and stream sources
  IRenderer::GetRendererInstance()->setShader(m_pShader);
  IRenderer::GetRendererInstance()->setVertexFormat(m_pDecl);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant4f("right", vecRight);
  IRenderer::GetRendererInstance()->setShaderConstant4f("up", vecUp);

	#if 0

  stx_Dump4x4f(matVP);
  stx_Dump4f(vecRight);
  stx_Dump4f(vecUp);
{LOG_FNLN;LOG_PRINT("setShaderConstant4x4f\n");LOG_PRINT("setShaderConstant4f\n");LOG_PRINT("setShaderConstant4f\n");}
	#endif
  #if 1
  IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#endif
  IRenderer::GetRendererInstance()->setTexture("Sampler", m_pTextures[i]);

          VB.resize(Num * 4);
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		Num*4,
		Num*2,
		&IB[0],
		&IB[0],
		CONSTANT_INDEX2,
		&VB[0],
		&VB[0],
		sizeof(sShaderVertex), 0);

	#if 0
{LOG_FNLN;LOG_PRINT("setBlendState\n");LOG_PRINT("setDepthState\n");LOG_PRINT("setTexture\n");LOG_PRINT("DrawIndexedPrimitiveUP\n");}

	LOG_FNLN;
	
	LOG_PRINT("m_pShader=%x\n", m_pShader);
  	LOG_PRINT("m_pDecl=%x\n", m_pDecl);
	LOG_PRINT("GetblendSrcAlpha()=%x\n", IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  	LOG_PRINT("GetnoDepthTest()=%x\n", IRenderer::GetRendererInstance()->GetnoDepthTest());

	LOG_PRINT("m_pTextures[%d]=%x\n", i, m_pTextures[i]);
	LOG_PRINT("Num=%d\n", Num);
	LOG_PRINT("VB.size()=%d\n", VB.size());
	LOG_PRINT("IB.size()=%d\n", IB.size());
	#endif
	}
  }
}

void cParticleEmitter::Process(__WORD__ Elapsed)
{
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

    // Go to next particle
    Particle = NextParticle;
  }
}


////////////////////////////////////////////////////////////////////////////
//
// Specialized particle functions
//
////////////////////////////////////////////////////////////////////////////
void cParticleEmitter::HandleSmoke(D3DXFROMWINEVECTOR3 *vecPos, __WORD__ Elapsed)
{
  static __WORD__ Timer = 0;

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


