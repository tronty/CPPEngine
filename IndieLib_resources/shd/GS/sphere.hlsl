/*
//--------------------------------------------------------------------------------------
// Create an index buffer to be used for procedural sphere mesh rendering
//--------------------------------------------------------------------------------------
HRESULT CreateIndexBuffer( ID3D10Device* pd3dDevice, ID3D10Buffer **ppIndexBuffer )
{
    HRESULT hr;
    int IndexNumber = SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 6;

    unsigned *pIndices = new unsigned[IndexNumber];
    unsigned indexCount = 0;

    for( int i = 0; i < SPHERE_PARALLEL_SLICES_NUM; i++ )
    {
        for( int j = 0; j < SPHERE_MERIDIAN_SLICES_NUM; j++ )
        {
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;

            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
        }
    }

    D3D10_BUFFER_DESC desc;
    memset( &desc, 0, sizeof(D3D10_BUFFER_DESC) );
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.ByteWidth = sizeof (unsigned int) * IndexNumber;
    desc.BindFlags = D3D10_BIND_INDEX_BUFFER;

    D3D10_SUBRESOURCE_DATA data;
    data.pSysMem = pIndices;
        
    V_RETURN( pd3dDevice->CreateBuffer( &desc, &data, &g_pSphereIndexBuffer ) )

    if( pIndices )
    {
        delete [] pIndices;
        pIndices = NULL;
    }
    
    return S_OK;
}

*/
struct VSIn
{
    uint index : SV_VertexID;
};

struct PSIn
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
    float3 viewVectorTangent : TEXCOORD1;
    float3 lightVectorTangent : TEXCOORD2;
};
//--------------------------------------------------------------------------------------
// Calculate vertex positions for procedural sphere mesh based on an input index buffer
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn input)
{
    PSIn output;
    
    float meridianPart = ( input.index % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( input.index / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    
    float angle1 = meridianPart * 3.14159265 * 2.0;
    float angle2 = ( parallelPart - 0.5 ) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    float3 VertexPosition;
    VertexPosition.z = cos_angle1 * cos_angle2;
    VertexPosition.x = sin_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    
    output.position = mul( float4( VertexPosition, 1.0 ), g_ModelViewProj );
    output.texCoord = float2( 1.0 - meridianPart, 1.0 - parallelPart );
    
    float3 tangent = float3( cos_angle1, 0.0, -sin_angle1 );
    float3 binormal = float3( -sin_angle1 * sin_angle2, cos_angle2, -cos_angle1 * sin_angle2 );
        
    float3 viewVector = normalize(g_EyePosition - VertexPosition);
    
    output.viewVectorTangent.x = dot( viewVector, tangent );
    output.viewVectorTangent.y = dot( viewVector, binormal);
    output.viewVectorTangent.z = dot( viewVector, VertexPosition );
    
    float3 lightVector = normalize( g_LightPosition );
    
    output.lightVectorTangent.x = dot( lightVector, tangent );
    output.lightVectorTangent.y = dot( lightVector, binormal);
    output.lightVectorTangent.z = dot( lightVector, VertexPosition );
    
    return output;
}

