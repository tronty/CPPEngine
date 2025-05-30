// https://www.shadertoy.com/view/lsGGDd
// Load Custom Textures
// ----------------------------------------------------------------------------------------
//	"Load Custom Textures" by Antoine Clappier - March 2015
//
//	Licensed under a Creative Commons Attribution-ShareAlike 4.0 International License
//	http://creativecommons.org/licenses/by-sa/4.0/
// ----------------------------------------------------------------------------------------

// Image credits:
//  Nasa, http://visibleearth.nasa.gov/


/*

A simple method to load custom image textures in Shadertoy!
-----------------------------------------------------------

The idea is to call directly the SetTexture function found in Shadertoy js code.

Here is how to loads the three textures needed for this shader:
 - Open the javascript console of your browser:
				   Mac      /     Windows
	Chrome:  cmd + opt + J  /  ctrl + shift J
	Firefox: cmd + opt + K  /  ctrl + shift K
    IE:          na         /  F12   

- Then copy the following lines in the console to load custom 2048x2048 textures:

gShaderToy.SetTexture(0, {mSrc:'https://dl.dropboxusercontent.com/s/88u2uo8dxdmgzxo/world2.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});
gShaderToy.SetTexture(1, {mSrc:'https://dl.dropboxusercontent.com/s/5rdhhnvnr5mochq/cloud2.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});
gShaderToy.SetTexture(2, {mSrc:'https://dl.dropboxusercontent.com/s/ojl5zoxgbdn5w5s/light2.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});

- Or, the following lines for 1024x1024 textures:

gShaderToy.SetTexture(0, {mSrc:'https://dl.dropboxusercontent.com/s/0j4q7p4x0upj40q/world1.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});
gShaderToy.SetTexture(1, {mSrc:'https://dl.dropboxusercontent.com/s/26xr0l2ly68xgzh/cloud1.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});
gShaderToy.SetTexture(2, {mSrc:'https://dl.dropboxusercontent.com/s/b67udjdsw4gzf99/light1.jpg?dl=0', mType:'tex2D', mID:1, mSampler:{ filter: 'mipmap', wrap: 'repeat', vflip:'true', srgb:'false', internal:'byte' }});

- hit return to execute and load the textures.


Using your own images:
 - The first argument of gShaderToy.SetTexture() is the iChannel index from 0 to 3
 - The second argument defines the url and additional parameters of the tex2D.
 - Your images must be hosted on a server (such as Dropbox) that allows direct link 
   from a different domain in javascript. Otherwise, you will get an error message:
   "'example.com has been blocked from loading by Cross-Origin Resource Sharing policy"

*/


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
const float2 iMouse=float2(1, 1);
const float2 iResolution=float2(1, 1);
float iTime=0.0;
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;

#define Pi 3.14159265359
#define d2r(a) ((a)*180.0/Pi)
#define RGB(r,g,b) pow(float3(float(r), float(g), float(b))/255.0, float3(2.22, 2.22, 2.22))

#define R0 1.0000	// Nomralized Earth radius (6360 km)
#define R1 1.0094	// Atmosphere radius (6420 km) 

float3 Render(in float2 uv)
{
    float3 Color = float3(0,0,0);
    float t = 1.0*iTime;

    // Sun:
    float3 L0 = float3(cos(0.1*t), 0.0, sin(0.1*t));
    float cs = cos(d2r(90.0 + 23.4)), sn = sin(d2r(90.0 + 23.4));
    float3 LightDir = float3(cs*L0.x + sn*L0.y, cs*L0.y - sn*L0.x, L0.z);

    float2 SunC = -5.0*LightDir.xy/LightDir.z - uv;
    float Halo = max(0.0, dot(LightDir, normalize(float3(uv.x, uv.y, -5.0))));
    float3 v3=tex2D(iChannel1, float2(0.1*t, atan2(SunC.x,SunC.y))).xyz;
	float SunRay = pow(abs(v3), float3(2.22, 2.22, 2.22)).x;
	float f=1.0;
	//f=smoothstep(0.85, 1.3, length(SunC+uv);
    float Sun = 0.05*(1.0 + SunRay)*pow(Halo, 1000.0)*f;
   
    // Sphere hit:
    float z = 1.0 - dot(uv, uv);
    if(z < 0.0)
    {
        Sun += 1.5*pow(Halo, 10000.0);
        float3 v3_=Sun*RGB(255,250,230);
        return v3_;
    }
    
    // Intersection:
    float3 Normal     = float3(uv.x, uv.y, sqrt(z));
    float3 Reflection = reflect(float3(0.0, 0.0, 1.0), Normal);


    // Textures:
	float U = 1.0-atan2(Normal.z, Normal.x) / (2.0*Pi);
	float V = 1.0-(atan2(length(Normal.xz), Normal.y)) / Pi;
	float3 v31=float3(0.5,0.5,0.5);
	float3 v32=float3(1,1,1);
	float3 v33=float3(1,1,1);
	float3 v34=float3(0,0,0);
	float3 v35=float3(0,0,0);
	#if 1
	v31=tex2D(iChannel0, float2(U-t/80.0, V)).xyz;
	v32=tex2D(iChannel1, float2(U-t/75.0, V)).xyz;
	v33=tex2D(iChannel1, float2(U-t/75.0+0.001, V)).xyz;
	v34=tex2D(iChannel1, float2( -t/200.0, 0.8)).xyz;
	v35=tex2D(iChannel2, float2(U-t/80.0, V)).xyz;
	#endif
 	float3 Ground = pow(v31, float3(2.22, 2.22, 2.22));
	float3 Cloud  = pow(v32, float3(2.22, 2.22, 2.22));
	float3 Cloud2 = pow(v33, float3(2.22, 2.22, 2.22));
	float3 KsMap  = pow(v34, float3(2.22, 2.22, 2.22));
	float3 Night  = pow(v35, float3(2.22, 2.22, 2.22));
	
    // Shading
	float Diffuse     = max(0.0, dot(Normal, LightDir));
	float Specular    = max(0.0, dot(-Reflection, LightDir));

    //float Scatter     = 4.0*pow((sqrt(R1 - dot(uv, uv)) - Normal.z) / sqrt(R1-R0), 1.35);
    float Scatter     = 4.0*pow((sqrt(R1 - dot(uv, uv)) - Normal.z) / sqrt(R1-R0), 1.35);
    
    float Extinct     = pow(1.0 - Diffuse, 4.0);
    float Sea         = smoothstep(1.0, 0.0, 100.0*length(Ground - RGB(2,5,20)));
    float Shadow      = 1.0 - pow(Cloud2.x, 0.2);
    
    float3 Light = mix(float3(1,1,1), RGB(255, 150, 40), Extinct);
 
    Color = Shadow*(Ground + 0.8*Sea*RGB(19,35,60));
    Color = mix(Color, float3(1,1,1), 2.0*Cloud);
    Color *= Light*Diffuse;
    Color += 2.0*Light*Diffuse*(0.3 + 0.7*KsMap.x)*mix(0.03, 0.4, Sea)*pow(Specular, (0.8 + 0.2*KsMap.x)*mix(9.0, 200.0, Sea));
    Color += pow(max(0.0, dot(Normal, -LightDir)), 2.0)*Night*(1.0-pow(Cloud.x, 0.2));
    Color *= mix(float3(1,1,1), RGB(255-58,255-72,255-90), 1.0*Scatter);
    Color += 4.0*Diffuse*(1.0 + Sea)*Scatter*RGB(58,72,90);

    Color += Sun*RGB(255,250,230);
    
    return Color;
}

float4 main(VsOut IN): COLOR 
{
	//return IN.Color;
	float4 fragColor;
	float2 fragCoord=IN.uv;
	float2 uv = (2.0*IN.position.xy - iResolution.xy) / iResolution.y;
    float3 Color = pow(abs(Render(1.05*uv)),  float3(0.45, 0.45, 0.45));
	fragColor = float4(Color, 1.0);
	return fragColor;
}

