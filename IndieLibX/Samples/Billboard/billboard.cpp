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
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
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

// Custom vertex type for the trees
struct TREEVERTEX
{
    D3DXFROMWINEVECTOR3 p;      // Vertex position
    D3DXFROMWINEVECTOR3 color;  // Vertex color
    D3DXFROMWINEVECTOR2 uv; // Vertex texture coordinates
};

// Tree textures to use
char* g_strTreeTextures[] =
{
    "/dx8sdk/tree02S.png",
    "/dx8sdk/tree35S.png",
    "/dx8sdk/tree01S.png",
};

#define NUMTREETEXTURES 3

struct Tree
{
    TREEVERTEX  v[4];           // Four corners of billboard quad
    D3DXFROMWINEVECTOR3 vPos;           // Origin of tree
    __WORD__       dwTreeTexture;  // Which texture map to use
    __WORD__       dwOffset;       // Offset into vertex buffer of tree's vertices
};

D3DXFROMWINEMATRIX matProj, matView, MATRIX_PROJECTION, MATRIX_VIEW, MATRIX_WORLD;

             // Font for drawing text
    MeshRenderer2     m_pTerrain;           // Terrain object
    ShaderID shdT = -1;
    VertexFormatID vfT = -1;
    TextureID m_pTerrainTexture = -1;
    //MeshRenderer2    m_pSkyBox;            // Skybox background object

    ShaderID m_pTreeSHD=-1;
    VertexFormatID m_pTreeVF=-1;
    std::vector<TREEVERTEX> m_pTreeVB;  // Vertex buffer for rendering a tree
    TextureID      m_pTreeTextures[NUMTREETEXTURES]; // Tree images
    D3DXFROMWINEMATRIX    m_matBillboardMatrix; // Used for billboard orientation
    std::vector<Tree>          m_Trees(NUM_TREES);   // Array of tree info

    int DrawBackground();
    int DrawTrees();
    int Render();
    int FrameMove();

int OneTimeSceneInit()
{
    // Initialize the tree data
    for( __WORD__ i=0; i<NUM_TREES; i++ )
    {
        // Position the trees randomly
        float fTheta  = 2.0f*D3DXFROMWINE_PI*(float)stx_rand()/RAND_MAX;
        float fRadius = 25.0f + 55.0f * (float)stx_rand()/RAND_MAX;
        m_Trees[i].vPos.x  = fRadius * sinf(fTheta);
        m_Trees[i].vPos.z  = fRadius * cosf(fTheta);
        m_Trees[i].vPos.y  = HeightField( m_Trees[i].vPos.x, m_Trees[i].vPos.z );

        // Size the trees randomly
        float fWidth  = 1.0f + 0.2f * (float)(stx_rand()-stx_rand())/RAND_MAX;
        float fHeight = 1.4f + 0.4f * (float)(stx_rand()-stx_rand())/RAND_MAX;

        // Each tree is a random color between red and green
        __WORD__ r = ((255-190) + (__WORD__)(190*(float)(stx_rand())/RAND_MAX))/255.0f;
        __WORD__ g = ((255-190) + (__WORD__)(190*(float)(stx_rand())/RAND_MAX))/255.0f;
        __WORD__ b = 0;
        //__WORD__ dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);
	D3DXFROMWINEVECTOR3 dwColor(r,g,b);
#if 1
        m_Trees[i].v[0].p     = D3DXFROMWINEVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_Trees[i].v[0].color = dwColor;
        m_Trees[i].v[0].uv    = D3DXFROMWINEVECTOR2(1.0f, 1.0f);
        m_Trees[i].v[1].p     = D3DXFROMWINEVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[1].color = dwColor;
        m_Trees[i].v[1].uv    = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
        m_Trees[i].v[2].p     = D3DXFROMWINEVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_Trees[i].v[2].color = dwColor;
        m_Trees[i].v[2].uv    = D3DXFROMWINEVECTOR2(0.0f, 1.0f);
        m_Trees[i].v[3].p     = D3DXFROMWINEVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[3].color = dwColor;
        m_Trees[i].v[3].uv    = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
#else
        m_Trees[i].v[0].p     = D3DXFROMWINEVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_Trees[i].v[0].color = dwColor;
        m_Trees[i].v[0].uv    = D3DXFROMWINEVECTOR2(0.0f, 1.0f);
        m_Trees[i].v[1].p     = D3DXFROMWINEVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[1].color = dwColor;
        m_Trees[i].v[1].uv    = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
        m_Trees[i].v[2].p     = D3DXFROMWINEVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_Trees[i].v[2].color = dwColor;
        m_Trees[i].v[2].uv    = D3DXFROMWINEVECTOR2(1.0f, 1.0f);
        m_Trees[i].v[3].p     = D3DXFROMWINEVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[3].color = dwColor;
        m_Trees[i].v[3].uv    = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
#endif
        // Pick a random texture for the tree
        m_Trees[i].dwTreeTexture = (__WORD__)( ( NUMTREETEXTURES * stx_rand() ) / (float)RAND_MAX );
    }

    return 0;
}

bool TreeSortCB(const Tree & p1, const Tree &p2)
{
    float d1 = p1.vPos.x * g_vDir.x + p1.vPos.z * g_vDir.z;
    float d2 = p2.vPos.x * g_vDir.x + p2.vPos.z * g_vDir.z;
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
    if( vDir.x > 0.0f )
        D3DXFROMWINEMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DXFROMWINE_PI/2 );
    else
        D3DXFROMWINEMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DXFROMWINE_PI/2 );

    // Sort trees in back-to-front order
    g_vDir = vDir;
    //qsort( m_Trees, NUM_TREES, sizeof(Tree), TreeSortCB );
	std::sort(m_Trees.begin(), m_Trees.end(), TreeSortCB);

    return 0;
}

int DrawTrees()
{
    for( __WORD__ i=0; i<NUM_TREES; i++ )
    {
    // Loop through and render all trees
	IRenderer::GetRendererInstance()->setShader(m_pTreeSHD);
	IRenderer::GetRendererInstance()->setVertexFormat(m_pTreeVF);
  IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
        // Set the tree texture
        IRenderer::GetRendererInstance()->setTexture( "Sampler", m_pTreeTextures[m_Trees[i].dwTreeTexture] );

        // Translate the billboard into place
        m_matBillboardMatrix.index(3,0) = m_Trees[i].vPos.x;
        m_matBillboardMatrix.index(3,1) = m_Trees[i].vPos.y;
        m_matBillboardMatrix.index(3,2) = m_Trees[i].vPos.z;
        MATRIX_WORLD=m_matBillboardMatrix;

	D3DXFROMWINEMATRIX MVP=MATRIX_WORLD*MATRIX_VIEW*MATRIX_PROJECTION;
    D3DXFROMWINEMatrixTranspose(&MVP, &MVP);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", MVP);

	//
	//LOG_PRINT("DrawPrimitiveUP\n");
        // Render the billboard
        IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &m_Trees[i].v[0].p.x, &m_Trees[i].v[0].p.x, sizeof(TREEVERTEX), 0);

    }
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
		//
		D3DXFROMWINEMATRIX mvp=MATRIX_VIEW*MATRIX_PROJECTION;
	//LOG_PRINT("m_pSkyBox.render(mvp);\n");
        sb.render();
	    }

      //m_pSkyBox.render(&mvp);
            // Restore the render states
            //MATRIX_VIEW=matViewSave;
/*
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZENABLE, true );
            IRenderer::GetRendererInstance()->SetRenderState( D3DRS_ZWRITEENABLE, true);
*/
        }

	//DrawBackground();

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
// Draw the terrain

int init(const char* aTitle)
{
    // Set the transform matrices (view and world are updated per frame)
    float fAspect = float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight();
    D3DXFROMWINEMatrixPerspectiveFovLH( &matProj, D3DXFROMWINE_PI/4, fAspect, 0.001f, 100000.0f );
    //D3DXFROMWINEMatrixIdentity(&matProj);
    MATRIX_PROJECTION=matProj;

    m_pTreeVB.clear();

    for( __WORD__ i=0; i<NUMTREETEXTURES; i++ )
        m_pTreeTextures[i] = 0;

    SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
    // Create the tree textures
    for( __WORD__ i=0; i<NUMTREETEXTURES; i++ )
    	m_pTreeTextures[i]=IRenderer::GetRendererInstance()->addImageLibTexture(g_strTreeTextures[i], false, ss);

    // Copy tree mesh data into vertexbuffer
	m_pTreeVB.resize(NUM_TREES*4);
    TREEVERTEX* v=(TREEVERTEX*)&m_pTreeVB[0];
    int iTree;
    __WORD__ dwOffset = 0;
    for( iTree = 0; iTree < NUM_TREES; iTree++ )
    {
        stx_memcpy( &v[dwOffset], m_Trees[iTree].v, 4*sizeof(TREEVERTEX) );
        m_Trees[iTree].dwOffset = dwOffset;
        dwOffset += 4;
    }

	m_pTreeSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/Billboard/billboard.shd", "main", "main");

	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	m_pTreeVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), m_pTreeSHD);

	//m_pSkyBox.Load("/Billboard/skybox2.x", eSimpleShader);
    sb.init("Billboard", "bmp");
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
	IRenderer* r=IRenderer::GetRendererInstance("Billboard: DX8 Billboarding Example");
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

