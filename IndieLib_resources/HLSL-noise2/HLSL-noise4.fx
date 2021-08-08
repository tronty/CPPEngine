float noiseScale
<
    string UIWidget = "slider";
    string UIName = "noise scale";
    float UIMin = 0.0; float UIMax = 20.0; float UIStep = 0.01;
> = 5.0;

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

#define B  32      // table size
#define B2 66      // B*2 + 2
#define BR 0.03125 // 1 / B

// this is the smoothstep function f(t) = 3t^2 - 2t^3, without the normalization
float3 s_curve(float3 t)
{
    return t*t*( float3(3.0, 3.0, 3.0) - float3(2.0, 2.0, 2.0)*t);
}

float2 s_curve(float2 t)
{
    return t*t*( float2(3.0, 3.0) - float2(2.0, 2.0)*t);
}

float s_curve(float t)
{
    return t*t*(3.0-2.0*t);
}

//float4 pg[B2];            // permutation/gradient table

// 3D version
float noise333(float3 v, const float4 pg[B2])
{
    v = v + float3(10000.0, 10000.0, 10000.0);   // hack to avoid negative numbers

    float3 i = frac(v * BR) * float(B);   // index between 0 and B-1
    float3 f = frac(v);            // fractional position

    // lookup in permutation table
    float2 p;
    p.x = pg[ int(i[0])     ].w;
    p.y = pg[ int(i[0]) + 1 ].w;
    p = p + i[1];

    float4 b;
    b.x = pg[ int(p[0]) ].w;
    b.y = pg[ int(p[1]) ].w;
    b.z = pg[ int(p[0]) + 1 ].w;
    b.w = pg[ int(p[1]) + 1 ].w;
    b = b + i[2];

    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ int(b[0]) ].xyz, f );
    r[1] = dot( pg[ int(b[1]) ].xyz, f - float3(1.0, 0.0, 0.0) );
    r[2] = dot( pg[ int(b[2]) ].xyz, f - float3(0.0, 1.0, 0.0) );
    r[3] = dot( pg[ int(b[3]) ].xyz, f - float3(1.0, 1.0, 0.0) );

    float4 r1;
    r1[0] = dot( pg[ int(b[0]) + 1 ].xyz, f - float3(0.0, 0.0, 1.0) );
    r1[1] = dot( pg[ int(b[1]) + 1 ].xyz, f - float3(1.0, 0.0, 1.0) );
    r1[2] = dot( pg[ int(b[2]) + 1 ].xyz, f - float3(0.0, 1.0, 1.0) );
    r1[3] = dot( pg[ int(b[3]) + 1 ].xyz, f - float3(1.0, 1.0, 1.0) );

    // interpolate
    f = s_curve(f);
    r = /*mix*/lerp( r, r1, f[2] );
    r = /*mix*/lerp( r.xyyy, r.zwww, f[1] );
    return /*mix*/lerp( r.x, r.y, f[0] );
}

// 2D version
float noise333(float2 v, const float4 pg[B2])
{
    v = v + float2(10000.0, 10000.0);

    float2 i = frac(v * BR) * float(B);   // index between 0 and B-1
    float2 f = frac(v);            // fractional position

    // lookup in permutation table
    float2 p;
    p[0] = pg[ int(i[0])   ].w;
    p[1] = pg[ int(i[0]) + 1 ].w;
    p = p + i[1];

    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ int(p[0]) ].xy,   f);
    r[1] = dot( pg[ int(p[1]) ].xy,   f - float2(1.0, 0.0) );
    r[2] = dot( pg[ int(p[0]) + 1 ].xy, f - float2(0.0, 1.0) );
    r[3] = dot( pg[ int(p[1]) + 1 ].xy, f - float2(1.0, 1.0) );

    // interpolate
    f = s_curve(f);
    r = /*mix*/lerp( r.xyyy, r.zwww, f[1] );
    return /*mix*/lerp( r.x, r.y, f[0] );
}

// 1D version
float noise333(float v, const float4 pg[B2])
{
    v = v + 10000.0;

    float i = frac(v * BR) * float(B);   // index between 0 and B-1
    float f = frac(v);            // fractional position

    // compute dot products between gradients and vectors
    float2 r;
    r[0] = pg[int(i)].x * f;
    r[1] = pg[int(i) + 1].x * (f - 1.0);

    // interpolate
    f = s_curve(f);
    return /*mix*/lerp( r[0], r[1], f);
}
float4 pg[B2];            // permutation/gradient table

/*
d:\C++GameSamples\D3DvsOGL\D3D\dx9_HLSL-noise\vnoise.fxh(89): error X4004: program too complex; there are more active values than registers
d:\C++GameSamples\D3DvsOGL\D3D\dx9_HLSL-noise\dx9_HLSL-noise3.fx(353): ID3DXEffectCompiler::CompileEffect: There was an error compiling expression
ID3DXEffectCompiler: Compilation failed
(3048): Fx Compile Error
(3048): d:\C++GameSamples\D3DvsOGL\D3D\dx9_HLSL-noise\vnoise.fxh(89): error X4004: program too complex; there are more active values than registers
d:\C++GameSamples\D3DvsOGL\D3D\dx9_HLSL-noise\dx9_HLSL-noise3.fx(353): ID3DXEffectCompiler::CompileEffect: There was an error compiling expression
ID3DXEffectCompiler: Compilation failed
First-chance exception at 0x00475823 in dx9_HLSL-noise-D3D9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.
Unhandled exception at 0x00475823 in dx9_HLSL-noise-D3D9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.
*/
#define ssnoise(x) ((2*noise333(x,pg))-1)

float turbulence(int octaves, float3 P, float lacunarity, float gain)
{
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*ssnoise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}

struct VS_INPUT
{
    float3 position	: POSITION;
    float3 normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 texture0	: TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 hposition : POSITION;
	float2 v_texCoord2D  : TEXCOORD0;
	float3 v_texCoord3D  : TEXCOORD1;
	float4 color	 : COLOR0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

ROW_MAJOR float4x4 modelViewProjection MVPSEMANTIC;
float time;

/*
 * Both 2D and 3D texture coordinates are defined, for testing purposes.
 */

VS_OUTPUT mainVS( const VS_INPUT IN)
{
    VS_OUTPUT OUT;

	OUT.hposition = mul( modelViewProjection, float4(IN.position, 1) );

	OUT.v_texCoord2D = IN.texture0.xy;

	OUT.v_texCoord3D = IN.position.xyz;

	OUT.color = float4(0,1,0,1);//IN.color;

	return OUT;

}

PS_OUTPUT mainPS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;

  /* These lines test, in order, 2D classic noise, 2D simplex noise,
   * 3D classic noise, 3D simplex noise, 4D classic noise, and finally
   * 4D simplex noise.
   * Everything but the 4D simpex noise will make some uniform
   * variables remain unused and be optimized away by the compiler,
   * so OpenGL will fail to bind them. It's safe to ignore these
   * warnings from the C program. The program is designed to work anyway.
   */
   float n = 0;
  //float n = noise(IN.v_texCoord2D * 32.0 + 240.0);
  //float n = snoise(IN.v_texCoord2D * 16.0);
  //float n = noise(float3(4.0 * IN.v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
//if(NoiseTechnique==1)
  n = ssnoise(float3(2.0 * IN.v_texCoord3D.xyz * noiseScale * (2.0 + sin(0.5 * time))));
/*else //if(NoiseTechnique==4)
  n = ssnoise(float3(2.0 * IN.v_texCoord3D.xyz * (2.0 + sin(0.5 * time))));
  */
  //float n = noise(float4(8.0 * IN.v_texCoord3D.xyz, 0.5 * time));
  //float n = snoise(float4(4.0 * IN.v_texCoord3D.xyz, 0.5 * time));

  OUT.color = IN.color * float4(0.5 + 0.5 * float3(n, n, n), 1.0);

  OUT.color=GammaCorrect4(OUT.color);

  return OUT;

}

PS_OUTPUT fragmentShaderPNoise(VS_OUTPUT IN)
{

  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float n = ssnoise(float3(4.0 * IN.v_texCoord3D.xyz));
  OUT.color = float4(0.5 + 0.85 * float3(n, n, n), 1.0);

  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
PS_OUTPUT fragmentShaderWood(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.658, 0.356, 0.0588);
  float3 color2 = float3(0.952, 0.713, 0.470);


  float x_v=IN.v_texCoord3D.x*0.6;
  float y_v=IN.v_texCoord3D.y*0.6;
  float z_v=IN.v_texCoord3D.z*0.6;

  float3 t1=float3(x_v*2,y_v*2,z_v*2);
  float3 t11=float3(x_v*2+1,y_v*2+1,z_v*2+1);
  float3 t2=float3(x_v*0,y_v*0,z_v*0);

  float temp_x=60*ssnoise(t1);
  float temp_y=60*ssnoise(t2);

  x_v=x_v-temp_x;
  y_v=y_v-temp_y;

  float3 diffuse=lerp(color1, color2, pow(cos(sqrt(x_v*x_v+y_v*y_v)+30*turbulence(68, t2, 2, 0.6))*0.5+0.5,3));
  OUT.color = float4(diffuse, 1.0);

  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
PS_OUTPUT fragmentShaderMarble(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.8f, 0.2);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.1+6*turbulence(5,IN.v_texCoord3D.xyz,2,0.6)));
  OUT.color = float4(diffuse, 1.0);

  OUT.color=GammaCorrect4(OUT.color);
  return OUT;

}
PS_OUTPUT fragmentShaderFur(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.8, 0.7, 0.0);
  float3 color2 = float3(0.6, 0.1, 0.0);

  float3 diffuse=lerp(color1, color2, clamp(0.7*turbulence(6,IN.v_texCoord3D.xyz,3,0.9), 0,1));

  OUT.color = float4(diffuse, 1.0);

  OUT.color=GammaCorrect4(OUT.color);
  return OUT;

}
PS_OUTPUT fragmentShaderCloud(VS_OUTPUT IN)
{
  PS_OUTPUT OUT;

  IN.v_texCoord3D=float3(IN.v_texCoord2D, 0);

  float3 color1 = float3(0.1f, 0.1f, 0.99);
  float3 color2 = float3(0.8f, 0.8, 0.8f);

  float3 diffuse=lerp(color1, color2, cos(IN.v_texCoord3D.z*0.5+2*turbulence(6,IN.v_texCoord3D.xyz,2,0.5))*0.9);
  OUT.color = float4(diffuse, 1.0);

  OUT.color=GammaCorrect4(OUT.color);
  return OUT;


}
       
#define TextureMatrix modelViewProjection
float Displacement=0.5;

[Vertex shader]
VS_OUTPUT mainVVS( VS_INPUT IN )
{
	VS_OUTPUT OUT=(VS_OUTPUT)0;
    float4 noisePos = mul(TextureMatrix/*[0]*/ , float4(IN.position,1));

    float i = (ssnoise(noisePos.xyz/*, pg*/) + 1.0) * 0.5;
    OUT.color = float4(i, i, i, 1.0);

    // displacement along normal
    float4 position = float4(IN.position,1) + (float4(IN.normal, 1.0) * i * Displacement);
    //float4 position = IN.position + (IN.normal * i * Displacement);
    position.w = 1.0;

    OUT.hposition = mul(modelViewProjection , position);
    return OUT;
};
VS_OUTPUT main( VS_INPUT IN )
{
	return mainVVS(IN);
};
[Fragment shader]
PS_OUTPUT mainVPS( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;

	OUT.color = IN.color;

  OUT.color=GammaCorrect4(OUT.color);
	return OUT;
};
PS_OUTPUT main( VS_OUTPUT IN )
{
	return mainVPS(IN);
};
/*
//------------------------------------
vertexOutput VS(vertexInput IN)
{
    vertexOutput OUT;
    OUT.hPosition = mul(IN.position, worldViewProj);
    OUT.texcoord = IN.texcoord * noiseScale;
    OUT.wPosition = mul(IN.position, world).xyz * noiseScale;
    return OUT;
}

//-----------------------------------
float4 PS_inoise(vertexOutput IN): COLOR
{
	float3 p = IN.wPosition;
//	return abs(inoise(p));
//	return inoise(p);
	return inoise(p)*0.5+0.5;
//	return inoise(float3(IN.texcoord, 0.0))*0.5+0.5;
}
*/

