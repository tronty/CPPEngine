#ifndef _SKINNED_MESH_H_
#define _SKINNED_MESH_H_
/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   SkinnedMesh.h
// Author: Ignacio Llamas
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA_Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// SkinnedMesh Interface and "factory"
//----------------------------------------------------------------------------------

#include <vector>
class SkinnedMesh;

class SkinnedMesh
{
public:
    typedef std::vector<std::string> stdStringVector;

    // Creation
    static SkinnedMesh *CreateD3D10FromColladaFile(std::string filename int maxSoBufNum, 
		bool useExternalEffect = false, EffectID* pEffect = 0, char pEffectPath[MAX_PATH]=0);
    
    // Rendering
    //   Instancing is used if "instanceCount" > 0
    virtual bool Render(float *pWorldMatrix, float *pViewMatrix, float *pProjectionMatrix,
        unsigned int  instanceCount = 0,
		bool useAlternativeTechnique = false, stx_Technique* SkinnedAlternativeTechnique = 0, 
		stx_Technique* RigidAlternativeTechnique = 0
		) = 0;
    
    virtual bool RenderToSO(float *pWorldMatrix, unsigned int dstSoBufNum) = 0;
    virtual bool RenderFromSO(unsigned int srcSoBufNum, stx_Technique *pTechnique, 
        unsigned int instanceCount = 0) = 0; 
    virtual bool RenderFromSO(VertexFormatID *pInputLayout, stx_Technique *pTechnique, 
        unsigned int instanceCount = 0) = 0;


    // Render filters: these are mutually excluse filters. They define a list of
    //   nodes to be rendered.
    //   Default: no filters, all nodes are rendered
    //   Exclude: all nodes except the ones with id matching any of those listed is rendered
    //   Include: only the nodes with id matching any of those listed is rendered
    virtual void SetRenderFilterExclude(stdStringVector *excludeNodes) = 0;
    virtual void SetRenderFilterInclude(stdStringVector *includeNodes) = 0;

    virtual exBufferID *GetSOBuffer(unsigned int soBufNum) = 0;

    // Animation controls
    enum ANIMATION_END_HANDLING
    {
        BLOCK,
        LOOP,
    };
    virtual void Update(float elapsedTime) = 0;
    virtual void SetStartEndTimes(float startTime, float endTime) = 0;
    virtual void GetStartEndTimes(float *outStartTime, float *outEndTime) = 0;
    virtual void SetCurrentTime(float currentTime) = 0;
    virtual void SetAnimationEndHandling(ANIMATION_END_HANDLING aeh) = 0;

    // Bounding Volumes
    virtual void GetBoundingSphere(float outCenter[3], float *outRadius, bool applyRenderFilters = true) = 0;
    virtual void GetAABB(float outAABBmin[3], float outAABBmax[3], bool applyRenderFilters = true) = 0;

    // Destructor
    virtual ~SkinnedMesh();

protected:
    virtual bool LoadFromColladaFile(std::string filename) = 0;
    SkinnedMesh();
};


//----------------------------------------------------------------------------------
// SkinnedMeshImpl: implements loading and animation
//----------------------------------------------------------------------------------

#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#undef //////////////SAFE_RELEASE
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsTools.h"
#include <string>
#include <list>
#include <vector>
#include <map>
class FCDSceneNode;
class FCDEntityInstance;
class FCDMaterialInstance;
class FCDMaterial;
class FCDImage;


class SkinnedMeshImpl : public SkinnedMesh
{
public:
    virtual ~SkinnedMeshImpl();

    // Animation Controls
    virtual void Update(float elapsedTime);
    virtual void SetStartEndTimes(float startTime, float endTime);
    virtual void GetStartEndTimes(float *outStartTime, float *outEndTime);
    virtual void SetCurrentTime(float currentTime);
    virtual void SetAnimationEndHandling(ANIMATION_END_HANDLING aeh);

    // Bounding Volumes
    virtual void GetBoundingSphere(float outCenter[3], float *outRadius, bool applyRenderFilters = true);
    virtual void GetAABB(float outAABBmin[3], float outAABBmax[3], bool applyRenderFilters = true);


protected:
    friend SkinnedMesh;

    SkinnedMeshImpl();

    bool LoadFromColladaFile(std::string filename);

    // Assumptions:
    //
    // Input is triangle mesh, with a given number of vertex, a number of attributes per vertex
    //  and a numer of triangles (a list of indices into the vertex arrays). 
    //
    // The mesh may contain multiple subsets of faces affected by a different material.
    //
    // Attributes may be Position, Normal, Tangent, Binormal/Bitangent, TexCoord#, Color#, 
    //  InfluenceWeight#, InfluenceIdx# (for Skinning), etc.
    //
    // Input also contains a hierarchy of scene nodes, each with a matrix xform that may be
    //  animated. If animated, it is stored as a list of matrices for different keyframes.
    //
    // Materials may contain multiple texture maps, such as diffuse, specular, normal.
    //
    // All the geometries in the geometry library are loaded into a common list of vertex attribute 
    //  arrays, such that there is only a single array for positions, a single array for normals, etc.
    //  So if a subset of geometry doesn't have a particular attribute that has been already created
    //  for other geometry that attribute array is padded with zeroes.
    
    // Custom data structures to store the data
    // ========================================
    struct Node;
    class VtxAttributeArray;

    typedef std::vector<VtxAttributeArray*> VtxAttrArPtrVector;
    
    typedef FMMatrix44 Matrix44;
    typedef std::map<float, Matrix44> FloatToMatrix44Map;

    // bounding sphere
    struct Sphere
    {
        FMVector3           center;
        float               radius;
    };

    // bounding box
    struct AABB
    {
        FMVector3           aabbMin;
        FMVector3           aabbMax;
    };

    struct Texture
    {
        std::string         id;
        fstring             fileName;
    };
    typedef std::list<Texture> TextureList;

    class BoundTexture
    {
    public:
        enum Semantic { DIFFUSE, SPECULAR, NORMAL, COUNT, UNKNOWN = -1 };
        BoundTexture(Texture *pTex, uint32 sem);
        Semantic            semantic;
        Texture             *pTexture;
    protected:
        BoundTexture() {}
    };
    typedef std::list<BoundTexture> BoundTextureList;

    struct Material
    {
        std::string         id;
        BoundTextureList    boundTextures;
    };
    typedef std::list<Material> MaterialList;

    struct MeshSubset
    {
        // begin and count within m_indices arrays
        uint32              idxBegin;
        uint32              idxCount;
        // mesh subsets within a Mesh may use different # of attributes
        VtxAttrArPtrVector  vtxAttributes;
        // mesh subsets will be bound at instantiation time to a given material
        fstring             materialSemantic;
    };
    typedef std::list<MeshSubset> MeshSubsetList;
    typedef std::list<MeshSubset*> MeshSubsetPtrList;
    typedef std::map<MeshSubset*,Material*> MsPtrToMatPtrMap;

    struct Mesh
    {
        std::string         id;
        // begin and count within the multiple equal-sized m_vtxAttributes arrays
        uint32              vtxBegin;
        uint32              vtxCount;
        MeshSubsetPtrList   subsets;
        bool                hasSkin;
        VtxAttrArPtrVector  vtxAttributes;  // all the vtx attributes used by all the mesh subsets
        FCDGeometryIndexTranslationMap oldIdxToNewIdxs;

        // bounding volumes (in Mesh object space)
        Sphere              boundingSphere;
        AABB                aabb;
    };
    typedef std::list<Mesh> MeshList;

    struct NodeBinding
    {
        std::string sid;
        Node        *node;
        Matrix44    invBindPose;
    };
    typedef std::vector<NodeBinding> NodeBindingVector;

    struct MeshInstance
    {
        Mesh                *pMesh;
        MsPtrToMatPtrMap    subsetToMaterial;
        NodeBindingVector   bones;
        Matrix44            meshBindPose;
        
        // bounding volumes (in World space, based on worldXForm of container Node)
        Sphere              boundingSphere;
        AABB                aabb;
    };
    typedef std::list<MeshInstance>MeshInstanceList;

    typedef std::list<Node*>NodePtrList;
    typedef std::list<Node> NodeList;
    struct Node
    {
        std::string         id;
        std::string         sid;
        Node                *pParent;
        NodePtrList         children;

        Matrix44            xform;      // the local xform, relative to the parent
        Matrix44            worldXForm; // the world xform, composed with the parent's xform

        // animation
        FloatToMatrix44Map  xformKeyFrames;
        float               minKeyTime;
        float               maxKeyTime;

        // for Rendering
        MeshInstanceList    instances;

        // bounding volumes (in World space (based on worldXForm), union for all contained mesh 'instances')
        Sphere              instances_boundingSphere;
        AABB                instances_aabb;

        void EvaluateLocalXForm(float timeValue);
    };

    class VtxAttributeArray
    {
    public:
        enum ScalarType { ST_FLOAT, ST_SINT32, ST_UINT32, ST_SNORM32, ST_UNORM32,
                            ST_SINT8, ST_UINT8, ST_SNORM8, ST_UNORM8, SCALAR_TYPE_COUNT };

        enum Semantic { POSITION, VERTEX, NORMAL, GEOTANGENT, GEOBINORMAL,
                        TEXCOORD, TEXTANGENT, TEXBINORMAL, UV, COLOR, EXTRA, // Maya-specific, used for blind data
                        BLENDINDICES0, BLENDINDICES1, BLENDWEIGHTS0, BLENDWEIGHTS1,
                        UNKNOWN = -1,
        };

        VtxAttributeArray(ScalarType type, uint32 compCount, 
            const std::string &strSemantic, Semantic semantic, uint32 semanticIdx = 0);
        virtual ~VtxAttributeArray();

        bool AppendData(uint32 compCount, uint32 scalarCount, void *srcData);

        float           * AsFloat(int idx = 0)  { return &(static_cast<float*>(m_pData))[idx*m_info.m_compCount]; }
        int32           * AsSInt32(int idx = 0) { return &(static_cast<int32*>(m_pData))[idx*m_info.m_compCount]; }
        uint32          * AsUInt32(int idx = 0) { return &(static_cast<uint32*>(m_pData))[idx*m_info.m_compCount]; }
        int8            * AsSInt8(int idx = 0)  { return &(static_cast<int8*>(m_pData))[idx*m_info.m_compCount]; }
        uint8           * AsUInt8(int idx = 0)  { return &(static_cast<uint8*>(m_pData))[idx*m_info.m_compCount]; }
        
        std::string         m_strSemantic;  // semantic string (to differentiate in case it's UNKNOWN)
        
        struct VtxAttributeArrayInfo
        {
            VtxAttributeArrayInfo( Semantic semantic,
                                   uint32 semanticIdx,
                                   ScalarType type,
                                   uint32 compCount );

            Semantic            m_semantic;     // semantic enumerant for well known semantics
            uint32              m_semIdx;       // semantic index, for attributes that need more than 4 floats
            ScalarType          m_type;         // data type of scalar value for each component
            uint32              m_scalarSize;   // number of bytes per scalar
            uint32              m_compCount;    // number of scalar components per attribute

        };

        struct VtxAttributeArrayInfoLess
        {
            bool operator() (const VtxAttributeArrayInfo &pLeft, const VtxAttributeArrayInfo &pRight) const;
        };

        VtxAttributeArrayInfo   m_info;

        uint32              m_count;        // number of attributes in this array

    protected:
        VtxAttributeArray();

        void                *m_pData;       // a pointer to the buffer containing the attribute data array

    };
    typedef std::list<VtxAttributeArray> VtxAttributeArrayList;
    //typedef std::map<VtxAttributeArray::Semantic, VtxAttributeArray *> SemanticToVtxAttrArMap;
    typedef std::map<VtxAttributeArray::VtxAttributeArrayInfo, VtxAttributeArray *,
        VtxAttributeArray::VtxAttributeArrayInfoLess> SemanticToVtxAttrArMap;
    typedef std::map<uint32, SkinnedMeshImpl::VtxAttributeArray::Semantic> FSemanticToSemanticMap;

    typedef std::vector<uint32> IndexBuffer;

    typedef std::map<std::string, Mesh*>        IdToMeshMap;
    typedef std::map<std::string, Material*>    IdToMaterialMap;
    typedef std::map<std::string, Texture*>     IdToTextureMap;

    
    
    // Data holders
    // ============

    // arrays of vertex attributes
    VtxAttributeArrayList   m_vtxAttributes;
    SemanticToVtxAttrArMap  m_semToVtxAttrAr;
    FSemanticToSemanticMap  m_fsemToSemMap;
    // index buffer
    IndexBuffer             m_indices;

    // mesh subsets associate a subset of mesh triangles with a material
    MeshSubsetList          m_meshSubsets;
    MeshList                m_meshes;
    IdToMeshMap             m_idToMesh;
    

    // library of textures
    TextureList             m_textures;
    IdToTextureMap          m_idToTexture;
    // library of materials: each material contain references to textures, shaders, etc
    MaterialList            m_materials;
    IdToMaterialMap         m_idToMaterial;

    // The nodes (includings bones/joints) with animation data
    NodeList                m_nodes;

    uint32                  m_vtxCount;
    uint32                  m_triCount;


    // Animation State
    //================
    ANIMATION_END_HANDLING  m_animEndHandling;  // how should we update m_currentTime when we reach the end
    float                   m_currentTime;  // the time after the last call to Update or SetCurrentTime
    float                   m_startTime;    // animation start time, may be set by the user
    float                   m_endTime;      // the animation end time, may be set by the user, 
                                            //   enforced to be greater or equal to startTime
    
    float                   m_minTime;      // minimum time across all keyframes in all the Node animations
    float                   m_maxTime;      // maximum time across all keyframes in all the Node animations
    float                   m_lastNodesUpdateTime;  // the last time value at which all nodes where updated

    // Bounding Volume state
    //  bounding sphere
    float                   m_lastBSphUpdateTime;
    Sphere                  m_boundingSphere;
    //  bounding box
    float                   m_lastAABBUpdateTime;
    AABB                    m_aabb;
    

    // Internal functions
    // ==================
    // Perform a mesh self-consistency check and return true if the mesh is self-consistent
    bool IsSelfConsistent(void);
    // Perform some Initialization
    void InitFsemToSemMap(void);

    Node * AddNode(FCDSceneNode *pSrcSceneNode, Node *pParentNode);
    bool ProcessNode(FCDSceneNode *pSrcSceneNode, Node *pParentNode);
    bool ProcessGeometryInstNode(FCDEntityInstance *pEntityInstance, Node *pDstNode);
    bool ProcessControllerInstNode(FCDEntityInstance *pEntityInstance, Node *pDstNode);
    Mesh * GetGeometry(FCDEntity *pEntity);
    Mesh * AddGeometry(FCDEntity *pEntity);
    void GenSubsetToMaterialMap(MsPtrToMatPtrMap &dstSubsetToMaterialMap, 
            const MeshSubsetPtrList &subsetPtrs, FCDEntityInstance *pGeomInst);
    Material * GetMaterial(FCDMaterialInstance *pMatInst);
    Material * AddMaterial(FCDMaterialInstance *pMatInst);
    Texture * GetTexture(FCDImage *pImage);
    Texture * AddTexture(FCDImage *pImage);

    void UpdateNodes(float timeValue);
    void UpdateNode(Node *pNode, float timeValue);

    void AABBUnion(AABB *outAABB, const AABB &inAABB1, const AABB &inAABB2);
    void BoundingSphereOfAABB(Sphere *outSphere, const AABB &inAABB);

    VtxAttributeArray * GetVtxAttributeArray(VtxAttributeArray::ScalarType type, uint32 compCount, 
        const std::string &strSemantic, VtxAttributeArray::Semantic semantic, uint32 semanticIdx = 0);
};


//----------------------------------------------------------------------------------
// SkinnedMeshD3D10 - rendering implementation for Direct3D 10
//----------------------------------------------------------------------------------

class SkinnedMeshD3D10 : public SkinnedMeshImpl
{
public:
    virtual ~SkinnedMeshD3D10();

    virtual bool Render(float *pWorldMatrix, float *pViewMatrix, float *pProjectionMatrix,
        unsigned int instanceCount,
		bool useAlternativeTechnique = false, stx_Technique* SkinnedAlternativeTechnique = 0, stx_Technique* RigidAlternativeTechnique=0);
    virtual bool RenderToSO(float *pWorldMatrix, unsigned int dstSoBufNum);
    virtual bool RenderFromSO(unsigned int srcSoBufNum, stx_Technique *pTechnique,
        unsigned int instanceCount);
    virtual bool RenderFromSO(VertexFormatID *pInputLayout, stx_Technique *pTechnique,
        unsigned int instanceCount);

    virtual void SetRenderFilterExclude(stdStringVector *excludeNodes);
    virtual void SetRenderFilterInclude(stdStringVector *includeNodes);

    virtual exBufferID *GetSOBuffer(unsigned int soBufNum);

	float* getWorldXFormMatrix( std::string id );
	bool reloadExternalEffect( EffectID* pEffect, char pEffectPath[MAX_PATH] );


    // A simple vertex type used in the streamout buffers
    struct SOVertex
    {
        float Pos[3];
        float Norm[3];
        float Tex[2];
    };

protected:
    // Constructor is protected to ensure the use SkinnedMesh::CreateD3D10FromColladaFile 
    SkinnedMeshD3D10();
    // SkinnedMesh must be friend so that it can create a SkinnedMeshD3D10
    friend SkinnedMesh;


    // Custom types for resource storage
    typedef std::vector<exBufferID*> ID3D10BufferVector;
    typedef std::vector<VertexFormatID*> ID3D10InputLayoutVector;
    typedef std::vector<TextureID*> ID3D10SRVVector;
    typedef std::vector<unsigned int> UINTVector;
    struct MeshSubsetD3D10
    {
        stx_Technique   **pTechnique;
        stx_Technique   **pSoTechnique;
        VertexFormatID       *pInputLayout;
        ID3D10BufferVector      vtxBuffers;
        UINTVector              strides;
        UINTVector              offsets;
    };
    typedef std::map<VtxAttributeArray*, exBufferID*> VtxAttrArToVtxBufMap;
    typedef std::map<MeshSubset*, MeshSubsetD3D10> MeshSubsetToMSD3D10Map;
    typedef std::map<Texture*, TextureID*> TextureToSRVMap;

    ID3D10Device                *m_pd3dDevice;

    exBufferID                *m_pIndexBuffer;
    ID3D10BufferVector          m_vtxBuffers;
    ID3D10InputLayoutVector     m_inputLayouts;

    ID3D10SRVVector             m_textureSRVs;

    ID3D10BufferVector          m_SOBuffers;
    VertexFormatID           *m_pSOInputLayout;
    VertexFormatID           *m_pTmpSOInputLayout;

    VtxAttrArToVtxBufMap        m_vtxAtrToVtxBuffers;
    MeshSubsetToMSD3D10Map      m_meshSubsetToD3D10;
    TextureToSRVMap             m_textureToSRV;

    EffectID                *m_pEffect;
    stx_Technique       *m_etSkinned;
    stx_Technique       *m_etRigid;
    stx_Technique       *m_pRigid_TC_Technique;    
    stx_Technique       *m_etSkinnedSO;
    stx_Technique       *m_etRigidSO;
    stx_Technique       *m_pRigidSO_TC_Technique;
    
    stx_Technique       *m_etFromSO;

    stx_Variable  *m_evWorldMatrix;
    stx_Variable  *m_evViewProjectionMatrix;
    stx_Variable  *m_evBoneMatrices;
    stx_Variable  *m_evEyeVector;

    stx_ShaderResourceVariableID *m_evTexture_diffuse;
    stx_ShaderResourceVariableID *m_evTexture_specular;
    stx_ShaderResourceVariableID *m_evTexture_normal;
    
    stx_Variable  *m_evUseNormalMap;

    unsigned int                m_instanceCount;

    // For rendering from/to Streamout 
    bool                        m_IsRenderingToSO;
    bool                        m_IsRenderingFromSO;
    unsigned int                m_dstSoBuffer;
    unsigned int                m_srcSoBuffer;

    // For filtering of scene graph nodes
    stdStringVector             *m_pIncludeNodes;
    stdStringVector             *m_pExcludeNodes;

    // Internal functions
    // ==================

    DXGI_FORMAT DXGIFormatFromVtxAttr(VtxAttributeArray *pVtxAtAr);

	virtual bool InitializeForRendering( int maxSoBufNum, bool useExternalEffect = false, EffectID* pEffect = 0, char pEffectPath[MAX_PATH]=0 );
    virtual bool SetRenderMatrices(float *pWorldMatrix, float *pViewMatrix, float *pProjectionMatrix);
    virtual bool RenderFromSO(stx_Technique *pTechnique, unsigned int instanceCount);
    virtual bool RenderRootNodes(float *pWorldMatrix, bool useAlternativeTechnique =false, stx_Technique* SkinnedAlternativeTechnique=0, stx_Technique* RigidAlternativeTechnique=0);
    virtual bool RenderNode(Node *pNode, float *pWorldMatrix, bool useAlternativeTechnique, stx_Technique* SkinnedAlternativeTechnique, stx_Technique* RigidAlternativeTechnique);

public:
    static int LoadTextureFromFile( std::string file, TextureID** pOutTexSRV);

    static bool SkinnedMeshD3D10::VtxAttrSemanticLess(VtxAttributeArray *pLeft, VtxAttributeArray *pRight);
};

#endif // _SKINNED_MESH_H_

