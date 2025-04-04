/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
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
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
// https://www.shadertoy.com/view/tdjyzz
// Tree in the wind
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
float iTime=0.0;
sampler2D iChannel0;

float3 mod(float3 x, float y)
{
  return x - y * floor(x / y);
}

float rbgToluminance(float3 v3)
{
	return 0.5;// ???
}

//   _____                 _         _   _                     _           _ 
//  /__   \_ __ ___  ___  (_)_ __   | |_| |__   ___  __      _(_)_ __   __| |
//    / /\/ '__/ _ \/ _ \ | | '_ \  | __| '_ \ / _ \ \ \ /\ / / | '_ \ / _` |
//   / /  | | |  __/  __/ | | | | | | |_| | | |  __/  \ V  V /| | | | | (_| |
//   \/   |_|  \___|\___| |_|_| |_|  \__|_| |_|\___|   \_/\_/ |_|_| |_|\__,_|
//                                                                                                       
//
//----------------------------------------------------------------------------------------
//     ___                                                    _  _    
//    / __\ _   _     /\/\    __ _  _   _  _ __  ___    __ _ (_)| | __
//   /__\//| | | |   /    \  / _` || | | || '__|/ _ \  / _` || || |/ /
//  / \/  \| |_| |  / /\/\ \| (_| || |_| || |  | (_) || (_| || ||   < 
//  \_____/ \__, |  \/    \/ \__,_| \__,_||_|   \___/  \__, ||_||_|\_\
//          |___/                                      |___/          
//
//----------------------------------------------------------------------------------------

//The day/night cyle takes around 1 a minute, so sit back and relax.

//Text ascii art using : http://patorjk.com/software/taag/

float tonemapOp(float v)
{
    v = pow(v, 2.0);
    v = v / (1.0 + v);
    return pow(v, 1.0/2.0) * 1.025;
}

float3 tonemap(float3 colour)
{
    float inputLuminance = max(0.0001, rbgToluminance(colour));
    float3 normalisedColour = colour / inputLuminance;
    
    float3 tonemapColour;
    tonemapColour.r = tonemapOp(colour.r);
    tonemapColour.g = tonemapOp(colour.g);
    tonemapColour.b = tonemapOp(colour.b);
    float tonemappedLuminance = tonemapOp(inputLuminance);
    
    tonemapColour = (tonemapColour / max(0.0001, rbgToluminance(tonemapColour)));
    
    return tonemappedLuminance * lerp(normalisedColour, tonemapColour, min(1.0, 0.35*inputLuminance));
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 uv = fragCoord.xy / iResolution.xy;
      
	float4 colour = tex2D(iChannel0, uv);
    
    //Vignette
    colour.rgb *= smoothstep(1.15, 0.3, length(uv - 0.5*oz.xx));
    
	//Tonemap
    float toeStrength = 1.25;
    colour.rgb = tonemap(colour.rgb * toeStrength); colour.rgb = pow(colour.rgb, toeStrength*oz.xxx);
    
    //Gamma
    colour = pow(colour, float4(1.0/2.2));
    fragColor = colour;
    
    //Dithering
    fragColor += ((hash12(fragCoord)) - 0.5)*4.0/255.0;
	return fragColor;
}

