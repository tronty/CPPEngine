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
ROW_MAJOR 
float4x4 worldViewProj 
MVPSEMANTIC
;
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
float2      iResolution=float2(1,1);                  // viewport iResolution (in pixels)
float     iTime=0;                        // shader playback iTime (in seconds)
float2      iMouse=float2(1,1);                       // iMouse pixel coords
sampler2D iChannel0;
sampler2D iChannel1;

// Himalayas. Created by Reinder Nijhoff 2018
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// @reindernijhoff
//
// https://www.shadertoy.com/view/MdGfzh
//
// This is my first attempt to render volumetric clouds in a fragment shader.
//
// I started this shader by trying to implement the clouds of Horizon Zero Dawn, as
// described in "The real-time volumetric cloudscapes of Horizon Zero Dawn" by 
// Andrew Schneider and Nathan Vos.[1] To model the shape of the clouds, two look-up
// textures are created with different frequencies of (Perlin -) Worley noise:
//
// Buffer A: The main look-up texture for the cloud shapes. 
// Buffer B: A 3D (32x32x32) look-up texture with Worley Noise used to add small details 
//           to the shapes of the clouds. I have packed this 3D texture into a 2D buffer.
//           
// Because it is not possible (yet) to create buffers with fixed size, or 3D buffers, the
// look-up texture in Buffer A is 2D, and a slice of the volume that is described in the 
// article. Therefore, and because I didn't have any slots left (in Buffer C) to use a 
// cloud type/cloud coverage texture, the modelling of the cloud shapes in this shader is 
// in the end mostly based on trial and error, and is probably far from the code used in 
// Horizon Zero Dawn.
//
// Buffer D: Rendering of the clouds.
//
// I render the clouds using the improved integration method of volumetric media, as described 
// in "Physically Based Sky, Atmosphere and Cloud Rendering in Frostbite" by 
// Sébastien Hillaire.[2]
//
// You can find the (excellent) example shaders of Sébastien Hillaire (SebH) here:
//
// https://www.shadertoy.com/view/XlBSRz
// https://www.shadertoy.com/view/MdlyDs
//
// Buffer C: Landscape
//
// To create an interesting scene and to add some scale to the clouds, I render a 
// terrain using a simple heightmap, based on the work by Íñigo Quílez on value noise and its 
// analytical derivatives.[3]
//
// In fact, the heightmap of this shader is almost exactly the same as the heightmap that 
// is used in Íñigo Quílez' shader Elevated:
//
// https://www.shadertoy.com/view/MdX3Rr
//
// To reduce noise I use temporal reprojection (both for clouds (Buffer D) and the terrain 
// (Buffer C)) separatly. The temporal reprojection code is based on code from the shader
// "Rain Forest" (again by Íñigo Quílez):
//
// https://www.shadertoy.com/view/4ttSWf
// 
// Finally, in the Image tab, clouds and terrain are combined, a small humanoid is added
// (by Hazel Quantock) and post processing is done.
//
// [1] https://www.guerrilla-games.com/read/the-real-time-volumetric-cloudscapes-of-horizon-zero-dawn
// [2] https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/s2016-pbs-frostbite-sky-clouds-new.pdf
// [3] http://iquilezles.org/www/articles/morenoise/morenoise.htm
//

#define AA 3

//
// Cheap 2D Humanoid SDF for dropping into scenes to add a sense of scale.
// Hazel Quantock 2018
//
// Based on: https://www.shadertoy.com/view/4scBWN
//
float RoundMax( float a, float b, float r ) {
    a += r; b += r;    
    float f = ( a > 0. && b > 0. ) ? sqrt(a*a+b*b) : max(a,b);    
    return f - r;
}

float RoundMin( float a, float b, float r ) {
    return -RoundMax(-a,-b,r);
}

float Humanoid( in float2 uv, in float phase ) {
    float n3 = sin((uv.y-uv.x*.7)*11.+phase)*.014; // "pose"
    float n0 = sin((uv.y+uv.x*1.1)*23.+phase*2.)*.007;
    float n1 = sin((uv.y-uv.x*.8)*37.+phase*4.)*.004;
    float n2 = sin((uv.y+uv.x*.9)*71.+phase*8.)*.002;

    
    float head = length((uv-float2(0,1.65))/float2(1,1.2))-.15/1.2;
    float neck = length(uv-float2(0,1.5))-.05;
    float torso = abs(uv.x)-.25 - uv.x*.3;

    torso = RoundMax( torso, uv.y-1.5, .2 );
    torso = RoundMax( torso, -(uv.y-.6), .0 );

    float f = RoundMin(head,neck,.04);
    f = RoundMin(f,torso,.02);
    
    float leg = abs(abs(uv.x+(uv.y-.9)*.1*cos(phase*3.))-.15+.075*uv.y)-.07-.07*uv.y; 
    leg = max( leg, uv.y-1. );
    
    f = RoundMin(f,leg,.1);

    float stick = max(abs(uv.x+.4-uv.y*.04)-0.025, uv.y-1.15);
    float arm = max(max(abs(uv.y-1.-uv.x*.3) - .06, uv.x), -uv.x-.4);
    
    f = RoundMin(f, stick, 0.0);
    f = RoundMin(f, arm, 0.05);
    
    f += (-n0+n1+n2+n3)*(.1+.9*uv.y/1.6);
    
    return max( f, -uv.y );
}

//
// Lens flare, original based on:
// musk's lens flare by mu6k  
//
// https://www.shadertoy.com/view/4sX3Rs
//
float lensflare(float2 fragCoord) {
    float3 ro, ta;
    float3x3 cam = getCamera( iTime, iMouse/iResolution.xyxy, ro, ta );
    float3 cpos = SUN_DIR*cam; 
    float2 pos = CAMERA_FL * cpos.xy / cpos.z;
    float2 uv = (-iResolution.xy + 2.0*fragCoord)/iResolution.y;
    
	float2 uvd = uv*(length(uv));
	float f = 0.1/(length(uv-pos)*16.0+1.0);
	f += max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*0.25;
	float2 uvx = lerp(uv,uvd,-0.5);
	f += max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	f += max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	uvx = lerp(uv,uvd,-0.4);
	f += max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
    
	return f;
}

bool intersectSphere ( in float3 ro, in float3 rd, in float4 sph ) {
    float3  ds = ro - sph.xyz;
    float bs = dot(rd, ds);
    float cs = dot(ds, ds) - sph.w*sph.w;
    float ts = bs*bs - cs;
	
    if( ts > 0.0 ) {
        ts = -bs - sqrt( ts );
		if( ts>0. ) {
			return true;
		}
    }
    return false;
}

bool intersectPlane (in float3 ro, in float3 rd, in float3 n, in float3 p0, inout float dist) {   
    dist = dot(p0 - ro, n)/dot(rd,n);
    return dist > 0.;
}

void mainImage( out float4 fragColor, in float2 fragCoord ) {  
    if( letterBox(fragCoord, iResolution.xy, 2.35) ) {
        fragColor = float4( 0., 0., 0., 1. );
    } else {
        float4 col = texture2D(iChannel0, ivec2(fragCoord));
        float4 clouds = texture2D(iChannel1, ivec2(fragCoord));
    	
        col.rgb = clouds.rgb + col.rgb * clouds.a;
       
        float3 ro, rd, ta;
		float3x3 cam = getCamera( iTime, iMouse/iResolution.xyxy, ro, ta );
        float dist;
        float4 tcol = float4(0.);
        float2 p = (-iResolution.xy + 2.0*(fragCoord))/iResolution.y;
        rd = cam * normalize(float3(p,CAMERA_FL)); 
        
        if (intersectSphere(ro,rd,float4(FLAG_POSITION,HUMANOID_SCALE*INV_SCENE_SCALE*2.))) {
            for(int x=0; x<AA; x++) {
                for(int y=0; y<AA; y++) {
                    float2 p = (-iResolution.xy + 2.0*(fragCoord + float2(x,y)/float(AA) - .5))/iResolution.y;
                    rd = cam * normalize(float3(p,CAMERA_FL)); 

                    if (intersectPlane(ro, rd, float3(0,0,1), FLAG_POSITION, dist) && dist < col.w) {
                        float3 pos = ro + rd * dist;
                        float2 uv = (pos.xy - FLAG_POSITION.xy)*(SCENE_SCALE/HUMANOID_SCALE);
                        uv.x = -uv.x + uv.y*0.05;
                        float sdf = Humanoid( uv, 3. );
                        float a = smoothstep(.4,.6,.5-.5*sdf/(abs(sdf)+.002));
                        float sdf2 = Humanoid( uv+float2(.025,0.05), 3. );
                        float a2 = smoothstep(.4,.6,.5-.5*sdf2/(abs(sdf2)+.002));
                        float c = (a-a2)*2.;
                        c = clamp(c+uv.x*.2+.6,0.,1.); c*=c; c*=c;
                        tcol += float4(lerp(float3(.04,0.05,0.06),SUN_COLOR,c),a);
                    }
                }
            }
            tcol /= float(AA*AA);
        }
        
        col.rgb = lerp(col.rgb, tcol.rgb, tcol.w);    
            
        // lens flare
        col.rgb += SUN_COLOR*lensflare(fragCoord)*smoothstep(-.3,.5,dot(rd,SUN_DIR));       
        col.rgb = clamp(col.rgb, float3(0), float3(1));
        
        // gamma and contrast
        col.rgb = lerp(col.rgb, pow(col.rgb, float3(1./2.2)), .85);
        col.rgb = lerp( col.rgb, col.bbb, 0.2 ); 
     
        // vignette
        float2 uv = fragCoord / iResolution.xy;
        col.rgb = lerp(col.rgb*col.rgb, col.rgb, pow( 16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.1 ));
        
        // noise
        col.rgb -= hash12(fragCoord)*.025;
        
        return float4( col.rgb, 1. );
    }
}
