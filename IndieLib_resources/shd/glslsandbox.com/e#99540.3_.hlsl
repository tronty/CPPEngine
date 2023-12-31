float3      iResolution;
float4      iMouse;
float     iTime;
float3      resolution;
float4      mouse;
//float time;
float     iGlobalTime;
float4      iDate;
float     iSampleRate;
float3      iChannelResolution[4];
float     iChannelTime[4];
float2      ifFragCoordOffsetUniform;
float     iTimeDelta;
int       iFrame;
float     iFrameRate;
struct Channel {
    float3  resolution;
    //float time;
};
#define mainImage main
#define max_steps 50
#define min_step .015
#define epsilon .0034

float sdSphere(float3 v, float a){
	return length(v)-a;
}

float map(float3 v){
	// return (sdSphere(v-float3(-.0))*sdSphere(v-float3(cos(time))));
	return sdSphere(v, .3);
}

// iquilezles.org/articles/normalsSDF
float3 normal(float3 p){
    const float h = 0.0001; 
    const float2 k = float2(1,-1);
    return normalize(k.xyy*map(p + k.xyy*h)+ 
                     k.yyx*map(p + k.yyx*h)+ 
                     k.yxy*map(p + k.yxy*h)+ 
                     k.xxx*map(p + k.xxx*h));
}

float3 trace(float3 o, float3 d){
	float dist = 0., t = 0.;
	for(int i = 0; i < max_steps; i++){
		dist = map(o+d*t);
		if(dist < epsilon){
			float l = .3+.6*pow(max(dot(normalize(float3(cos(time),.3, -.9)),normal(o+d*t)),-.1),1.);
			return float3(l);
		}
		t+= dist;
	}
	return float3(0);
}

float3 map2(float3 v){
	return v;
}

// iquilezles.org/articles/normalsSDF
float3 normal2(float3 p){
    const float h = 0.0001; 
    const float2 k = float2(1,-1);
    return normalize(k.xyy*map2(p + k.xyy*h)+ 
                     k.yyx*map2(p + k.yyx*h)+ 
                     k.yxy*map2(p + k.yxy*h)+ 
                     k.xxx*map2(p + k.xxx*h));
}

float3 trace2(float3 o, float3 d){
	float dist = 0., t = 0.;
	for(int i = 0; i < max_steps; i++){
		dist = length(map2(o+d*t));
		if(dist < epsilon){
			float l = .3+.6*pow(max(dot(normalize(float3(cos(time),.3, -.9)),normal2(o+d*t)),-.1),1.);
			return float3(l);
		}
		t+= dist;
	}
	return float3(0);
}

struct VsOut {
    float4 position;
    float3 color;
    float2 uv;
};
[Vertex shader]

struct VsIn2 {
    float2 position;
    float2 uv;
};
struct VsIn3 {
    float3 position;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float3 Color;
    float2 uv;
};
float4x4 worldViewProj;
VsOut main2( in VsIn2 In ) {
    VsOut Out = VsOut(float4(0.0, 0.0, 0.0, 0.0), float2(0.0, 0.0));
    Out.position = float4( In.position.x, In.position.y, 0.0, 1.0);
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0-In.uv.y;
    return Out;
}
VsOut main(VsIn2 xlt_In) {
    VsOut xl_retval;
    xl_retval = main2( xlt_In);
    float2 uv_ = float2(xl_retval.uv);
#if 0
	float2 uv = (2.*uv_-resolution.xy)/resolution.y;
#else
	float2 uv = uv_;
#endif
	float3 c = trace(float3(0,.0,-1.),float3(uv, 1));
	float3 p = trace2(float3(0,.0,-1.),float3(uv, 1));
#if 0
	xl_retval.position = float4(xl_retval.position);
#elif 0
	xl_retval.position = float4(p, 1);
#else
	float meridianPart = uv.x;
	float parallelPart = uv.y;
	float angle1 = meridianPart * 3.14159265 * 2.0;
	float angle2 = ( parallelPart - 0.5 ) * 3.14159265;
	float cos_angle1 = cos( angle1 );
	float sin_angle1 = sin( angle1 );
	float cos_angle2 = cos( angle2 );
	float sin_angle2 = sin( angle2 );
	xl_retval.position.z = cos_angle1 * cos_angle2;
	xl_retval.position.x = sin_angle1 * cos_angle2;
	xl_retval.position.y = sin_angle2;
	xl_retval.position = mul( float4( VertexPosition, 1.0 ), worldViewProj );
#endif
#if 1
	xl_retval.uv= uv;
#else
	xl_retval.uv= float2( 1.0 - meridianPart, 1.0 - parallelPart );
#endif
	xl_retval.color=c;
	return xl_retval;
}

[Fragment shader]

float4 main(VsOut in):COLOR0{
#if 0
	float2 uv = (2.*in.uv-resolution.xy)/resolution.y;
	float3 c = trace(float3(0,.0,-1.),float3(uv,1));
	return float4(c, 1.);
#else
	return float4(in.color, 1.);
#endif
}

