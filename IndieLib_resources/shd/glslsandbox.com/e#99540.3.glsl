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

void main(void){	
	vec2 uv = (2.*xlv_TEXCOORD0.xy-resolution.xy)/resolution.y;

	vec3 c = trace(vec3(0,.0,-1.),vec3(uv,1));
	gl_FragColor = vec4(c, 1.);
}

