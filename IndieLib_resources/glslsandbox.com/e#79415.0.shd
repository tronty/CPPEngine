// Posted by Trisomie21
// from http://glsl.heroku.com/e#5248.0
#define BLADES 6.0
#define BIAS 0.1
#define SHARPNESS 3.0
void main() {
	vec2 fragCoord = xlv_TEXCOORD0.xy / resolution.xy;
	float blade = clamp(pow(sin(atan( fragCoord.y, fragCoord.x )*BLADES)+BIAS, SHARPNESS), 0.0, 1.0);
	vec3 color = mix(vec3(-0.34, -0.5, -1.0), vec3(0.0, -0.5, -1.0), ( fragCoord.y + 1.0) * 0.25);
	color += (vec3(0.95, 0.65, 0.30) * 1.0 / distance(vec2(0.0),  fragCoord) * 0.075);
	color += vec3(0.95, 0.45, 0.30) * min(1.0, blade *0.7) * (1.0 / distance(vec2(0.0, 0.0),  fragCoord)*0.075);
	gl_FragColor=vec4(color, 1);
}

