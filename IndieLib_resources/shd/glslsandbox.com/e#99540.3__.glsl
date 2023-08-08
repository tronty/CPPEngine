
vec2 raySphereIntersect(vec3 r0, vec3 rd, vec3 s0, float sr) {
    float a = dot(rd, rd);
    vec3 s0_r0 = r0 - s0;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sr * sr);
	float disc = b * b - 4.0 * a* c;
    if (disc < 0.0) {
        return vec2(-1.0, -1.0);
    }else{
		return vec2(-b - sqrt(disc), -b + sqrt(disc)) / (2.0 * a);
	}
}

uniform vec3      resolution=vec3(0);
uniform float     time=0;

#define max_steps 50
#define min_step .015
#define epsilon .0034

float sdSphere(vec3 v, float a){
	return length(v)-a;
}

float map(vec3 v){
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
			return vec3(vec2(length(o),length(d*t)),1);
		}
		t+= dist;
	}
	return vec3(0);
}

vec4 color(vec2 auv){
#if 0
	//vec2 uv = (2.*auv.xy-resolution.xy)/resolution.y;
	vec2 uv = auv.xy;
	//vec3 c = trace(vec3(0,.0,-1.),vec3(uv,1));
	vec3 c = trace(vec3(0,.0,-1.),vec3(1));
#else
  vec2 uv = auv.xy;///resolution.xy;
  //uv = uv * 2.0 - 1.0;
  //uv.x *= resolution.x / resolution.y;

  //float theta = time * 0.25;
  vec3 r = normalize(vec3(uv, 1.0));
  vec3 o = vec3(0.0, 0.0, -3.0) * time * -0.5;
  //r.xy *= mat2(cos(theta), -sin(theta), sin(theta), cos(theta));

  vec3 c = trace(o, r);
#endif
	return vec4(c, 1.);
}

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

float g_SphereMeridianSlices=128.0;
float g_SphereParallelSlices=128.0;

uniform vec4 scaleBias;
varying vec2 xlv_TEXCOORD0;
varying vec4 color_;

#if 1
VsOut xlat_main( in VsIn input_)
{
    VsOut output_;
    #if 0
    float inputindex = 1.0;
    float meridianPart = ( inputindex //% 
				/ ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( inputindex / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    #endif
    float R=1.0;
    float angle1 = atan(input_.z, input_.x);//meridianPart * 3.14159265 * 2.0;
    float angle2 = acos(input_.y / R);//( parallelPart - 0.5 ) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    vec4 VertexPosition;
    VertexPosition.z = cos_angle1 * cos_angle2;
    VertexPosition.x = sin_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    VertexPosition.w = 1.0;
    
    //gl_Position = mul( vec4( VertexPosition, 1.0 ), g_ModelViewProj );
    gl_Position = VertexPosition;
    xlv_TEXCOORD0 = VsIn.texCoord;
    color_=color(xlv_TEXCOORD0);

    #if 0
    output_.position = VertexPosition;
    output_.texCoord = VsIn.texCoord;
    output_.color=color(xlv_TEXCOORD0);
    #endif

    return output_;
};
#else
VsOut xlat_main( in VsIn In ) {
    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0));
    Out.position.xy = ((In.position.xy * scaleBias.xy) + scaleBias.zw);
    Out.position.w = 1.0;
#if 0
    Out.texCoord = In.texCoord;
    xlv_TEXCOORD0 = In.texCoord;
#else
    vec2 UV=raySphereIntersect(vec3 r0, vec3 rd, vec3 s0, float sr)
#endif
    Out.color=color(xlv_TEXCOORD0);
    return Out;
}
#endif

void main() {
    VsOut xl_retval;
    VsIn xlt_In;
    xlt_In.position = vec2(gl_Vertex);
    xlt_In.texCoord = vec2(gl_MultiTexCoord0);
    xl_retval = xlat_main( xlt_In);
    gl_Position = vec4(xl_retval.position);
    xlv_TEXCOORD0 = vec2(xl_retval.texCoord);
    color_=vec4(xl_retval.color);
}

[Fragment shader]

varying vec2 xlv_TEXCOORD0;
varying vec4 color_;

void main() {
    gl_FragData[0] = color_;
}

