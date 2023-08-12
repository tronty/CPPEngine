uniform vec3      iResolution;
uniform vec4      iMouse;
uniform float     iTime;
uniform vec3      resolution;
uniform vec4      mouse;
uniform float     time;
uniform float     iGlobalTime;
uniform vec4      iDate;
uniform float     iSampleRate;
uniform vec3      iChannelResolution[4];
uniform float     iChannelTime[4];
uniform vec2      ifFragCoordOffsetUniform;
uniform float     iTimeDelta;
uniform int       iFrame;
uniform float     iFrameRate;
struct Channel {
    vec3  resolution;
    float   time;
};
varying vec3 xlv_position;
varying vec3 xlv_Normal;
varying vec3 xlv_Binormal;
varying vec3 xlv_Tangent;
varying vec3 xlv_Color;
varying vec2 xlv_TEXCOORD0;
#define mainImage main
#define max_steps 50
#define min_step .015
#define epsilon .0034

float sdSphere(vec3 v, float a){
	return length(v)-a;
}

float map(vec3 v){
	// return (sdSphere(v-vec3(-.0))*sdSphere(v-vec3(cos(time))));
	return sdSphere(v, .3);
}

// iquilezles.org/articles/normalsSDF
vec3 normal(vec3 p){
    const float h = 0.0001; 
    const vec2 k = vec2(1,-1);
    return normalize(k.xyy*map(p + k.xyy*h)+ 
                     k.yyx*map(p + k.yyx*h)+ 
                     k.yxy*map(p + k.yxy*h)+ 
                     k.xxx*map(p + k.xxx*h));
}

vec3 trace(vec3 o, vec3 d){
	float dist = 0., t = 0.;
	for(int i = 0; i < max_steps; i++){
		dist = map(o+d*t);
		if(dist < epsilon){
			float l = .3+.6*pow(max(dot(normalize(vec3(cos(time),.3, -.9)),normal(o+d*t)),-.1),1.);
			return vec3(l);
		}
		t+= dist;
	}
	return vec3(0);
}

[Vertex shader]

struct VsOut {
    vec4 position;
    vec2 uv;
};
struct VsIn2 {
    vec2 position;
    vec2 uv;
};
struct VsIn3 {
    vec3 position;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec3 Color;
    vec2 uv;
};
uniform mat4 worldViewProj;
VsOut main2( in VsIn2 In ) {
    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0));
    Out.position = vec4( In.position.x, In.position.y, 0.0, 1.0);
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0-In.uv.y;
    return Out;
}
void main() {
    VsOut xl_retval;
    VsIn2 xlt_In;
    xlt_In.position = vec2(gl_Vertex);
    xlt_In.uv = vec2(gl_MultiTexCoord0);
    xl_retval = main2( xlt_In);
    gl_Position = vec4(xl_retval.position);
    xlv_TEXCOORD0 = vec2(xl_retval.uv);	
	vec2 uv = (2.*xlv_TEXCOORD0.xy-resolution.xy)/resolution.y;
	vec3 c = trace(vec3(0,.0,-1.),vec3(uv,1));
	xlv_Color=c;
}

[Fragment shader]

void main(void){
#if 1
	vec2 uv = (2.*xlv_TEXCOORD0.xy-resolution.xy)/resolution.y;
	vec3 c = trace(vec3(0,.0,-1.),vec3(uv,1));
	gl_FragColor = vec4(c, 1.);
#else
	gl_FragColor = vec4(xlv_Color, 1.);
#endif
}

