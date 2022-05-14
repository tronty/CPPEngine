/*
 * Original shader from: https://www.shadertoy.com/view/NdfSzX
 */
#if 0
#ifdef GL_ES
precision mediump float;
#endif

// glslsandbox uniforms
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution
#endif

// --------[ Original ShaderToy begins here ]---------- //
#define FAR 500.
#define CASTTERRIAN 3.
#define CASTRIVER 4.
#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)
const mat2 rotate2D = mat2(1.12, 0.1531, -.1131, 1.1623);
const mat2 rotateCloud = mat2(1.12, 0.3531, -.2131, 1.4623);
vec2 add = vec2(1.0, 0.0);

float Hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}
vec2 Hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return fract((p3.xx+p3.yz)*p3.zy);

}

float Noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    float res = mix(mix( Hash12(p),          Hash12(p + add.xy),f.x),
                    mix( Hash12(p + add.yx), Hash12(p + add.xx),f.x),f.y);
    return res;
}

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);
 
    float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float snoise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);
 
    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);
 
    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);
 
    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));
 
    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);
 
    return o4.y * d.y + o4.x * (1.0 - d.y);
}



float terrain(vec2 st){
    const float LOOP=5.;
    float atm=2.;
    float f=.3;
    float offset=2.4;
    float h=0.;
    for(float i=0.;i<LOOP;i++){
        h+=atm*Noise(f*st+offset);
        st=rotate2D*st;
        atm*=.5;
        f*=2.5;
        offset*=2.;
    }
    // float ff=snoise(st*.2);
    //h+=pow(abs(ff),3.)*2.;
    return h;
}

float water(vec2 st){
    const float LOOP=5.;
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

float cloud(vec3 p){
    vec2 st=p.xz;
    const float LOOP=3.;
    float atm=.15;
    float f=.55;
    float offset=2.4;
    float h=0.;
    for(float i=0.;i<LOOP;i++){
        h+=atm*noise(f*st+offset+.0*iTime);
        atm*=.5;
        f*=2.;
        offset*=2.;
        st=rotateCloud*st;
    }
    return h;
}


float random(float f){
    return fract(sin(3212.23*f+5866.56)+4343.232);
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
        float dist=length(vec2(x-curve(z),high-z));
        nearset=min(nearset,dist-width);
        if(nearset<0.){
           return nearset;
        }
    }
    return nearset;
}

float intersectRiver(vec3 pos){
    float z=pos.z;
    float fw=.1*sin(z)+.05*sin(.5*z)+.6;
    float width=fw;
    float nearset=nearestRiver(z-width,z,pos.x,width);
    return nearset;
}



vec2 map(vec3 pos){
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
    return vec2(
            //height
            tH,
            //type
            type
        );
}

vec2 mapT(vec3 pos){
    float castRiver=intersectRiver(pos);
    float tH=terrain(pos.xz);
    float type=CASTTERRIAN;
    float abX=castRiver;
    tH=smoothstep(0.3,1.5,abX)*tH;
    if(tH<0.001){
        return vec2(
            //height
            0.,
            //type
            CASTRIVER
        );
    }
    return vec2(
            //height
            tH,
            //type
            type
        );
}

vec3 calcNormal(vec3 pos,float type){
    vec3 result=vec3(0.);
    vec2 delta=vec2(1.,-1.)*0.5773*0.0005;
    vec2 res=vec2(FAR);
    return normalize(
        delta.xyy*map(pos+delta.xyy).x+
        delta.yyx*map(pos+delta.yyx).x+
        delta.yxy*map(pos+delta.yxy).x+
        delta.xxx*map(pos+delta.xxx).x
    );
}

vec4 castTerrain(vec3 or,vec3 rd){
    const float CASTLOOP=50.;
    float dist=0.01;
    vec4 result=vec4(0.);
    for(float i=0.;i<CASTLOOP;i++){
        vec3 target=or+dist*rd;
        vec2 map=mapT(target);
        float h=map.x;
        float delta=target.y-h;
        if(delta<.01){
            return vec4(
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
            result= vec4(
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

vec4 castObjects(vec3 or,vec3 rd){
    const float CASTLOOP=150.;
    float dist=0.;
    vec4 result=vec4(0.);
    for(float i=0.;i<CASTLOOP;i++){
        vec3 target=or+dist*rd;
        vec2 map=map(target);
        float h=map.x;
        float delta=target.y-h;
        if(delta<.02){
            return vec4(
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
            result= vec4(
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
        dist+=.19*abs(delta)+max(.002*abs(dist),.001);
    }
    return result;

}

vec3 drawSky(vec3 rd){
    float baseN=.45;
    float sm=.1;
    vec3 col= vec3(0.4431, 0.6353, 0.9882);
    float cloudV=cloud(rd);
    col+=vec3(1.)*cloudV;
    return col;
}

vec3 getTerrainCol(vec3 p){
    vec3 res=vec3(0.4863, 0.451, 0.3765);
    vec3 baseCoord=p.xyz*11.76;
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
    res+=darkWater*vec3(0.6471, 0.5333, 0.3686);
    res+=(1.-darkWater)*hasTree*vec3(0.0078, 0.2392, 0.0078);
    float snow=smoothstep(2.5,2.5+.5,heightFromWater);
    res+=snow*vec3(1. );
    float nl=(1.-snow)*(1.-darkWater)*smoothstep(.7,.7+.01,rockn);
    return res;
}



vec3 render(vec3 or,vec3 rd){
    vec4 castResult=castObjects(or,rd);
    vec3 lin=vec3(0.);
    vec3 res=vec3(0.4863, 0.451, 0.3765);
    vec3 normal=vec3(0.,0.,0.);
    float dist=FAR;
    vec3 speNormal=normal;
    float ks=1.;
    float br=1.;
    float difR=0.4;
    float waterR=0.;
    vec3 p=or+castResult.x*rd;
    if(castResult.z>0.){
        normal=calcNormal(p,castResult.y);
        speNormal=normal;
        dist=castResult.x;
    }
    if(abs(castResult.y-CASTRIVER)<0.01){
        res=vec3(0.1608, 0.3804, 0.3569);
        br=40.;
        difR=.7;
        waterR=1.;
        ks=2.;
        speNormal=vec3(0.,1.,0.);
        vec3 red=rd+vec3(0.,-(rd.y)*2.,0.);
        red=normalize(red);
        vec3 from=p+.1*normal;
        vec4 castRes=castTerrain(from,red);
        if(castRes.z>0.){
            if(abs(castRes.y-CASTTERRIAN)<0.01){
                vec3 rP=from+castRes.x*red;
                vec3 reCol=getTerrainCol(rP);
                res=res*.8+.2*reCol;
            }
        }else{
            res=res*.7+.3*drawSky(red);
           // res=drawSky(red);
        }
        float castRiver=intersectRiver(p);
        float width=-castRiver;
        res-=vec3(.4)*smoothstep(0., 0.6,width );
        

    }
    else if(abs(castResult.y-CASTTERRIAN)<0.01){
        ks=0.1;
        res=getTerrainCol(p);
    }
    {
    
        vec3 light=normalize(vec3 (-0.02,0.2,0.3));
        vec3 hal=normalize(light-rd);
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
        vec3 suncol=vec3(0.9686, 0.9216, 0.7922);
        lin+=difR*res+res*.7*dif*suncol+.7*res*waterR*(1.-dif)*.75*suncol;
        
        lin+=5.*spe*vec3(1.3,1.0,0.7)*ks;
        lin=step(0.1,castResult.z)*lin+(1.-step(0.1,castResult.z))*drawSky(rd);
       
        //  lin = mix( lin, vec3(0.7,0.7,0.9), 1.0-exp( -0.0001*height*height*height ) );

    }
    vec3 fogCol=vec3(0.9529, 0.9804, 0.9804);
    float fogRatio = smoothstep(1.,10. ,dist );
    lin=(1.-fogRatio)*lin+fogRatio*fogCol;
    float skyRatio=smoothstep(.2,1.,abs(rd.y))*(1.-castResult.z);
    lin=skyRatio*drawSky(rd)+(1.-skyRatio)*lin;
    return lin;
}

void mainImage_( out vec4 fragColor, in vec2 fragCoord ){
    vec2 st = fragCoord.xy/iResolution.xy;
    
    st.y*=iResolution.y/iResolution.x;
    st-=.5;
    st.y+=.2;
    
    st*=5.;
    float z=.5*iTime+2.;
    float x=curve(z);
    float dXZ=calcCurveDif(z);
    vec3 cameraPosition=vec3(x,.5,z);
    vec3 dP=normalize(vec3(dXZ,0.,1.));
    vec3 lookAt=normalize(dP+vec3(0.,-.1,0.));
    vec3 rightLook=cross(lookAt,vec3(0.,1.,0.));
    rightLook=normalize(rightLook);
    vec3 upLook=cross(rightLook,lookAt);
    upLook=normalize(upLook);
    vec3 direction=lookAt+st.x*rightLook+st.y*upLook;
    direction=normalize(direction);

    vec3 res=render(cameraPosition,direction);

    fragColor=vec4(res,1.);

}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage_(gl_FragColor, xlv_TEXCOORD0.xy);
}

