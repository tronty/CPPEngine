/*
  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "SDKmesh.h"

#if 1
#define LOG_FNLN
#define LOG_PRINT
#endif

#ifdef LINUX
#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#endif

    BYTE* m_pStaticMeshData=0;
    BYTE* m_pHeapData=0;
    BYTE* m_pAnimationData=0;
    BYTE** m_ppVertices=0;
    BYTE** m_ppIndices=0;

	std::vector<stx_Material> vt;
	std::vector<__WORD__> g_indices2;
	std::vector<stx_Mesh> mesh__;
	    
    void CDXUTSDKMesh::LoadMaterials(  _SDKMESH_MATERIAL* pMaterials, UINT numMaterials )
{
        for( UINT m = 0; m < numMaterials; m++ )
        {
        	STX_PRINT("pMaterials[%d].DiffuseTexture=%s\n", m, pMaterials[m].DiffuseTexture);
        	STX_PRINT("pMaterials[%d].NormalTexture=%s\n", m, pMaterials[m].NormalTexture);
                STX_PRINT("pMaterials[%d].SpecularTexture=%s\n", m, pMaterials[m].SpecularTexture);
        }
}

    struct _stx_VertexPositionNormalTexture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal;
        D3DXFROMWINEVECTOR3 BiNormal;
        D3DXFROMWINEVECTOR3 Tangent;
        D3DXFROMWINEVECTOR3 Color;
        D3DXFROMWINEVECTOR2 Tex_;
        D3DXFROMWINEVECTOR2 Tex;
        };

int CDXUTSDKMesh::CreateFromMemory( BYTE* pData, UINT DataBytes)
{
    int hr = E_FAIL;
    D3DXFROMWINEVECTOR3 lower; 
    D3DXFROMWINEVECTOR3 upper; 
    
    // Set outstanding resources to zero
    m_NumOutstandingResources = 0;
    {
        m_pHeapData = pData;
        m_pStaticMeshData = pData;
    }

    // Pointer fixup
    m_pMeshHeader = ( _SDKMESH_HEADER* )m_pStaticMeshData;
    m_pVertexBufferArray = ( _SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
    m_pIndexBufferArray = ( _SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
    m_pMeshArray = ( _SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
    m_pSubsetArray = ( _SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
    m_pFrameArray = ( _SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
    m_pMaterialArray = ( _SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );

    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }
#if 0
    // error condition
    if( m_pMeshHeader->Version != _SDKMESH_FILE_VERSION )
    {
        stx_exit(0);
    }
#endif
    // Setup buffer data pointer
    BYTE* pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;
    
    STX_PRINT("m_pMeshHeader->NumVertexBuffers=%d\n", m_pMeshHeader->NumVertexBuffers);
    STX_PRINT("m_pMeshHeader->NumIndexBuffers=%d\n", m_pMeshHeader->NumIndexBuffers);
    STX_PRINT("m_pMeshHeader->NumMaterials=%d\n", m_pMeshHeader->NumMaterials);
    STX_PRINT("m_pMeshHeader->NumFrames=%d\n", m_pMeshHeader->NumFrames);

    // Create VBs
    //m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        _stx_VertexPositionNormalTexture* pVertices = NULL;
        pVertices = ( _stx_VertexPositionNormalTexture* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

            //CreateVertexBuffer( &m_pVertexBufferArray[i], pVertices);

        STX_PRINT("pVertices = %x\n", pVertices);
        _SDKMESH_VERTEX_BUFFER_HEADER* pHeader=&m_pVertexBufferArray[i];
        STX_PRINT("NumVertices = %d\n", ( UINT )pHeader->NumVertices);
        STX_PRINT("SizeBytes = %d\n", ( UINT )pHeader->SizeBytes);
        STX_PRINT("StrideBytes = %d\n", ( UINT )pHeader->StrideBytes);
        STX_PRINT("sizeof(stx_VertexPositionNormalTexture) = %d\n", sizeof(stx_VertexPositionNormalTexture));
        STX_PRINT("sizeof(_stx_VertexPositionNormalTexture) = %d\n", sizeof(_stx_VertexPositionNormalTexture));
        STX_PRINT("sizeof(m_pVertexBufferArray[%d]) = %d\n", i, sizeof(m_pVertexBufferArray[i]));

        //m_ppVertices[i] = (BYTE*)pVertices;
        for( UINT ind = 0; ind < pHeader->NumVertices; ind++ )
    	{
		stx_VertexPositionNormalTexture v_;
		v_.Position=(pVertices+ind)->Position;
		v_.Normal=(pVertices+ind)->Normal;
		v_.Tex=(pVertices+ind)->Tex;
    		mesh__[0].vertices.push_back(v_);
    		//mesh__.vertices.push_back(v_);
    	}
    }
    // Create IBs
    //m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );

            //CreateIndexBuffer( &m_pIndexBufferArray[i], pIndices);

            
        unsigned int stride=sizeof(__WORD__);   
        if (m_pIndexBufferArray[i].IndexType == IT_16BIT ) {
            STX_PRINT("indsize = %d\n", 2);
        }else {
            STX_PRINT("indsize = %d\n", 4);
            stride=sizeof(__DWORD__);
        }

        STX_PRINT("pIndices = %x\n", pIndices);
        _SDKMESH_INDEX_BUFFER_HEADER* pHeader=&m_pIndexBufferArray[i];
        STX_PRINT("SizeBytes = %d\n", ( UINT )pHeader->SizeBytes);
        STX_PRINT("NumIndices = %d\n", ( UINT )pHeader->NumIndices);
        STX_PRINT("IndexType = %d\n", ( UINT )pHeader->IndexType);
        
        STX_PRINT("sizeof(m_pIndexBufferArray[%d]) = %d\n", i, sizeof(m_pIndexBufferArray[i]));

        //m_ppIndices[i] = pIndices;
        for( UINT ind = 0; ind < pHeader->NumIndices; ind++ )
    	{  
        	if (m_pIndexBufferArray[i].IndexType == IT_16BIT )
        	{
    			mesh__[0].indices.push_back(*((__WORD__*)pIndices+ind));
    			//mesh__.indices.push_back(*((__WORD__*)pIndices+ind));
    			g_indices2.push_back(*((__WORD__*)pIndices+ind));
    		}
    		else
        	{
    			mesh__[0].indices.push_back(*((__DWORD__*)pIndices+ind));
    			//mesh__.indices.push_back(*((__DWORD__*)pIndices+ind));
    			g_indices2.push_back(*((__DWORD__*)pIndices+ind));
    		}
    	}
    }

    // Load Materials
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials);
#if 0
    // Create a place to store our bind pose frame matrices
    m_pBindPoseFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pBindPoseFrameMatrices )
        stx_exit(0);

    // Create a place to store our transformed frame matrices
    m_pTransformedFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pTransformedFrameMatrices )
        stx_exit(0);
#endif
    if(0)for( UINT i = 0; i < m_pMeshHeader->NumFrames; i++ )
    {
    D3DXFROMWINEMATRIX m=m_pFrameArray[i].Matrix;
    stx_Dump4x4f(m, "m:\n");
    STX_PRINT("&m_pFrameArray[%d].Matrix=%x\n", i, &m_pFrameArray[i].Matrix);
    }

    hr = S_OK;
    return hr;
}

class _3DAnimation : public STXGUI {
public:
	_3DAnimation(){}
	virtual ~_3DAnimation(){}
	MeshRenderer2 m_Mesh;
	TextureID texID;
	virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	unsigned int i=0;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
	}
  }
}
	virtual void init(const char* aTitle="")
	{
		//STXGUI::init("/skeleton/GUILayout.xml");
			SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
			texID=IRenderer::GetRendererInstance()->addImageLibTexture("/MotionBlur/Warrior_Diff.png", false, ss);
		m_Mesh.Load("/MotionBlur/Warrior.x", eSimpleShader, true, true);
		m_Mesh.SetPlay(false);
		m_Mesh.SetAnimIndex(0);
		m_Mesh.SetPlay(true);
		size_t n=m_Mesh.GetNumAnimations();
		LOG_PRINT("NumAnimations=%d\n", n);
		LOG_PRINT("m_Mesh.vertices.size()=%d\n", m_Mesh.vertices.size());
		LOG_PRINT("m_Mesh.indices.size()=%d\n", m_Mesh.indices.size());
		if(m_Mesh.meshes.size())
		{
		LOG_PRINT("m_Mesh.meshes[0].vertices.size()=%d\n", m_Mesh.meshes[0].vertices.size());
		LOG_PRINT("m_Mesh.meshes[0].indices.size()=%d\n", m_Mesh.meshes[0].indices.size());
		}

	}
	virtual void render(void)
	{
STX_PRINT("21:\n");
	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX mvp, R, S, T;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
		stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
STX_PRINT("22:\n");
        m_Mesh.Draw(&mvp, texID);
STX_PRINT("23:\n");
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

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
STX_PRINT("24:\n");
	}
};

int ApplicationLogic()
{
STX_PRINT("1:\n");
	IRenderer* r=IRenderer::GetRendererInstance("SDKMesh");
STX_PRINT("2:\n");
	IInput*    i=STX_Service::GetInputInstance();
STX_PRINT("3:\n");
		stx_Mesh m;
STX_PRINT("4:\n");
		mesh__.push_back(m);
STX_PRINT("5:\n");
const char* filename[] =
{
	"/MotionBlur/armor.sdkmesh",
	"/MotionBlur/Helmet.sdkmesh",
	"/MotionBlur/rightshoulder.sdkmesh",
	"/MotionBlur/BackPlate.sdkmesh",
	"/MotionBlur/Leftshin.sdkmesh",
	"/MotionBlur/rightthigh.sdkmesh",
	"/MotionBlur/Belt.sdkmesh",
	"/MotionBlur/Leftshoulder.sdkmesh",
	"/MotionBlur/Warrior.sdkmesh",
	"/MotionBlur/Eyes.sdkmesh",
	"/MotionBlur/Leftthigh.sdkmesh",
	"/MotionBlur/windmillstage.sdkmesh",
	"/MotionBlur/Fan.sdkmesh",
	"/MotionBlur/rightforearm.sdkmesh",
	"/MotionBlur/Hammer.sdkmesh",
	"/MotionBlur/rightshin.sdkmesh"
};
STX_PRINT("6:\n");
for(unsigned int i=0;i<elementsOf(filename);i++)
{
	if(i!=8)
		continue;
	STX_PRINT("filename[%d]=%s\n", i, filename[i]);
	std::vector<BYTE> m_pStaticMeshData=STX_Service::ReadFile(filename[i]);
	CDXUTSDKMesh cm;
	int R=cm.CreateFromMemory( &m_pStaticMeshData[0], m_pStaticMeshData.size());
	 STX_PRINT("&m_pStaticMeshData[0]=%x\n", &m_pStaticMeshData[0]);
	 STX_PRINT("m_pStaticMeshData.size()=%d\n", m_pStaticMeshData.size());
	 
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
	stx_Material m("Warrior_Diff.png", vC);
	vt.push_back(m);
	stx_Materials_(vt, g_indices2, mesh__);
	stx_writeXFile("/MotionBlur/Warrior.x", mesh__);
}
STX_PRINT("7:\n");
	_3DAnimation app;
STX_PRINT("8:\n");
	app.init("");
STX_PRINT("9:\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
STX_PRINT("10:\n");
   	return 0;
}

