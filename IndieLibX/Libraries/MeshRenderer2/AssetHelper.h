/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2012, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/


#if (!defined AV_ASSET_HELPER_H_INCLUDED)
#define AV_ASSET_HELPER_H_INCLUDED

#include <assimp/scene.h>

namespace AssimpView {

    class SceneAnimator;

    //-------------------------------------------------------------------------------
    /** \brief Class to wrap ASSIMP's asset output structures
    */
    //-------------------------------------------------------------------------------
    class AssetHelper
    {
    public:
        enum
        {
            // the original normal set will be used
            ORIGINAL = 0x0u,

            // a smoothed normal set will be used
            SMOOTH = 0x1u,

            // a hard normal set will be used
            HARD = 0x2u,
        };

        // default constructor
        AssetHelper()
            : iNormalSet( ORIGINAL )
        {
            mAnimator = NULL;
            apcMeshes = NULL;
            pcScene = NULL;
        }

        //---------------------------------------------------------------
        // default vertex data structure
        // (even if tangents, bitangents or normals aren't
        // required by the shader they will be committed to the GPU)
        //---------------------------------------------------------------
        struct Vertex
        {
            aiVector3D vPosition;
            aiVector3D vNormal;

            D3DXFROMWINECOLOR dColorDiffuse;
            aiVector3D vTangent;
            aiVector3D vBitangent;
            aiVector2D vTextureUV;
            aiVector2D vTextureUV2;
            unsigned char mBoneIndices[ 4 ];
            unsigned char mBoneWeights[ 4 ]; // last Weight not used, calculated inside the vertex shader
#if 0
            /** Returns the vertex declaration elements to create a declaration from. */
            static D3DVERTEXELEMENT9* GetDeclarationElements()
            {
                static D3DVERTEXELEMENT9 decl[] =
                {
                    { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
                    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
                    { 0, 24, D3DDECLTYPE_D3DFROMWINECOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
                    { 0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
                    { 0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
                    { 0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
                    { 0, 60, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
                    { 0, 68, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
                    { 0, 72, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
                    D3DDECL_END()
                };
                return decl;
            }
#endif
        };

        //---------------------------------------------------------------
        // FVF vertex structure used for normals
        //---------------------------------------------------------------
        struct LineVertex
        {
            aiVector3D vPosition;
            __DWORD__ dColorDiffuse;

            // retrieves the FVF code of the vertex type
            static __DWORD__ GetFVF()
            {
                return D3DFVF_DIFFUSE | D3DFVF_XYZ;
            }
        };

        //---------------------------------------------------------------
        // Helper class to store GPU related resources created for
        // a given aiMesh
        //---------------------------------------------------------------
        class MeshHelper
        {
        public:

            MeshHelper()
                :
                eShadingMode(),
                piVB( -1 ),
                piIB( -1 ),
                piVBNormals( -1 ),
                piEffect( -1 ),
                bSharedFX( false ),
                piDiffuseTexture( -1 ),
                piSpecularTexture( -1 ),
                piAmbientTexture( -1 ),
                piEmissiveTexture( -1 ),
                piNormalTexture( -1 ),
                piOpacityTexture( -1 ),
                piShininessTexture( -1 ),
                piLightmapTexture( -1 ),
                fOpacity(),
                fShininess(),
                fSpecularStrength(),
                twosided( false ),
                pvOriginalNormals( 0 )
            {}

            ~MeshHelper()
            {
                // NOTE: This is done in DeleteAssetData()
                // TODO: Make this a proper d'tor
            }

            // shading mode to use. Either Lambert or otherwise phong
            // will be used in every case
            aiShadingMode eShadingMode;

            // vertex buffer
            VertexBufferID piVB;

            // index buffer. For partially transparent meshes
            // created with dynamic usage to be able to update
            // the buffer contents quickly
            IndexBufferID piIB;

            // vertex buffer to be used to draw vertex normals
            // (vertex normals are generated in every case)
            VertexBufferID piVBNormals;

            // shader to be used
            EffectID piEffect;
            bool bSharedFX;

            // material textures
            TextureID piDiffuseTexture;
            TextureID piSpecularTexture;
            TextureID piAmbientTexture;
            TextureID piEmissiveTexture;
            TextureID piNormalTexture;
            TextureID piOpacityTexture;
            TextureID piShininessTexture;
            TextureID piLightmapTexture;

            // material colors
            D3DXFROMWINEVECTOR4 vDiffuseColor;
            D3DXFROMWINEVECTOR4 vSpecularColor;
            D3DXFROMWINEVECTOR4 vAmbientColor;
            D3DXFROMWINEVECTOR4 vEmissiveColor;

            // opacity for the material
            float fOpacity;

            // shininess for the material
            float fShininess;

            // strength of the specular highlight
            float fSpecularStrength;

            // two-sided?
            bool twosided;

            // Stores a pointer to the original normal set of the asset
            aiVector3D* pvOriginalNormals;
        };

        // One instance per aiMesh in the globally loaded asset
        MeshHelper** apcMeshes;

        // Scene wrapper instance
        aiScene* pcScene;

        // Animation player to animate the scene if necessary
        SceneAnimator* mAnimator;

        // Specifies the normal set to be used
        unsigned int iNormalSet;

        // ------------------------------------------------------------------
        // set the normal set to be used
        void SetNormalSet( unsigned int iSet );

        // ------------------------------------------------------------------
        // flip all normal vectors
        void FlipNormals();
        void FlipNormalsInt();
    };
}

#endif // !! IG
