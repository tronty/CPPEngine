/******************************************************************************
File:  vnoise.glsl

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:

sgreen 5/02/02:

This is based on Perlin's original code:
    http://mrl.nyu.edu/~perlin/doc/oscar.html

    It combines the permutation and gradient tables into one array of
    float4's to conserve constant memory.
    The table is duplicated twice to avoid modulo operations.

jallen@nvidia.com: 10/12/03:

    GLSL version of Cg vertex noise shader

Notes:

    Should use separate tables for 1, 2 and 3D versions

******************************************************************************/

  float3 mod289(float3 x)
  {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
  }

  float4 mod289(float4 x)
  {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
  }

//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//
float4 permute(float4 x){return mod289(((x*34.0)+1.0)*x);}
float4 taylorInvSqrt(float4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(float3 v){ 
  const float2  C = float2(1.0/6.0, 1.0/3.0) ;
  const float4  D = float4(0.0, 0.5, 1.0, 2.0);

// First corner
  float3 i  = floor(v + dot(v, C.yyy) );
  float3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  float3 g = step(x0.yzx, x0.xyz);
  float3 l = 1.0 - g;
  float3 i1 = min( g.xyz, l.zxy );
  float3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  float3 x1 = x0 - i1 + 1.0 * C.xxx;
  float3 x2 = x0 - i2 + 2.0 * C.xxx;
  float3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod289(i); 
  float4 p = permute( permute( permute( 
             i.z + float4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + float4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + float4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  float3  ns = n_ * D.wyz - D.xzx;

  float4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  float4 x_ = floor(j * ns.z);
  float4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  float4 x = x_ *ns.x + ns.yyyy;
  float4 y = y_ *ns.x + ns.yyyy;
  float4 h = 1.0 - abs(x) - abs(y);

  float4 b0 = float4( x.xy, y.xy );
  float4 b1 = float4( x.zw, y.zw );

  float4 s0 = floor(b0)*2.0 + 1.0;
  float4 s1 = floor(b1)*2.0 + 1.0;
  float4 sh = -step(h, float4(0.0));

  float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  float3 p0 = float3(a0.xy,h.x);
  float3 p1 = float3(a0.zw,h.y);
  float3 p2 = float3(a1.xy,h.z);
  float3 p3 = float3(a1.zw,h.w);

//Normalise gradients
  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  float4 m = max(0.6 - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, float4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

#define ssnoise(x) ((2*snoise(x)*0.5+0.5)-1)
struct VsIn {
	float3 Vertex: POSITION;
	float3 Normal: NORMAL;
	float3 BiNormal: BINORMAL;
        float3 Tangent: TANGENT;
        float3 Color: TEXCOORD0;
        float2 Tex: TEXCOORD1;
};
struct VsOut {
	float4 Position: POSITION;
	float4 FrontColor: COLOR0;
};
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
float Displacement=1.0;
VsOut main(VsIn In)
{
	VsOut Out=(VsOut)0;
    float4 noisePos = mul(worldViewProj, float4(In.Vertex, 1.0));

    float i = (ssnoise(noisePos.xyz) + 1.0) * 0.5;
    Out.FrontColor = float4(i, i, i, 1.0);

    // displacement along normal
    float4 position = float4(In.Vertex, 1.0) + (float4(In.Normal, 1.0) * i * Displacement);
    position.w = 1.0;

    Out.Position = mul(worldViewProj, position);
	return Out;
}

[Fragment shader]

float4 main(VsOut IN): COLOR {
	return IN.FrontColor;
}

