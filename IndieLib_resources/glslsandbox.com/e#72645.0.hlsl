/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
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

/*
 * Original shader from: https://www.shadertoy.com/view/NdfSzX
 */

// glslsandbox uniforms

float2 mouse=float2(1, 1);
float2 resolution=float2(1, 1);
float time=0.0;

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
#define FAR 500.
#define CASTTERRIAN 3.
#define CASTRIVER 4.
#define HASHSCALE1 .1031
#define HASHSCALE3 float3(.1031, .1030, .0973)
#define HASHSCALE4 float4(1031, .1030, .0973, .1099)
float2x2 rotate2D = float2x2(1.12, 0.1531, -.1131, 1.1623);
float2x2 rotateCloud = float2x2(1.12, 0.3531, -.2131, 1.4623);
float2 add = float2(1.0, 0.0);

float Hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}
float2 Hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);

}

float Noise( in float2 x )
{
    float2 p = floor(x);
    float2 f = frac(x);
    f = f*f*(3.0-2.0*f);
    
    float res = lerp(lerp( Hash12(p),          Hash12(p + add.xy),f.x),
                    lerp( Hash12(p + add.yx), Hash12(p + add.xx),f.x),f.y);
    return res;
}

float rand(float2 n) { 
    return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
}
float noise(float2 p){
    float2 ip = floor(p);
    float2 u = frac(p);
    u = u*u*(3.0-2.0*u);
 
    float res = lerp(
        lerp(rand(ip),rand(ip+float2(1.0,0.0)),u.x),
        lerp(rand(ip+float2(0.0,1.0)),rand(ip+float2(1.0,1.0)),u.x),u.y);
    return res*res;
}

float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float2 mod289(float2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 permute(float3 x) { return mod289(((x*34.0)+1.0)*x); }

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
float4 mod289(float4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
float4 perm(float4 x){return mod289(((x * 34.0) + 1.0) * x);}

float snoise(float3 p){
    float3 a = floor(p);
    float3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);
 
    float4 b = a.xxyy + float4(0.0, 1.0, 0.0, 1.0);
    float4 k1 = perm(b.xyxy);
    float4 k2 = perm(k1.xyxy + b.zzww);
 
    float4 c = k2 + a.zzzz;
    float4 k3 = perm(c);
    float4 k4 = perm(c + 1.0);
 
    float4 o1 = frac(k3 * (1.0 / 41.0));
    float4 o2 = frac(k4 * (1.0 / 41.0));
 
    float4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    float2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);
 
    return o4.y * d.y + o4.x * (1.0 - d.y);
}



float terrain(float2 st){
    float LOOP=5.;
    float atm=2.;
    float f=.3;
    float offset=2.4;
    float h=0.;
    for(float i=0.;i<LOOP;i++){
        h+=atm*Noise(f*st+offset);
        st=mul(rotate2D,st);
        atm*=.5;
        f*=2.5;
        offset*=2.;
    }
    // float ff=snoise(st*.2);
    //h+=pow(abs(ff),3.)*2.;
    return h;
}

float water(float2 st){
    float LOOP=5.;
    float atm=.1;
    float f=1.15;
    float offset=2.4;
    float h=0.;
    for(float i=0.;i<LOOP;i++){
        h+=atm*noise(f*st+offset+.8*iTime);
        atm*=.5;
        f*=2.;
        offset*=2.;
    }
    return h;
}

float cloud(float3 p){
    float2 st=p.xz;
    float LOOP=3.;
    float atm=.15;
    float f=.55;
    float offset=2.4;
    float h=0.;
    for(float i=0.;i<LOOP;i++){
        h+=atm*noise(f*st+offset+.0*iTime);
        atm*=.5;
        f*=2.;
        offset*=2.;
        st=mul(rotateCloud,st);
    }
    return h;
}


float random(float f){
    return frac(sin(3212.23*f+5866.56)+4343.232);
}
float curve(float x){
    float frequency=.8;
    float atm=.9;
    float value=0.;
    for(float i=0.;i<2.;i++){
        value+=atm*sin(frequency*x+3.);
        atm*=.5;
        frequency*=2.;
    }
    return value;
}



float calcCurveDif(float x){
    //if use central differential,it will shake 
    return 0.9*cos(.8*x+3.)*.8+.9*.5*cos(.8*2.*x+3.)*2.*.8;
}

float nearestRiver(float low,float high,float x,float width){
    float nearset=FAR;
    for(float i=0.;i<3.;i++){
        float z=low+(high-low)/50.*i;
        float dist=length(float2(x-curve(z),high-z));
        nearset=min(nearset,dist-width);
        if(nearset<0.){
           return nearset;
        }
    }
    return nearset;
}

float intersectRiver(float3 pos){
    float z=pos.z;
    float fw=.1*sin(z)+.05*sin(.5*z)+.6;
    float width=fw;
    float nearset=nearestRiver(z-width,z,pos.x,width);
    return nearset;
}



float2 map(float3 pos){
    float castRiver=intersectRiver(pos);
    float tH=terrain(pos.xz);
    float type=CASTTERRIAN;

    float abX=castRiver;
    tH=smoothstep(0.3,1.5,abX)*tH;
    if(tH<0.001){
        type=CASTRIVER;
        tH=water(pos.xz);
    }
   // tH=max(tH,.1);
    return float2(
            //height
            tH,
            //type
            type
        );
}

float2 mapT(float3 pos){
    float castRiver=intersectRiver(pos);
    float tH=terrain(pos.xz);
    float type=CASTTERRIAN;
    float abX=castRiver;
    tH=smoothstep(0.3,1.5,abX)*tH;
    if(tH<0.001){
        return float2(
            //height
            0.,
            //type
            CASTRIVER
        );
    }
    return float2(
            //height
            tH,
            //type
            type
        );
}

float3 calcNormal(float3 pos,float type){
    float3 result=float3(0,0,0);
    float2 delta=float2(1.,-1.)*0.5773*0.0005;
    float2 res=float2(FAR, FAR);
    return normalize(
        delta.xyy*map(pos+delta.xyy).x+
        delta.yyx*map(pos+delta.yyx).x+
        delta.yxy*map(pos+delta.yxy).x+
        delta.xxx*map(pos+delta.xxx).x
    );
}

float4 castTerrain(float3 or,float3 rd){
    float CASTLOOP=50.;
    float dist=0.01;
    float4 result=float4(0,0,0,0);
    for(float i=0.;i<CASTLOOP;i++){
        float3 target=or+dist*rd;
        float2 map=mapT(target);
        float h=map.x;
        float delta=target.y-h;
        if(delta<.01){
            return float4(
                //dist
                dist,
                //type
                map.y,
                //casted
                1.,
                //h
                h
            );
        };
        if(dist>FAR){
            result= float4(
                //dist
                dist,
                //type
                map.y,
                //casted
                0.,
                0.
            );
            break;
        };
        dist+=.27*abs(delta)+max(.002*abs(dist),.05);
    }
    return result;

}

float4 castObjects(float3 or,float3 rd){
    float CASTLOOP=150.;
    float dist=0.;
    float4 result=float4(0,0,0,0);
    for(float i=0.;i<CASTLOOP;i++){
        float3 target=or+dist*rd;
        float2 map_=map(target);
        float h=map_.x;
        float delta=target.y-h;
        if(delta<.02){
            return float4(
                //dist
                dist,
                //type
                map_.y,
                //casted
                1.,
                //h
                h
            );
        };
        if(dist>FAR){
            result= float4(
                //dist
                dist,
                //type
                map_.y,
                //casted
                0.,
                0.
            );
            break;
        };
        dist+=.19*abs(delta)+max(.002*abs(dist),.001);
    }
    return result;

}

float3 drawSky(float3 rd){
    float baseN=.45;
    float sm=.1;
    float3 col= float3(0.4431, 0.6353, 0.9882);
    float cloudV=cloud(rd);
    col+=float3(1,1,1)*cloudV;
    return col;
}

float3 getTerrainCol(float3 p){
    float3 res=float3(0.4863, 0.451, 0.3765);
    float3 baseCoord=p.xyz*11.76;
   // float ks=.1;
    // baseCoord=rotate2D*baseCoord;

    float rn=snoise(baseCoord);
    rn=rn;
    float rockn=snoise(p.xyz);
    float baseN=.5;
    float stepN=0.1;
    float heightFromWater=p.y;
    
    float hasTree=1.-smoothstep(rn+.4,rn+.4+.8*rn,heightFromWater);
    
    float darkWater=1.-smoothstep(.01+.1*rn,.01+.1*rn+.08,heightFromWater);
    float dark=snoise(p.xyz);
    res+=darkWater*float3(0.6471, 0.5333, 0.3686);
    res+=(1.-darkWater)*hasTree*float3(0.0078, 0.2392, 0.0078);
    float snow=smoothstep(2.5,2.5+.5,heightFromWater);
    res+=snow*float3(1,1,1);
    float nl=(1.-snow)*(1.-darkWater)*smoothstep(.7,.7+.01,rockn);
    return res;
}



float3 render(float3 or,float3 rd){
    float4 castResult=castObjects(or,rd);
    float3 lin=float3(0,0,0);
    float3 res=float3(0.4863, 0.451, 0.3765);
    float3 normal=float3(0.,0.,0.);
    float dist=FAR;
    float3 speNormal=normal;
    float ks=1.;
    float br=1.;
    float difR=0.4;
    float waterR=0.;
    float3 p=or+castResult.x*rd;
    if(castResult.z>0.){
        normal=calcNormal(p,castResult.y);
        speNormal=normal;
        dist=castResult.x;
    }
    if(abs(castResult.y-CASTRIVER)<0.01){
        res=float3(0.1608, 0.3804, 0.3569);
        br=40.;
        difR=.7;
        waterR=1.;
        ks=2.;
        speNormal=float3(0.,1.,0.);
        float3 red=rd+float3(0.,-(rd.y)*2.,0.);
        red=normalize(red);
        float3 from=p+.1*normal;
        float4 castRes=castTerrain(from,red);
        if(castRes.z>0.){
            if(abs(castRes.y-CASTTERRIAN)<0.01){
                float3 rP=from+castRes.x*red;
                float3 reCol=getTerrainCol(rP);
                res=res*.8+.2*reCol;
            }
        }else{
            res=res*.7+.3*drawSky(red);
           // res=drawSky(red);
        }
        float castRiver=intersectRiver(p);
        float width=-castRiver;
        res-=float3(4,4,4)*smoothstep(0., 0.6,width );
        

    }
    else if(abs(castResult.y-CASTTERRIAN)<0.01){
        ks=0.1;
        res=getTerrainCol(p);
    }
    {
    
        float3 light=normalize(float3 (-0.02,0.2,0.3));
        float3 hal=normalize(light-rd);
        float dif=clamp(dot(normal,light),0.,1.);
        float spe=pow(
            clamp(dot(speNormal,hal),0.,1.),
            br

        );
        spe*=dif;
        spe*=0.14+0.96*pow(
            clamp(1.-dot(hal,light),0. ,1. ),
            5.
        );
        float3 suncol=float3(0.9686, 0.9216, 0.7922);
        lin+=difR*res+res*.7*dif*suncol+.7*res*waterR*(1.-dif)*.75*suncol;
        
        lin+=5.*spe*float3(1.3,1.0,0.7)*ks;
        lin=step(0.1,castResult.z)*lin+(1.-step(0.1,castResult.z))*drawSky(rd);
       
        //  lin = lerp( lin, float3(0.7,0.7,0.9), 1.0-exp( -0.0001*height*height*height ) );

    }
    float3 fogCol=float3(0.9529, 0.9804, 0.9804);
    float fogRatio = smoothstep(1.,10. ,dist );
    lin=(1.-fogRatio)*lin+fogRatio*fogCol;
    float skyRatio=smoothstep(.2,1.,abs(rd.y))*(1.-castResult.z);
    lin=skyRatio*drawSky(rd)+(1.-skyRatio)*lin;
    return lin;
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
    float2 st = fragCoord.xy/iResolution.xy;
    
    st.y*=iResolution.y/iResolution.x;
    st-=.5;
    st.y+=.2;
    
    st*=5.;
    float z=.5*iTime+2.;
    float x=curve(z);
    float dXZ=calcCurveDif(z);
    float3 cameraPosition=float3(x,.5,z);
    float3 dP=normalize(float3(dXZ,0.,1.));
    float3 lookAt=normalize(dP+float3(0.,-.1,0.));
    float3 rightLook=cross(lookAt,float3(0.,1.,0.));
    rightLook=normalize(rightLook);
    float3 upLook=cross(rightLook,lookAt);
    upLook=normalize(upLook);
    float3 direction=lookAt+st.x*rightLook+st.y*upLook;
    direction=normalize(direction);

    float3 res=render(cameraPosition,direction);

    fragColor=float4(res,1.);
	return fragColor;

}
// --------[ Original ShaderToy ends here ]---------- //

