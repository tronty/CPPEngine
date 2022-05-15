// post processing in 2d
// crosses, gamma, vignetting

float grid(in vec2 st) {
    vec2 st1 = 2.0*(abs(fract(st))-0.5);
    float d1 = min(1.0-abs(st1.x),1.0-abs(st1.y));
    float d2 = clamp(max(1.0-abs(st1.x),1.0-abs(st1.y))-0.3,0.0,1.0);
	float d = d1+d2;
	return d;    
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

    vec2 uv = fragCoord.xy / iResolution.xy;
    // read in the image from Buffer B
	vec3 col = texture(iChannel0, uv).rgb;

    uv.y *= iResolution.y/iResolution.x;
    // crosses
    uv -= 0.5;
    float aa = 2.0/iResolution.y;
    
	#define NUMBARS 5.0
    uv *= NUMBARS;
	float dist = grid(uv);
    float bars = 1.0-smoothstep(0.01, 0.01 + aa, dist);
    col = mix(col, vec3(0.4), 1.0-smoothstep(0.001*NUMBARS, 0.001*NUMBARS + aa, dist));
    
    // gamma correction
    col = pow(abs(col), vec3(1. / 2.2));
    
    // iq's vignetting
 	vec2 q = fragCoord/iResolution.xy;
    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.3 );
    
    // send it to the screen
    fragColor = vec4(col, 1.0);
}

