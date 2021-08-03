
//----------------------------------------------------------------------------------
// File:   PerlinFire.fx
// Author: Andrew Tatarinov
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
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

const int nSamples = 2;
// Textures

sampler2D   SceneTexture;
sampler2D   ScreenDepth;
sampler2D   FireShape;
sampler2D   JitterTexture;

//Texture2DArray ShadowMap;
sampler2D ShadowMap;

//Texture2D<uint>
sampler2D PermTexture;   // permutation texture


//sampler2D   FireTex;//???

// Some variables
// Matrices and vectors

float3 EyePos;   // object-space eye position
float4 LightPos; // global-space light position
float Time;     // global simulation time
float LightIntensity = 0;   // for flickering light
float FrequencyWeights[5];


// Fire params

float StepSize = 0;
float NoiseScale=1.35;
float Roughness=3.20;
bool bJitter=true;
/*
// Texture samples

SamplerState SamplerClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState SamplerRepeat
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
*/

// Transformation matrices

ROW_MAJOR float4x4 WorldViewProj MVPSEMANTIC;

float4x4 CubeViewMatrices[6];

float4x4 CubeProjectionMatrix;

int CubeMapFace;

struct VolumeVertex
{
    float4   ClipPos      : POSITION;
    float3   Pos         : TEXCOORD0;      // vertex position in model space
    float3   RayDir      : TEXCOORD1;   // ray direction in model space
};

float and31(float x)
{
	return frac(x/pow(2, 5))*pow(2, 5);
}

// Find which simplex we are in by magnitude sorting

void Simplex3D( const in float3 P, out float3 simplex0, out float3 simplex1, out float3 simplex2, out float3 simplex3 )
{
    float3 T = P.xzy >= P.yxz;
    simplex0 = 0;
    simplex1 = T.xzy > T.yxz;
    simplex2 = T.yxz <= T.xzy;
    simplex3 = 1;
}

void Simplex4D( const in float4 P, out float4 simplex0, out float4 simplex1, out float4 simplex2, out float4 simplex3, out float4 simplex4 )
{
    float4 offset0;

    float3 isX = step( P.yzw, P.xxx );        // See comments in 3D simplex function
    offset0.x = dot( isX, float3(1, 1, 1) );
    offset0.yzw = 1 - isX;

    float2 isY = step( P.zw, P.yy );
    offset0.y += dot( isY, float2(1, 1) );
    offset0.zw += 1 - isY;

    float isZ = step( P.w, P.z );
    offset0.z += isZ;
    offset0.w += 1 - isZ;

    // offset0 now contains the unique values 0,1,2,3 in each channel

    simplex4 = 1;
    simplex3 = saturate (   offset0);
    simplex2 = saturate (-- offset0);
    simplex1 = saturate (-- offset0);
    simplex0 = 0;
}

// Hashing functions

int Hash( float3 P )
{
    //P /= 256;   // normalize texture coordinate
    //return PermTexture.SampleLevel( SamplerRepeat, P.xy, 0 ).r /* ^ */ + PermTexture.SampleLevel( SamplerRepeat, float2( P.z, 0 ), 0 ).r;
	//return tex2D(PermTexture, int2(P.xy, 0) ).r /* ^ */ + tex2D(PermTexture, int2( P.z, 0 ) ).r;
	return tex2D(PermTexture, P.xy).r /* ^ */ + tex2D(PermTexture, int2( P.z, 0 ) ).r;
}

int Hash( float4 P )
{
    //P /= 256;   // normalize texture coordinate
    //return PermTexture.SampleLevel( SamplerRepeat, P.xy, 0 ).r /* ^ */ + PermTexture.SampleLevel( SamplerRepeat, P.zw, 0 ).r;
	//return tex2D(PermTexture, int2(P.xy, 0) ).r /* ^ */ + tex2D(PermTexture, int2(P.zw, 0) ).r;
	return tex2D(PermTexture, P.xy).r /* ^ */ + tex2D(PermTexture, P.zw ).r;

    //return PermTexture.SampleLevel( SamplerRepeat,
    //    float2(PermTexture.SampleLevel( SamplerRepeat, P.xy, 0 ).r, P.z) + P.w, 0 ).r;
}

// gradient tables

//cbuffer Gradients

    // 3D gradients
    
    const float3 Grad3[] = {
        float3( -1, -1, 0 ), // 0
        float3( -1, +1, 0 ), // 1
        float3( +1, -1, 0 ), // 2
        float3( +1, +1, 0 ), // 3

        float3( -1, 0, -1 ), // 4
        float3( -1, 0, +1 ), // 5
        float3( +1, 0, -1 ), // 6
        float3( +1, 0, +1 ), // 7

        float3( 0, -1, -1 ), // 8
        float3( 0, -1, +1 ), // 9
        float3( 0, +1, -1 ), // 10
        float3( 0, +1, +1 ), // 11

        // padding   
        float3( +1, +1, 0 ), // 12
        float3( -1, -1, 0 ), // 13
        float3( 0, -1, +1 ), // 14
        float3( 0, -1, -1 )  // 15
    };

    // 4D case is more regular

    const float4 Grad4[] = {
        // x, y, z
        float4( -1, -1, -1, 0 ), // 0
        float4( -1, -1, +1, 0 ), // 1
        float4( -1, +1, -1, 0 ), // 2
        float4( -1, +1, +1, 0 ), // 3
        float4( +1, -1, -1, 0 ), // 4
        float4( +1, -1, +1, 0 ), // 5
        float4( +1, +1, -1, 0 ), // 6
        float4( +1, +1, +1, 0 ), // 7
        // w, x, y
        float4( -1, -1, 0, -1 ), // 8
        float4( -1, +1, 0, -1 ), // 9
        float4( +1, -1, 0, -1 ), // 10
        float4( +1, +1, 0, -1 ), // 11
        float4( -1, -1, 0, +1 ), // 12
        float4( -1, +1, 0, +1 ), // 13
        float4( +1, -1, 0, +1 ), // 14
        float4( +1, +1, 0, +1 ), // 15
        // z, w, x
        float4( -1, 0, -1, -1 ), // 16
        float4( +1, 0, -1, -1 ), // 17
        float4( -1, 0, -1, +1 ), // 18
        float4( +1, 0, -1, +1 ), // 19
        float4( -1, 0, +1, -1 ), // 20
        float4( +1, 0, +1, -1 ), // 21
        float4( -1, 0, +1, +1 ), // 22
        float4( +1, 0, +1, +1 ), // 23
        // y, z, w
        float4( 0, -1, -1, -1 ), // 24
        float4( 0, -1, -1, +1 ), // 25
        float4( 0, -1, +1, -1 ), // 26
        float4( 0, -1, +1, +1 ), // 27
        float4( 0, +1, -1, -1 ), // 28
        float4( 0, +1, -1, +1 ), // 29
        float4( 0, +1, +1, -1 ), // 30
        float4( 0, +1, +1, +1 ) // 31
    };

// Basic 3D simplex noise

#define F3 0.333333333333
#define G3 0.166666666667

float Snoise3D( float3 P )
{
    // Skew the (x,y,z) space to determine which cell of 6 simplices we're in

    float s = dot( P, F3 );
    float3 Pi = floor( P + s );
    float t = dot( Pi, G3 );

    float3 P0 = Pi - t;               // Unskew the cell origin back to (x,y,z) space
    float3 Pf0 = P - P0;            // The x,y,z distances from the cell origin

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // To find out which of the six possible tetrahedra we're in, we need to
    // determine the magnitude ordering of x, y and z components of Pf0.

    float3 simplex0;
    float3 simplex1;
    float3 simplex2;
    float3 simplex3;
    Simplex3D( Pf0, simplex0, simplex1, simplex2, simplex3);


    float n = 0;

    //[unroll]
    for ( int i = 0; i<4; i++ )
    {
        float3 Pf = Pf0 - simplex0 + G3 * i;
        int h = Hash( Pi + simplex0 );
	if(i==1)
	{
		Pf = Pf0 - simplex1 + G3 * i;
        	h = Hash( Pi + simplex1 );
	} else if(i==2) {
		Pf = Pf0 - simplex2 + G3 * i;
        	h = Hash( Pi + simplex2 );
	} else if(i==3) {
		Pf = Pf0 - simplex3 + G3 * i;
        	h = Hash( Pi + simplex3 );
	}

        float d = saturate( 0.6f - dot( Pf, Pf ) );
        d *= d; 
	float3 f3=Grad3[ and31(h) ];
        n += d * d * dot(f3, Pf);
        //n += d * d * grad( Pf, h );
    }

    return 32.0 * n;
}

// Simplex flow noise - rotate base gradient vectors

float Snoise3DFlow( float4 P )
{
    // Skew the (x,y,z) space to determine which cell of 6 simplices we're in

    float s = dot( P.xyz, F3 );
    float3 Pi = floor( P.xyz + s );
    float t = dot( Pi, G3 );

    float3 P0 = Pi - t;               // Unskew the cell origin back to (x,y,z) space
    float3 Pf0 = P.xyz - P0;         // The x,y,z distances from the cell origin

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // To find out which of the six possible tetrahedra we're in, we need to
    // determine the magnitude ordering of x, y and z components of Pf0.

    float3 simplex0;
    float3 simplex1;
    float3 simplex2;
    float3 simplex3;
    Simplex3D( Pf0, simplex0, simplex1, simplex2, simplex3);

    float n = 0;
    float sine, cosine;
    sincos( P.w, sine, cosine );

    //[unroll]
    for ( int i = 0; i<4; i++ )
    {
        float3 Pf = Pf0 - simplex0 + G3 * i;
        int h = Hash( Pi + simplex0 );
	if(i==1)
	{
		Pf = Pf0 - simplex1 + G3 * i;
        	h = Hash( Pi + simplex1 );
	} else if(i==2) {
		Pf = Pf0 - simplex2 + G3 * i;
        	h = Hash( Pi + simplex2 );
	} else if(i==3) {
		Pf = Pf0 - simplex3 + G3 * i;
        	h = Hash( Pi + simplex3 );
	}

        float d = saturate( 0.6f - dot( Pf, Pf ) );
        d *= d;
        float3 g = Grad3[ and31(h) ];
        g.xz = float2(g.x * cosine - g.z * sine, g.x * sine + g.z * cosine );   // rotate gradient vector
        n += d * d * dot( g, Pf );
    }

    return 32.0 * n;
}

// 4D simplex noise

#define F4 0.309016994375
#define G4 0.138196601125

float Snoise4D( float4 P )
{
    // Skew the (x,y,z) space to determine which cell of 6 simplices we're in

    float s = dot( P, F4 );
    float4 Pi = floor( P + s );
    float t = dot( Pi, G4 );

    float4 P0 = Pi - t;
    float4 Pf0 = P - P0;

    // For the 4D case, the simplex shape is a slightly irregular tetrahedron.
    // To find out which of the 24 possible tetrahedra we're in, we need to
    // determine the magnitude ordering of x, y, z and w components of Pf0.

    float4 simplex0;
    float4 simplex1;
    float4 simplex2;
    float4 simplex3;
    float4 simplex4;
    Simplex4D( Pf0, simplex0, simplex1, simplex2, simplex3, simplex4 );

    float n = 0;

    //[unroll]
    for ( int i = 0; i<5; i++ )
    {
        float4 Pf = Pf0 - simplex0 + G4 * i;
        int h = Hash( Pi + simplex0 );
	if(i==1)
	{
		Pf = Pf0 - simplex1 + G4 * i;
        	h = Hash( Pi + simplex1 );
	} else if(i==2) {
		Pf = Pf0 - simplex2 + G4 * i;
        	h = Hash( Pi + simplex2 );
	} else if(i==3) {
		Pf = Pf0 - simplex3 + G4 * i;
        	h = Hash( Pi + simplex3 );
	} else if(i==4) {
		Pf = Pf0 - simplex4 + G4 * i;
        	h = Hash( Pi + simplex4 );
	}
        float d = saturate( 0.6f - dot( Pf, Pf ) );
        d *= d;
	float4 f4=Grad4[ and31(h) ];
        n += d * d * dot( f4, Pf );
    }

    // Sum up and scale to fit [-1..1] range

    return 32.0 * n;   // FIXME: should this constant be 27.0?
}


// Turbulence function for 3D simplex noise
float Turbulence3D( float3 p )
{
    float res = 0;

    //[loop] // forcing the loop results in better register allocation overall
    for ( int i = 0; i<5; i++, p *= 2 )
        res += FrequencyWeights[i] * Snoise3D( p );

    return res;
}

// turbulence function for 3D flow noise
float Turbulence3DFlow( float4 p )
{
    float res = 0;

    //[loop]  // forcing the loop results in better register allocation overall
    for ( int i = 0; i<5; i++, p *= 2 )
        res += FrequencyWeights[i] * Snoise3DFlow( p );

    return res;
}

// Turbulence function for 4D simplex noise
float Turbulence4D( float4 p )
{
    float res = 0;

    //[unroll]    // unrolling the loop in 4D case gives better results
    for ( int i = 0; i<5; i++, p *= 2 )
        res += FrequencyWeights[i] * Snoise4D( p );

    return res;
}

[Vertex shader]

struct appdata {
    float3 Pos		: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 UV		: TEXCOORD1;
};

// Vertex Shader for perlin fire effect (common for all techniques)

VolumeVertex PerlinFireVS( appdata IN )
{
    VolumeVertex Out;

    Out.ClipPos = mul( float4( IN.Pos.x, IN.Pos.y, IN.Pos.z, 1 ), WorldViewProj );

    Out.RayDir = IN.Pos - EyePos;
    Out.Pos = IN.Pos;   // supposed to have range -0.5 ... 0.5

    return Out;
}
[Fragment shader]
// Perlin fire based on 3D noise

float4 PerlinFire3DPS(VolumeVertex In) : COLOR
{
	//return float4(0, 1, 0, 1);
    float3 Dir = normalize(In.RayDir) * StepSize;
    float Offset = bJitter ? tex2D(JitterTexture, In.ClipPos.xy / 256.0 ).r : 0;

    // Jitter initial position
    float3 Pos = In.Pos + Dir * Offset;

    float3 resultColor = 0;
    float SceneZ = tex2D( ScreenDepth, In.ClipPos.xy );

    for(int i=0; i<nSamples; i++)
    {
        //float3 Pos = In.Pos + Dir * Offset + Dir * i;
        float4 ClipPos = mul( float4( Pos, 1 ), WorldViewProj );
        ClipPos.z /= ClipPos.w;

        // Break out of the loop if there's a blocking occluder or we're outside the fire volume
        //if ( ClipPos.z > SceneZ || any( abs( Pos ) > 0.5 ))
        //    break;

        float3 NoiseCoord = Pos;
        NoiseCoord.y -= Time;

        // Evaluate turbulence function
        float Turbulence = abs( Turbulence3D( NoiseCoord * NoiseScale ) );

        float2 tc;
        tc.x = length( Pos.xz ) * 2;
        tc.y = 0.5 - Pos.y - Roughness * Turbulence * pow( ( 0.5 + Pos.y ), 0.5 );

        resultColor += StepSize * 12 * tex2D(FireShape, tc);

        Pos += Dir;
    }

	//return float4(1,0,0,1);
    return float4(resultColor, 1);
}

// Perlin fire based on 3D flow noise

float4 PerlinFire3DFlowPS(VolumeVertex In) : COLOR
{
	//return float4(0, 1, 0, 1);
    float3 Dir = normalize(In.RayDir) * StepSize;
    float Offset = bJitter ? tex2D(JitterTexture, In.ClipPos.xy / 256.0 ).r : 0;

    // Jitter initial position
    float3 Pos = In.Pos + Dir * Offset;

    float3 resultColor = 0;
    float SceneZ = tex2D( ScreenDepth, In.ClipPos.xy );

    for(int i=0; i<nSamples; i++)
    {
        float4 ClipPos = mul( float4( Pos, 1 ), WorldViewProj );
        ClipPos.z /= ClipPos.w;

        // Break out of the loop if there's a blocking occluder
        //if ( ClipPos.z > SceneZ || any( abs( Pos ) > 0.5 ) )
        //    break;

        float4 NoiseCoord;
        NoiseCoord.xyz = Pos;
        NoiseCoord.y -= Time;
        NoiseCoord.w = Time * 2.0;

        // Evaluate turbulence function
        float Turbulence = abs( Turbulence3DFlow( NoiseCoord * NoiseScale ) );

        float2 tc;
        tc.x = length( Pos.xz ) * 2;
        tc.y = 0.5 - Pos.y - Roughness * Turbulence * pow( ( 0.5 + Pos.y ), 0.5 );

        resultColor += StepSize * 12 * tex2D( FireShape, tc);

        Pos += Dir;
    }

	//return float4(1,0,0,1);
    return float4(resultColor, 1);
}


// Perlin fire based on 4D noise

float4 PerlinFire4DPS(VolumeVertex In) : COLOR
{
	//return float4(0, 1, 0, 1);
    float3 Dir = normalize(In.RayDir) * StepSize;
    float Offset = bJitter ? tex2D(JitterTexture, In.ClipPos.xy / 256.0).r : 0;

    // Jitter initial position
    float3 Pos = In.Pos + Dir * Offset;

    float3 resultColor = 0;
    float SceneZ = tex2D( ScreenDepth, In.ClipPos.xy );

    for(int i=0; i<nSamples; i++)
    {
        float4 ClipPos = mul( float4( Pos, 1 ), WorldViewProj );
        ClipPos.z /= ClipPos.w;

        // Break out of the loop if there's a blocking occluder
        //if ( ClipPos.z > SceneZ || any( abs( Pos ) > 0.5 ) )
        //    break;

        float4 NoiseCoord;
        NoiseCoord.xyz = Pos;
        NoiseCoord.y -= Time;
        NoiseCoord.w = Time * 0.5;

        // Evaluate turbulence function
        float Turbulence = abs( Turbulence4D( NoiseCoord * NoiseScale ) );

        float2 tc;
        tc.x = length( Pos.xz ) * 2;
        tc.y = 0.5 - Pos.y - Roughness * Turbulence * pow( ( 0.5 + Pos.y ), 0.5 );

        resultColor += StepSize * 12 * tex2D( FireShape, tc );

        Pos += Dir;
    }

	//return float4(1,0,0,1);
    return float4(resultColor, 1);
}
/*
struct MeshVertex
{
    float4   ClipPos : POSITION;
    float3   Pos : TEXCOORD0;
    float3   Normal : TEXCOORD1;
    float2   TexCoord : TEXCOORD2;
};

struct AppVertex
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct AppVertexShadow
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// Vertex Shader for shadows
// Since we use this shader only for cumputing shadows, we dont need to compute
// its color and normal

float4 ShadowVS( AppVertexShadow input ) : POSITION
{
    float4 ClipPos = float4( input.Pos.x, In.Pos.y, In.Pos.z, 1.0 );
    
    // This can be done more efficiently by multiplying matrices in the app but I'm just lazy :)

    ClipPos = mul( ClipPos, CubeViewMatrices );
    ClipPos = mul( ClipPos, CubeProjectionMatrix );

    return ClipPos;
}

// Simple pass-through vertex Shader

MeshVertex SimpleVS( AppVertex In )
{
    MeshVertex Out;

    Out.ClipPos = mul( float4( In.Pos.x, In.Pos.y, In.Pos.z, 1 ), WorldViewProj );
    Out.Pos = In.Pos;
    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;

    return Out;
}

// Pixel Shader

float4 SimplePS( MeshVertex In ) : COLOR
{
	//return float4(0, 1, 0, 1);
    float3 lightDir = normalize( LightPos - In.Pos );
    float3 lightCol = saturate( dot( lightDir, In.Normal ) );
    float4 shadeColor = float4( 1.0, 1.0, 1.0, 1.0 );
    float4 realPosLight = float4( In.Pos - LightPos, 1.0 );
  
    float maxCoord = max( abs( realPosLight.x ), max( abs( realPosLight.y ), abs( realPosLight.z ) ) );
    
    float4 diffuseColor = tex2D (SceneTexture, In.TexCoord);

    // the math is: -1.0 / maxCoord * (Zn * Zf / (Zf - Zn) + Zf / (Zf - Zn) (should match the shadow projection float4x4)
    float projectedDepth = -(1.0 / maxCoord) * (200.0 * 0.2 / (200.0 - 0.2)) + (200.0 / (200.0 - 0.2));

    float shadow = 1.0;// ??? projectedDepth - 0.001f > tex2D(ShadowMap, In.Pos - LightPos ).r ? 1 : 0;

    // Compute the final color

    shadeColor -= float4( float3( 0.6f, 0.6f, 0.6f ), 1.0 ) * shadow;

	//return float4(1,0,0,1);    
    return (diffuseColor * float4( lightCol, 1.0 ) * shadeColor * LightIntensity * pow( saturate( 150.0 / length( LightPos - In.Pos ) ), 3.0 ));
}
*/


