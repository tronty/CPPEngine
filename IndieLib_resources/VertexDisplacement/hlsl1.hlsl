// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
struct VsIn {
    float3 position	: POSITION;
    float3 normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In)
{
  VsOut Out=(VsOut)0;
  Out.uv = In.uv;
  Out.position = mul(worldViewProj, float4(In.position, 1.0));
  return Out;
}

[Fragment shader]
float4 main(VsOut IN): COLOR {

  // colour is RGBA: u, v, 0, 1
  float4 color = float4( float3( IN.uv, 0. ), 1. );
  return color;
}

