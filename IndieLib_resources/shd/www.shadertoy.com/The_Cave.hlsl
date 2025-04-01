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
float iTime=0.0;
float2 iMouse=float2(1, 1);
float2 iResolution=float2(1, 1);
sampler2D iChannel4;
sampler2D iChannel5;
sampler2D iChannel6;

// https://www.shadertoy.com/view/MsX3RH
// The Cave
// constants for the camera tunnel
float2 cama=float2(-2.6943,3.0483);
float2 camb=float2(0.2516,0.1749);
float2 camc=float2(-3.7902,2.4478);
float2 camd=float2(0.0865,-0.1664);

float2 lighta=float2(1.4301,4.0985);
float2 lightb=float2(-0.1276,0.2347);
float2 lightc=float2(-2.2655,1.5066);
float2 lightd=float2(-0.1284,0.0731);

// calculates the position of a single tunnel
float2 Position(float z, float2 a, float2 b, float2 c, float2 d)
{
	return sin(z*a)*b+cos(z*c)*d;
}

// calculates 3D positon of a tunnel for a given time
float3 Position3D(float time, float2 a, float2 b, float2 c, float2 d)
{
	return float3(Position(time,a,b,c,d),time);
}

// 2d distance field for a slice of a single tunnel
float Distance(float3 p, float2 a, float2 b, float2 c, float2 d, float2 e, float r)
{
	float2 pos=Position(p.z,a,b,c,d);	
	float radius=max(5.0,r+sin(p.z*e.x)*e.y)/10000.0;
	return radius/dot(p.xy-pos,p.xy-pos);
}

// 2d distance field for a slice of the tunnel network
float Dist2D(float3 pos)
{
	float d=0.0;
	
	d+=Distance(pos,cama,camb,camc,camd,float2(2.1913,15.4634),70.0000);
	d+=Distance(pos,lighta,lightb,lightc,lightd,float2(0.3814,12.7206),17.0590);
	d+=Distance(pos,float2(2.7377,-1.2462),float2(-0.1914,-0.2339),float2(-1.3698,-0.6855),float2(0.1049,-0.1347),float2(-1.1157,13.6200),27.3718);
	d+=Distance(pos,float2(-2.3815,0.2382),float2(-0.1528,-0.1475),float2(0.9996,-2.1459),float2(-0.0566,-0.0854),float2(0.3287,12.1713),21.8130);
	d+=Distance(pos,float2(-2.7424,4.8901),float2(-0.1257,0.2561),float2(-0.4138,2.6706),float2(-0.1355,0.1648),float2(2.8162,14.8847),32.2235);
	d+=Distance(pos,float2(-2.2158,4.5260),float2(0.2834,0.2319),float2(4.2578,-2.5997),float2(-0.0391,-0.2070),float2(2.2086,13.0546),30.9920);
	d+=Distance(pos,float2(0.9824,4.4131),float2(0.2281,-0.2955),float2(-0.6033,0.4780),float2(-0.1544,0.1360),float2(3.2020,12.2138),29.1169);
	d+=Distance(pos,float2(1.2733,-2.4752),float2(-0.2821,-0.1180),float2(3.4862,-0.7046),float2(0.0224,0.2024),float2(-2.2714,9.7317),6.3008);
	d+=Distance(pos,float2(2.6860,2.3608),float2(-0.1486,0.2376),float2(2.0568,1.5440),float2(0.0367,0.1594),float2(-2.0396,10.2225),25.5348);
	d+=Distance(pos,float2(0.5009,0.9612),float2(0.1818,-0.1669),float2(0.0698,-2.0880),float2(0.1424,0.1063),float2(1.7980,11.2733),35.7880);
	
	return d;
}

float3 nmap(float2 t, sampler2D tx, float str)
{
	float d=1.0/1024.0;

	float xy=tex2D(tx,t).x;
	float x2=tex2D(tx,t+float2(d,0)).x;
	float y2=tex2D(tx,t+float2(0,d)).x;
	
	float s=(1.0-str)*1.2;
	s*=s;
	s*=s;
	
	return normalize(float3(x2-xy,y2-xy,s/8.0));///2.0+0.5;
}

float4 main(float2 fragCoord : TEXCOORD0) : COLOR0
{
	float time=iTime/3.0+291.0;//+43.63/3.0;

	//calculate camera by looking ahead in the tunnel
	
	float2 p1=Position(time+0.05,cama,camb,camc,camd); //position ahead
	float3 Pos=Position3D(time,cama,camb,camc,camd); //current position
	float3 oPos=Pos;
	
	float3 CamDir=normalize(float3(p1.x-Pos.x,-p1.y+Pos.y,0.1));
	float3 CamRight=normalize(cross(CamDir,float3(0,1,0)));
	float3 CamUp=normalize(cross(CamRight,CamDir));	
	float3x3 cam=float3x3(CamRight,CamUp,CamDir);

	//ray calculation	
	float2 uv=2.0*fragCoord.xy/iResolution.xy-1.0;
	float aspect=iResolution.x/iResolution.y;
	
	float3 Dir=mul(cam, normalize(float3(uv*float2(aspect,1.0),1.0)));
	
	//raymarching
	float fade=0.0;
	
	float numit=75.0; //raymarch precision
	float threshold=1.20; //defines the thickness of tunnels
	float scale=1.5; //tunnel z depth
	
	float3 Posm1=Pos;
	
	//calculate first hit
	for (float x=0.0; x<numit; x++)
	{
		if (Dist2D(Pos)<threshold)
		{
			fade=1.0-x/numit;
			break;
		}
		Posm1=Pos;
		Pos+=Dir/numit*scale;//*(1.0+x/numit);
	}

	//track back to get better resolution
	for (int x=0; x<6; x++)
	{
		float3 p2=(Posm1+Pos)/2.0;
		if (Dist2D(p2)<threshold) 
			Pos=p2;
		else
			Posm1=p2;
	}	

	//lighting	
	float3 n=normalize(float3(Dist2D(Pos+float3(0.01,0,0))-Dist2D(Pos+float3(-0.01,0,0)),
						  Dist2D(Pos+float3(0,0.01,0))-Dist2D(Pos+float3(0,-0.01,0)),
						  Dist2D(Pos+float3(0,0,0.01))-Dist2D(Pos+float3(0,0,-0.01))));
	
	//triplanar blend vector
	float3 tpn=normalize(max(float3(0,0,0),(abs(n.xyz)-float3(0.2, 0.2, 0.2))*7.0))*0.5;
	
	//position of the light - uncomment the second line to get a more interesting path
	float3 lp=Position3D(time+0.5,cama,camb,camc,camd); //current light position
	//lp=Position3D(time+0.3,lighta,lightb,lightc,lightd);
	
	float3 ld=lp-Pos;	//light direction
	float lv=1.0;
	
	float ShadowIT=15.0; //shadow precision
	
	//shadow calc
	for (float x=1.0; x<ShadowIT; x++)
		if (Dist2D(Pos+ld*(x/ShadowIT))<threshold) 
		{
			lv=0.0;
			break;
		}

	float3 tuv=Pos*float3(3.0,3.0,1.5);	//tex2D coordinates
	
	//normal mapping
	float nms=0.19;
	float3 nmx=nmap(tuv.yz,iChannel4,nms)+nmap(-tuv.yz,iChannel4,nms);
	float3 nmy=nmap(tuv.xz,iChannel5,nms)+nmap(-tuv.xz,iChannel5,nms);
	float3 nmz=nmap(tuv.xy,iChannel6,nms)+nmap(-tuv.xy,iChannel6,nms);
	
	float3 nn=normalize(nmx*tpn.x+nmy*tpn.y+nmz*tpn.z);
	
	float dd;
	//normalmapped version:
	dd=max(0.0,dot(nn,normalize(mul(float3x3(float3(1,0,0),float3(0,0,1),n),ld))));
	//standard version:
	//dd=max(0.0,dot(n,normalize(ld)));
	
	float f=dd*1.2*lv;
	float4 diff=float4(f,f,f,1)+float4(0.2,0.2,0.2,1);

	//wisp
	float w=pow(dot(normalize(Pos-oPos),normalize(lp-oPos)),5000.0);
	if (length(Pos-oPos)<length(lp-oPos)) w=0.0;	

	//texturing
	//double sampling to fix seams on tex2D edges
	float4 tx=tex2D(iChannel4,tuv.yz)+tex2D(iChannel4,-tuv.yz);
	float4 ty=tex2D(iChannel5,tuv.xz)+tex2D(iChannel5,-tuv.xz);
	float4 tz=tex2D(iChannel6,tuv.xy)+tex2D(iChannel6,-tuv.xy);
	
	float4 col=tx*tpn.x+ty*tpn.y+tz*tpn.z;
	
	return col*diff*min(1.0,fade*10.0)+w;
}
