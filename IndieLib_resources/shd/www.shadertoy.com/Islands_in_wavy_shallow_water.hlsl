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

/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

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
#define textureLod tex2D
sampler2D iChannel0;
sampler2D iChannel1;
sampler2D iChannel2;
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
float mod(float x, float y)
{
  return x - y * floor(x / y);
}

float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}

// https://www.shadertoy.com/view/flSXzR
// Created by Arrangemonk 2021, arrangemonk@gmail.com
// where i have a clear source i added shadertoy link


float3 rampBiome(float x){
    float3 col = biome[0];
    float step_size = 1./(float(biome.length())-1.);
    for(int i = 0; i < biome.length()-1; i++){
        col =lerp(col, biome[i+1], linearstep(float(i)*step_size, (float(i)+1.)*step_size, x));
    }
    return col;
}
 
 float3 rampSky(float x){
    float3 col = sky[0];
    float step_size = 1./(float(sky.length())-1.);
    for(int i = 0; i < sky.length()-1; i++){
             col =lerp(col, sky[i+1], linearstep(float(i)*step_size, (float(i)+1.)*step_size, x));
    }
    return col;
}

float h(in float2 uv,bool water)
    {
    uv = uv * 0.005;
    float result = tex2D(iChannel0,uv).x - tex2D(iChannel3,uv*45.).x *0.025;
    if(!water)
        result = max(waterline + waterripple(uv*5.,iChannel3,iTime).z *0.03,result);
    return result ;
    }

//https://www.shadertoy.com/view/3sSSW1
float3 texNormalMap(in float2 uv,float scale)
{ 
    float p  = h(uv,true);
    float h1 = h(uv + float2(scale,0),true);
    float v1 = h(uv +  float2(0,scale),true);
       
   	float2 normal = (p - float2(h1, v1));
    
    normal *= normalStrength;
    normal += 0.5;
    
    return normalize(float3(normal, 1.));
}



float3 color( in float2 uv,float height)
{ 
    return lerp(rampBiome(height*0.25),lerp(fbm(uv,iChannel1,octaves).rgb,rampBiome(height*0.25),0.75),height/6. +0.1);
}

float3 target(in float3 origin,in float3 direction,in float distance)
    {
        return origin + (direction * distance);
    }

//lost link, most likely iqs doing
float RayMarch(float3 origin,float3 direction,in bool water, out bool result)
{
    float t = 0.;
    for (int i = 0; i < 72; i++)
    {
        float3 rp = target(origin,direction,t);
        float h = rp.y - (h(rp.xz,water));
        if (abs(h) < (.0015 * t) || t > DrawDistance) break;
        t += h;
    }
    result = t < DrawDistance;
    return t;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    time = iTime;
    float2 uv = fragCoord/iResolution.xy;
    float3 sunDir = normalize(float3(6.,-3., 2.));
    
    //low resolution 
   //  uv = floor(uv*float2(320.,240.))/float2(320.,240.);
    float aspect = iResolution.x / iResolution.y;
    
    float3 rd = normalize(float3((1.- 2. * uv.x) * aspect + sin(time * 0.1)*0.2+sin(time *3.)*.01,-1. + 2. * uv.y-0.6+sin(time *3.)*.01, -3.0));
    float3 ro = float3(sin(time * 0.1)*3.+sin(time *3.)*.01,5.+sin(time *5.)*.009, -time+sin(time *4.)*.011);

    bool result;
    float distance = RayMarch(ro,rd,false,result);
    float distfract = clamp(distance*DrawDistanceFract,0.,1.);
    
     float3 c = rampSky((uv.y-0.4)*1.1);
     if(result)
     {
         float3 point = target(ro,rd,distance);
         float height =  h(point.xz,false);
         if(h(point.xz,true) < height) // why water isnt at 0 u ask? i winged it, thats why
         {
            float3 waternormal = normalize(waterripple(point.xz * 0.02 + distance * 0.002,iChannel3,iTime) + float3(0,0,2.+ (distfract * distfract) * 10.));
            float3 wd = refract(rd,waternormal, .97);
            float3 wr = normalize(reflect(rd,waternormal));
             float wdistance = RayMarch(point,wd,true,result);
             float wdistfract = clamp(wdistance*DrawDistanceFract,0.,1.);
             float3 wp = target(point,wd,wdistance);
             float wheight =  h(wp.xz,true)-0.1;
             
             float hilight = clamp((dot(wr,waternormal) * 0.5 + 0.5)* distance *0.035,0.,1.);
             //float3 skyreflect = rampSky(1.1 -wr.z * 0.6);
             float3 skyreflect = rampSky(1.1 - tex2D(iChannel2,-wr).r* 0.6);
             skyreflect = lerp(skyreflect,tex2D(iChannel2,wr).rgb,.5);
             
             float wsun = dot( texNormalMap(wp.xz,1.), sunDir);
             wsun += dot( texNormalMap(wp.xz,0.25), sunDir )*2.;
             wsun += dot( texNormalMap(wp.xz,0.02), sunDir )*4.,
             wsun *= 0.143;
             wsun = clamp( wsun, 0.0, 1.0 )-0.6;
             float cst = caustics(wp.zx * 5. + wp.y,time*2.)*0.4 * clamp(wdistance,0.,1.)*clamp(wp.y/8.,0.,1.);
             
             float3 c1 = lerp(color(wp.xz,lerp(3.-wdistfract*10.,wheight,.2)) + wsun,skyreflect,hilight) +cst;
             //float3 c1 = float3(cst,cst,cst);
       
             c = lerp(c1,c,distfract);
             c = foam(point.xz + waternormal.xy *0.15 - float2(0.2,0.2)*time,iChannel3,iChannel1,c,wheight);
         }
         else{
             float sun = dot( texNormalMap(point.xz,1.), sunDir);
             sun += dot( texNormalMap(point.xz,0.25), sunDir )*2.;
             sun += dot( texNormalMap(point.xz,0.02), sunDir )*4.,
             sun *= 0.143;
             sun = clamp( sun, 0.0, 1.0 )-0.6;
             c = lerp(color(point.xz,height) + sun,c,distfract);
             c += foamborder(point.xz - float2(0.1,0.1)*time,iChannel3,height*height -5.45);
         }
     }
    return float4(c,1.0);

}



