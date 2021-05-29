/*********************************************************************NVMH3****
File:  $Id: //sw/devtools/SDK/9.5/SDK/MEDIA/HLSL/vbomb.fx#3 $

Copyright NVIDIA Corporation 2002-2004
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
	HLSL noise implementation for vertex program profile

	This is based on Perlin's original code:
	http://mrl.nyu.edu/~perlin/doc/oscar.html
	& Thanks to Simon Green - KB

	It combines the permutation and gradient tables into one array of
		float4's to conserve constant memory.
	The table is duplicated twice to avoid modulo operations.

Notes:
	Should use separate tables for 1, 2 and 3D versions

******************************************************************************/

#include <include\\vnoise-table.fxh>

float Script : STANDARDSGLOBAL <
    string UIWidget = "none";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string Script = "Technique=vBomb;";
> = 0.8;

//////////////////////////////////////////////////////////////
// UNTWEAKABLES //////////////////////////////////////////////
//////////////////////////////////////////////////////////////

float timer : TIME <string UIWidget="None";>;
float4x4 WorldViewProj : WORLDVIEWPROJECTION <string UIWidget="None";>;

//////////////////////////////////////////////////////////////
// TWEAKABLES ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

float Displacement <
	string UIWidget = "slider";
	float UIMin = 0.0;
	float UIMax = 2.0;
	float UIStep = 0.01;
> = 1.6f;

float Sharpness <
	string UIWidget = "slider";
	float UIMin = 0.1;
	float UIMax = 5.0;
	float UIStep = 0.1;
> = 1.90f;

float ColorSharpness <
	string UIWidget = "slider";
	float UIMin = 0.1;
	float UIMax = 5.0;
	float UIStep = 0.1;
> = 3.0f;

float Speed <
	string UIWidget = "slider";
	float UIMin = 0.01;
	float UIMax = 1.0;
	float UIStep = 0.001;
> = 0.3f;

float TurbDensity <
	string UIWidget = "slider";
	float UIMin = 0.01;
	float UIMax = 8.0;
	float UIStep = 0.001;
> = 2.27f;

float ColorRange <
	string UIWidget = "slider";
	float UIMin = -6.0;
	float UIMax = 6.0;
	float UIStep = 0.01;
> = -2.0f;

float4x4 NoiseMatrix = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

//float4 dd[5] = {
//	0,2,3,1, 2,2,2,2, 3,3,3,3, 4,4,4,4, 5,5,5,5 };

//////////////////////////// texture ///////////////////////

texture GradeTex
<
    string ResourceName = "1D\\FireGrade.bmp";
    string ResourceType = "2D";
    string UIName = "Fire Gradient";
>;

sampler2D GradeSampler = sampler_state
{
    Texture = <GradeTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
	AddressU = clamp;
};

///////////// functions 

// this is the smoothstep function f(t) = 3t^2 - 2t^3, without the normalization
float3 s_curve(float3 t) { return t*t*( float3(3,3,3) - float3(2,2,2)*t); }
float2 s_curve(float2 t) { return t*t*( float2(3,3) - float2(2,2)*t); }
float  s_curve(float  t) { return t*t*(3.0-2.0*t); }

// 3D version
float noise(float3 v,
			const uniform float4 pg[FULLSIZE])
{
    v = v + float3(10000.0f, 10000.0f, 10000.0f);   // hack to avoid negative numbers

    float3 i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float3 f = frac(v);            // fractional position

    // lookup in permutation table
    float2 p;
    p.x = pg[ i[0]     ].w;
    p.y = pg[ i[0] + 1 ].w;
    p = p + i[1];

    float4 b;
    b.x = pg[ p[0] ].w;
    b.y = pg[ p[1] ].w;
    b.z = pg[ p[0] + 1 ].w;
    b.w = pg[ p[1] + 1 ].w;
    b = b + i[2];

    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ b[0] ].xyz, f );
    r[1] = dot( pg[ b[1] ].xyz, f - float3(1.0f, 0.0f, 0.0f) );
    r[2] = dot( pg[ b[2] ].xyz, f - float3(0.0f, 1.0f, 0.0f) );
    r[3] = dot( pg[ b[3] ].xyz, f - float3(1.0f, 1.0f, 0.0f) );

    float4 r1;
    r1[0] = dot( pg[ b[0] + 1 ].xyz, f - float3(0.0f, 0.0f, 1.0f) );
    r1[1] = dot( pg[ b[1] + 1 ].xyz, f - float3(1.0f, 0.0f, 1.0f) );
    r1[2] = dot( pg[ b[2] + 1 ].xyz, f - float3(0.0f, 1.0f, 1.0f) );
    r1[3] = dot( pg[ b[3] + 1 ].xyz, f - float3(1.0f, 1.0f, 1.0f) );

    // interpolate
    f = s_curve(f);
    r = lerp( r, r1, f[2] );
    r = lerp( r.xyyy, r.zwww, f[1] );
    return lerp( r.x, r.y, f[0] );
}

// 2D version
float noise(float2 v,
			const uniform float4 pg[FULLSIZE])
{
    v = v + float2(10000.0f, 10000.0f);

    float2 i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float2 f = frac(v);            // fractional position

    // lookup in permutation table
    float2 p;
    p[0] = pg[ i[0]   ].w;
    p[1] = pg[ i[0]+1 ].w;
    p = p + i[1];

    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ p[0] ].xy,   f);
    r[1] = dot( pg[ p[1] ].xy,   f - float2(1.0f, 0.0f) );
    r[2] = dot( pg[ p[0]+1 ].xy, f - float2(0.0f, 1.0f) );
    r[3] = dot( pg[ p[1]+1 ].xy, f - float2(1.0f, 1.0f) );

    // interpolate
    f = s_curve(f);
    r = lerp( r.xyyy, r.zwww, f[1] );
    return lerp( r.x, r.y, f[0] );
}

// 1D version
float noise(float v,
		const uniform float4 pg[FULLSIZE])
{
    v = v + 10000.0f;

    float i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float f = frac(v);            // fractional position

    // compute dot products between gradients and vectors
    float2 r;
    r[0] = pg[i].x * f;
    r[1] = pg[i + 1].x * (f - 1.0f);

    // interpolate
    f = s_curve(f);
    return lerp( r[0], r[1], f);
}

/////////////////////////////

struct appData 
{
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;
    float4 TexCoord0    : TEXCOORD0;
};

// define outputs from vertex shader
struct vertexOutData
{
    float4 HPosition	: POSITION;
    float4 Color0		: COLOR0;
};

////////

vertexOutData mainVS(appData IN) {
    vertexOutData OUT;
    float4 noisePos = TurbDensity*mul(IN.Position+(Speed*timer),NoiseMatrix);
    float i = (noise(noisePos.xyz, NTab) + 1.0f) * 0.5f;
    float cr = 1.0-(0.5+ColorRange*(i-0.5));
    cr = pow(cr,ColorSharpness);
    OUT.Color0 = float4((cr).xxx, 1.0f);
    // displacement along normal
    float ni = pow(abs(i),Sharpness);
    i -=  0.5;
    //i = sign(i) * pow(i,Sharpness);
    // we will use our own "normal" vector because the default geom is a sphere
    float4 Nn = float4(normalize(IN.Position).xyz,0);
    float4 NewPos = IN.Position - (Nn * (ni-0.5) * Displacement);
    //position.w = 1.0f;
    OUT.HPosition = mul(NewPos,WorldViewProj);
    return OUT;
}

float4 hotPS(vertexOutData IN) : COLOR {
	float2 nuv = float2(IN.Color0.x,0);
	float4 nc = tex2D(GradeSampler,nuv);
	return nc;
}

//////////////////////

technique vBomb <
	string Script = "Pass=p0;";
> {
	pass p0 <
		string Script = "Draw=geometry;";
	> {
		VertexShader = compile vs_2_0 mainVS();
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = None;
		AlphaBlendEnable = false;
		PixelShader = compile ps_2_a hotPS();
	}
}

///////////////////////////////////////////////////////////////// eof //
