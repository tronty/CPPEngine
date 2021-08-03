//-----------------------------------------------------------------------------
// File: Billboard.cpp
//
// Desc: Example code showing how to do billboarding. The sample uses
//       billboarding to draw some trees.
//
//       Note: This implementation is for billboards that are fixed to rotate
//       about the Y-axis, which is good for things like trees. For
//       unconstrained billboards, like explosions in a flight sim, the
//       technique is the same, but the the billboards are positioned slightly
//       differently. Try using the inverse of the view matrix, TL-vertices, or
//       some other technique.
//
// Copyright (c) 1995-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

stx_SkyBox sb;

#define NUM_TREES 500

// Need global access to the eye direction used by the callback to sort trees
D3DXFROMWINEVECTOR3 g_vDir;

// Simple function to define "hilliness" for terrain
inline float HeightField( float x, float y )
{
    return 9*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

struct Tree
{
  D3DXFROMWINEVECTOR3 vecPos;     // Coordinates of particle
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  //D3DXFROMWINEVECTOR4       Diffuse;    // Diffuse color of particle
  D3DXFROMWINEVECTOR2 uv;       // Texture coordinates
};
std::vector<Tree> VB;
std::vector<__WORD__> IB;

D3DXFROMWINEMATRIX matProj, matView, MATRIX_PROJECTION, MATRIX_VIEW, MATRIX_WORLD;

             // Font for drawing text
    MeshRenderer2     m_pTerrain;           // Terrain object
    ShaderID shdT = -1;
    VertexFormatID vfT = -1;
    TextureID m_pTerrainTexture = -1;
    //MeshRenderer2    m_pSkyBox;            // Skybox background object

ShaderID       m_pShader   = -1;
VertexFormatID m_pDecl     = -1;
TextureID texID=-1;
    //D3DXFROMWINEMATRIX    m_matBillboardMatrix; // Used for billboard orientation

    int DrawBackground();
    int DrawTrees();
    int Render();
    int FrameMove();

#define NUMTREETEXTURES 6

int OneTimeSceneInit()
{
    // Initialize the tree data
    for( __WORD__ i=0; i<NUM_TREES; i++ )
    {
        // Position the trees randomly
        float fTheta  = 2.0f*D3DXFROMWINE_PI*(float)stx_rand()/RAND_MAX;
        float fRadius = 25.0f + 55.0f * (float)stx_rand()/RAND_MAX;
	D3DXFROMWINEVECTOR3 p;
        p.x  = fRadius * sinf(fTheta);
        p.z  = fRadius * cosf(fTheta);
        p.y  = HeightField( p.x, p.z );

        // Size the trees randomly
        float fWidth  = 1.0f + 0.2f * (float)(stx_rand()-stx_rand())/RAND_MAX;
        float fHeight = 1.4f + 0.4f * (float)(stx_rand()-stx_rand())/RAND_MAX;

        // Each tree is a random color between red and green
        __WORD__ r = ((255-190) + (__WORD__)(190*(float)(stx_rand())/RAND_MAX))/255.0f;
        __WORD__ g = ((255-190) + (__WORD__)(190*(float)(stx_rand())/RAND_MAX))/255.0f;
        __WORD__ b = 0;
        //__WORD__ dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);
	D3DXFROMWINEVECTOR3 dwColor(r,g,b);
	float HalfSize = 2.0f;

	unsigned int ATLAS_TEXTURE_SIZE=4096;

        // Pick a random texture for the tree
        unsigned int subtextureN = (__WORD__)( ( NUMTREETEXTURES * stx_rand() ) / (float)RAND_MAX );
	unsigned int iatlassize=ATLAS_TEXTURE_SIZE;
	unsigned int isubtexturesize=256;
	int ix = subtextureN / (iatlassize/isubtexturesize);
	int iy = subtextureN-ix*(iatlassize/isubtexturesize);
	float fx = 0.25f;
	float fy = 0.25f;

	Tree tree;
        tree.vecPos    = p;
        tree.vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, HalfSize);
        //tree.Diffuse   = Particle->m_Color;
        tree.uv         = D3DXFROMWINEVECTOR2((ix)*fx,(iy)*fy);
	VB.push_back(tree);
        tree.vecPos    = p;
        tree.vecOffset = D3DXFROMWINEVECTOR2(HalfSize, HalfSize);
        //tree.Diffuse   = Particle->m_Color;
        tree.uv         = D3DXFROMWINEVECTOR2((ix+1)*fx,(iy)*fy);
	VB.push_back(tree);
        tree.vecPos    = p;
        tree.vecOffset = D3DXFROMWINEVECTOR2(-HalfSize, -HalfSize);
        //tree.Diffuse   = Particle->m_Color;
        tree.uv         = D3DXFROMWINEVECTOR2((ix)*fx,(iy+1)*fy);
	VB.push_back(tree);
        tree.vecPos    = p;
        tree.vecOffset = D3DXFROMWINEVECTOR2(HalfSize, -HalfSize);
        //treev.Diffuse   = Particle->m_Color;
        tree.uv         = D3DXFROMWINEVECTOR2((ix+1)*fx,(iy+1)*fy);
	VB.push_back(tree);

    IB.push_back((VB.size()-4) * 4 + 0);
    IB.push_back((VB.size()-4) * 4 + 1);
    IB.push_back((VB.size()-4) * 4 + 2);
    IB.push_back((VB.size()-4) * 4 + 3);
    IB.push_back((VB.size()-4) * 4 + 2);
    IB.push_back((VB.size()-4) * 4 + 1);
    }

    return 0;
}

bool TreeSortCB(const Tree & p1, const Tree &p2)
{
    float d1 = p1.vecPos.x * g_vDir.x + p1.vecPos.z * g_vDir.z;
    float d2 = p2.vecPos.x * g_vDir.x + p2.vecPos.z * g_vDir.z;
    if (d1 < d2)
        return true;

    return false;
}

int FrameMove()
{
	float m_fTime = (float)timeGetTime();
    // Get the eye and lookat points from the camera's path
    D3DXFROMWINEVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXFROMWINEVECTOR3 vEyePt;
    D3DXFROMWINEVECTOR3 vLookatPt;

    vEyePt.x = 30.0f*cosf( 0.8f * ( m_fTime ) );
    vEyePt.z = 30.0f*sinf( 0.8f * ( m_fTime ) );
    vEyePt.y = 4 + HeightField( vEyePt.x, vEyePt.z );

    vLookatPt.x = 30.0f*cosf( 0.8f * ( m_fTime + 0.5f ) );
    vLookatPt.z = 30.0f*sinf( 0.8f * ( m_fTime + 0.5f ) );
    vLookatPt.y = vEyePt.y - 1.0f;

    // Set the app view matrix for normal viewing
    D3DXFROMWINEMATRIX matView;
    D3DXFROMWINEMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    //D3DXFROMWINEMatrixIdentity(&matView);
    MATRIX_VIEW=matView;

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXFROMWINEVECTOR3 vDir = vLookatPt - vEyePt;
/*
    if( vDir.x > 0.0f )
        D3DXFROMWINEMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DXFROMWINE_PI/2 );
    else
        D3DXFROMWINEMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DXFROMWINE_PI/2 );
*/
    // Sort trees in back-to-front order
    g_vDir = vDir;
    //qsort( VB, NUM_TREES, sizeof(Tree), TreeSortCB );
	//std::sort(VB.begin(), VB.end(), TreeSortCB);

    return 0;
}

int DrawTrees()
{
    //for(__WORD__ i=0; i<NUM_TREES; i++ )
    //{
/*
        // Translate the billboard into place
        m_matBillboardMatrix.index(3,0) = VB[i].vecPos.x;
        m_matBillboardMatrix.index(3,1) = VB[i].vecPos.y;
        m_matBillboardMatrix.index(3,2) = VB[i].vecPos.z;
        MATRIX_WORLD=m_matBillboardMatrix;

	D3DXFROMWINEMATRIX MVP=MATRIX_WORLD*MATRIX_VIEW*MATRIX_PROJECTION;
    D3DXFROMWINEMatrixTranspose(&MVP, &MVP);
    */

	D3DXFROMWINEMATRIX matVP;
	D3DXFROMWINEVECTOR4 vecRight, vecUp;
#if 1
  matVP = (MATRIX_VIEW) * (MATRIX_PROJECTION);
  D3DXFROMWINEMatrixTranspose(&matVP, &matVP);
#else
  matVP = (matProj)*(matView);
#endif

  // Get right and up vectors from view transformation and store as constants
  D3DXFROMWINEVECTOR4 vr(MATRIX_VIEW.m[0][0], MATRIX_VIEW.m[1][0], MATRIX_VIEW.m[2][0], 0.0f);
  D3DXFROMWINEVECTOR4 vu(MATRIX_VIEW.m[0][1], MATRIX_VIEW.m[1][1], MATRIX_VIEW.m[2][1], 0.0f);

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
		VB.size()/2,
		&IB[0],
		&IB[0],
		CONSTANT_INDEX2,
		&VB[0],
		&VB[0],
		sizeof(Tree), iDrawFlag);

    //}
    // Restore state
    D3DXFROMWINEMATRIX  matWorld;
    D3DXFROMWINEMatrixIdentity( &matWorld );
    MATRIX_WORLD=matWorld;
/*
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ALPHATESTENABLE,    false );
    IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ALPHABLENDENABLE,   false );
*/
    return 0;
}

void render()
{
	FrameMove();
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	//LOG_PRINT("BeginScene\n");
        // Render the Skybox
        {
            // Center view matrix for skybox and disable zbuffer
            D3DXFROMWINEMATRIX matView, matViewSave;
            matViewSave=MATRIX_VIEW;
            matView = matViewSave;
            matView.index(3,0) = 0.0f; matView.index(3,1) = -0.3f; matView.index(3,2) = 0.0f;
            //MATRIX_VIEW=matView;
/*
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, false );
            // Some cards do not disable writing to Z when
            // D3DRS_ZENABLE is false. So do it explicitly
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZWRITEENABLE, false );
*/
            // Render the skybox
            if(1)
	    {
		//LOG_FNLN;
		D3DXFROMWINEMATRIX mvp=MATRIX_VIEW*MATRIX_PROJECTION;
	//LOG_PRINT("m_pSkyBox.render(mvp);\n");
		//m_pSkyBox.render(&mvp);
        sb.render();
	    }
            // Restore the render states
            //MATRIX_VIEW=matViewSave;
/*
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, true );
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZWRITEENABLE, true);
*/
        }

	//DrawBackground();

        // Draw the terrain
            if(1)
	{
    D3DXFROMWINEMATRIX I;
    D3DXFROMWINEMatrixIdentity(&I);
		D3DXFROMWINEMATRIX mvp=MATRIX_VIEW*MATRIX_PROJECTION;
		D3DXFROMWINEMATRIX mvp_=MATRIX_PROJECTION*MATRIX_VIEW;

D3DXFROMWINEMATRIX MVP=MATRIX_VIEW*MATRIX_PROJECTION;
//D3DXFROMWINEMatrixTranspose(&MVP, &MVP);
	//LOG_PRINT("m_pTerrain.Draw(mvp, m_pTerrainTexture);\n");
		//m_pTerrain.Draw(&mvp, m_pTerrainTexture);
    #if 1
        m_pTerrain.render(&mvp);
    #else
      IRenderer::GetRendererInstance()->setVertexFormat(vfT);
  		IRenderer::GetRendererInstance()->setShader(shdT);
  		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mvp);
  		IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", m_pTerrainTexture);
      IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0,
        m_pTerrain.vertices.size(), m_pTerrain.indices.size()/3, &m_pTerrain.indices[0], &m_pTerrain.indices[0],
        CONSTANT_INDEX2, &m_pTerrain.vertices[0], &m_pTerrain.vertices[0], sizeof(stx_VertexPositionNormalTexture));
    #endif
	}

        // Draw the trees
        DrawTrees();

	//LOG_PRINT("EndScene\n");
	IRenderer::GetRendererInstance()->EndScene();
	//LOG_PRINT("Present\n");
	IRenderer::GetRendererInstance()->Present( );
}

int init(const char* aTitle)
{
    // Set the transform matrices (view and world are updated per frame)
    float fAspect = float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight();
    D3DXFROMWINEMatrixPerspectiveFovLH( &matProj, D3DXFROMWINE_PI/4, fAspect, 1.0f, 100.0f );
    //D3DXFROMWINEMatrixIdentity(&matProj);
    MATRIX_PROJECTION=matProj;
/*
    // Copy tree mesh data into vertexbuffer
	m_pTreeVB.resize(NUM_TREES*4);
    TREEVERTEX* v=(TREEVERTEX*)&m_pTreeVB[0];
    int iTree;
    __WORD__ dwOffset = 0;
    for( iTree = 0; iTree < NUM_TREES; iTree++ )
    {
        stx_memcpy( &v[dwOffset], VB[iTree].v, 4*sizeof(TREEVERTEX) );
        VB[iTree].dwOffset = dwOffset;
        dwOffset += 4;
    } */

if(-1==m_pShader) {
	m_pShader=IRenderer::GetRendererInstance()->addShaderFromFile("/ParticleVS/Particles3.shd", "main", "main");
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
 }
        SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	//SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();

  // Load textures if none already loaded
  if(texID==-1) {
	if(!stx_fileExists("/Billboard_atlas.png"))
	{
    //m_pTextures = new TextureID[NUM_PARTICLE_TYPES];
			const char* filename[] =
			{
                                "/dx8sdk/tree01S.png",
                                "/dx8sdk/tree02S.png",
                                "/dx8sdk/tree35S.png",
                                "/textures/ATI_SDK/grass001.png",
                                "/textures/ATI_SDK/grass002.png",
                                "/textures/NVIDIA_Corporation/2d/Palm.png",
				"/AdvAniCh12/Particle_Smoke.png",
				"/Orcs/Firetex.png",
				"/DeferredParticles/DeferredParticle.png",
				"/12_Explosion_Particle_Effect/explosion.png",
                                "/AdvAniCh12/Particle_People1.png",
                                "/AdvAniCh12/Particle_People2.png",
	"/textures/NVIDIA_Corporation/1D/fire_gradient3.png",
	"/textures/NVIDIA_Corporation/1D/FireGrade.png",
	//"/textures/ATI_SDK/Spectrum.png",
	//"/Smoke/FireTransferFunction.png",
	//"/Smoke/fireBase.png",
	"/textures/NVIDIA_Corporation/2d/flame.png",
	"/new/R2VB_VTF/R2VB-Sort/FirePalette.png",
	//"/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds",// black ??? TRILINEAR, CLAMP
			};

	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filename);i++)
		av.push_back(filename[i]);

	//subtextureN=elementsOf(filename);

	Image3 atlas;
	stx_CreateAtlas(atlas, av);
	texID=IRenderer::GetRendererInstance()->addTexture(&atlas, false, ss, 0);
	if(0)
	{
		std::string fn=stx_convertpath("/Billboard_atlas.png");
		//atlas.compress();
		atlas.saveImageLibImage(fn.c_str(), false);
	}
	}
	else
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/Billboard_atlas.png", false, ss);

  }
	//m_pSkyBox.Load("/Billboard/skybox2.x", eSimpleShader);
    //sb.init("Billboard", "bmp");
    sb.init("/SkyBox/cubemaps/x/OlympusGL.dds");
    #if 1
	     //m_pTerrain.CreateMesh("/Billboard/seafloor.x");
         m_pTerrain.Load("/Billboard/seafloor.x", eSimpleShader, true, true);
         // Add some "hilliness" to the terrain
           D3DXFROMWINEMATRIX S;
           D3DXFROMWINEMatrixScaling(&S, 200.0f, 1.0f, 200.0f);
         for( unsigned int i = 0; i < m_pTerrain.meshes[0].vertices.size();i++)
         {
 				      D3DXFROMWINEVec3TransformCoord(&m_pTerrain.meshes[0].vertices[i].Position, &m_pTerrain.meshes[0].vertices[i].Position, &S);
              m_pTerrain.meshes[0].vertices[i].Position.y = HeightField( m_pTerrain.meshes[0].vertices[i].Position.x, m_pTerrain.meshes[0].vertices[i].Position.z );
         }
    #else
    {
    shdT = IRenderer::GetRendererInstance()->addShaderFromFile("/MeshRenderer2/rest.HLSL.shd", "main", "main");
  FormatDesc format[] =
      {
        0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
        0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
        0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
        0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
        0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
        0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
      };
  vfT = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shdT);
}
  __WORD__ SIZE_X=10;
  __WORD__ SIZE_Z=10;
  __WORD__ COUNT=SIZE_X * SIZE_Z;
  LOG_PRINT("COUNT=%d\n", COUNT);
  LOG_PRINT("SIZE_X=%d\n", SIZE_X);
  LOG_PRINT("SIZE_Z=%d\n", SIZE_Z);
  m_pTerrain.vertices.resize(4*COUNT);
  m_pTerrain.indices.resize(6*COUNT);
  for (__WORD__ i = 0; i < COUNT; i++){
  		m_pTerrain.indices[6 * i + 0] = 4 * i;
  		m_pTerrain.indices[6 * i + 1] = 4 * i + 1;
  		m_pTerrain.indices[6 * i + 2] = 4 * i + 2;
  		m_pTerrain.indices[6 * i + 3] = 4 * i;
  		m_pTerrain.indices[6 * i + 4] = 4 * i + 2;
  		m_pTerrain.indices[6 * i + 5] = 4 * i + 3;
  	}
    for (int z = -SIZE_Z; z < SIZE_Z; z++)
    {
  		for (int x = -SIZE_X; z < SIZE_X; x++)
      {
        __WORD__  i=SIZE_X*(z+SIZE_Z)+(x+SIZE_X);
    			float tx = 100.0f*(float(x+SIZE_X) / (SIZE_X - 1) * 2 - 1);
      		float tz = 100.0f*(float(z+SIZE_Z) / (SIZE_Z - 1) * 2 - 1);
          LOG_PRINT("i=%d\n", i);
          LOG_PRINT("tx=%f\n", tx);
          LOG_PRINT("tz=%f\n", tz);
          break;
        m_pTerrain.vertices[i].Position.x = tx;
        m_pTerrain.vertices[i].Position.z = tz;
        // Add some "hilliness" to the terrain
        m_pTerrain.vertices[i].Position.y = HeightField( m_pTerrain.vertices[i].Position.x, m_pTerrain.vertices[i].Position.z );
  		}
  	}
	   m_pTerrainTexture = IRenderer::GetRendererInstance()->addImageLibTexture("/Billboard/seafloor.bmp", false, ss);
     #endif
#if 1
    	D3DXFROMWINEVECTOR3 min_( 1e10f, 1e10f, 1e10f);
    	D3DXFROMWINEVECTOR3 max_( -1e10f, -1e10f, -1e10f);
    	for( unsigned int i = 0; i < m_pTerrain.meshes[0].vertices.size();i++)
    	{
    		min_.x = std::min( min_.x, m_pTerrain.meshes[0].vertices[i].Position.x);
    		min_.y = std::min( min_.y, m_pTerrain.meshes[0].vertices[i].Position.y);
    		min_.z = std::min( min_.z, m_pTerrain.meshes[0].vertices[i].Position.z);
    		max_.x = std::max( max_.x, m_pTerrain.meshes[0].vertices[i].Position.x);
    		max_.y = std::max( max_.y, m_pTerrain.meshes[0].vertices[i].Position.y);
    		max_.z = std::max( max_.z, m_pTerrain.meshes[0].vertices[i].Position.z);
    	}
      LOG_PRINT("m_pTerrain.vertices.size()=%d\n", m_pTerrain.vertices.size());
      LOG_PRINT("m_pTerrain.indices.size()=%d\n", m_pTerrain.indices.size());
  		LOG_PRINT("m_pTerrain.meshes[0].vertices.size()=%d\n", m_pTerrain.meshes[0].vertices.size());
  		LOG_PRINT("m_pTerrain.meshes[0].indices.size()=%d\n", m_pTerrain.meshes[0].indices.size());
      LOG_PRINT("min.x=%f\n", min_.x);
      LOG_PRINT("min.y=%f\n", min_.y);
      LOG_PRINT("min.z=%f\n", min_.z);
      LOG_PRINT("max.x=%f\n", max_.x);
      LOG_PRINT("max.y=%f\n", max_.y);
      LOG_PRINT("max.z=%f\n", max_.z);
#endif
	OneTimeSceneInit();
	return 0;
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Billboard2: DX8 Billboarding Example");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		//stx_exit(0);
	}
	return 0;
}

