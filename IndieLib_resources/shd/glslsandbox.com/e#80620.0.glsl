/*
// arxeon was here
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;
*/
#define PI 3.14159265358979

vec3 color = 0.3*vec3(1.0,0.0,0.3);
float d2y(float d){return 1./(0.2+d);}
float radius = 0.3;

float fct(vec2 p, float r){
	float a = 3.*mod(atan(p.y, p.x)+time, 2.*PI);
	
	
	float scan = 0.*1.;
	return (d2y(a)+scan)*(1.-step(radius,r));
}

	
float circle(vec2 p, float r){
	float d=distance(r, radius);
	return d2y(10.*d);
}

float grid(vec2 p, float y){
	float a = 0.2;
	float res = 23.141592653589;
	float e = 0.1;
	vec2 pi = fract(p*res);
	pi = step(e, pi);
	return a * y * pi.x * pi.y;
}

void main( void ) {
	
	vec2 position = (( xlv_TEXCOORD0.xy )-0.5*resolution.xy)/ resolution.y ;
	position/=cos(.0*length(position));
	float y  = 0.0;
	
	float dc = length(position);
	
	y+=fct(position, dc);
	y+=circle(position, dc);
	y+=grid(position, y);
	y=pow(y,1.80);
	gl_FragColor = vec4( sqrt(y)*color,1. );
}
