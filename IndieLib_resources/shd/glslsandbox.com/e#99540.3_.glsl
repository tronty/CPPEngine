uniform vec3      resolution=vec3(0);
uniform float     time=0;

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
varying vec2 xlv_TEXCOORD0;
varying vec4 color_;

[Vertex shader]

struct VsOut {
    vec4 position;
    vec2 texCoord;
    vec4 color;
};

struct VsIn {
    vec2 position;
    vec2 texCoord;
};

uniform vec4 scaleBias;

VsOut xlat_main( in VsIn In ) {
    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0));
    Out.position.xy = ((In.position.xy * scaleBias.xy) + scaleBias.zw);
    Out.position.w = 1.0;

    Out.texCoord = In.position.xy;
    xlv_TEXCOORD0 = In.position.xy;

    vec3 c = trace(vec3(0,.0,-1.),vec3(In.position.xy,1));
    Out.color=vec4(c, 1.);
    return Out;
}

void main() {
    VsOut xl_retval;
    VsIn xlt_In;
    xlt_In.position = vec2(gl_Vertex);
    xlt_In.texCoord = vec2(gl_Vertex);
    xl_retval = xlat_main( xlt_In);
    gl_Position = vec4(xl_retval.position);
    xlv_TEXCOORD0 = vec2(xl_retval.position.xy);
    color_=vec4(xl_retval.color);
}

[Fragment shader]

void main(void){
	#if 0	
	vec2 uv = (2.*xlv_TEXCOORD0.xy-resolution.xy)/resolution.y;
	vec3 c = trace(vec3(0,.0,-1.),vec3(uv,1));
	gl_FragColor = vec4(c, 1.);
	#else
	gl_FragColor = color_;
	#endif
}

