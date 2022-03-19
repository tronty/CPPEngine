#ifdef GL_ES
precision lowp float;
#endif

#define PI 3.14159265358979
#define N 10
void main( void ) {
	vec2 fragCoord=xlv_TEXCOORD0.xy/iResolution.xy-0.5;
	float size = 0.25;
	float dist = 0.100;
	float ang = 0.0;
	vec2 pos = vec2(0.0,0.0);
	vec3 color = vec3(0.1);
	
	for(int i=0; i<N; i++){
		float r = 0.3;
		ang += PI / (float(N)*0.5)+(time/60.0);
		pos = vec2(cos(ang),sin(ang))*r*sin(time+ang/.3);				  
		dist += size / distance(pos, fragCoord);
		vec3 c = vec3(0.03, 0.05, 0.1);
		color = c*dist;
	}
	gl_FragColor = vec4(color, 1.0);
}

