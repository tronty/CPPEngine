#define texture(a1,a2) texture2D(a1,a2)
const vec3 backgroundColor = vec3(0.0, 0.0, 0.0);
const vec2 CENTER   = vec2(0.0, 0.0);
const vec3 RED 	    = vec3(1.0, 0.0, 0.0);
const vec3 GREEN 	= vec3(0.0, 1.0, 0.0);
const vec3 BLUE 	= vec3(0.0, 0.0, 1.0);
const vec3 YELLOW   = vec3(1.0, 1.0, 0.0);
const vec3 WHITE	= vec3(1.0, 1.0, 1.0);
const vec3 GREY		= vec3(0.5, 0.5, 0.5);

const vec3 ColMercury	= vec3(0.96078, 0.90588, 0.71765);
const vec3 ColVenus		= vec3(0.95686, 0.82353, 0.38039);
const vec3 ColEarth		= vec3(0.61961, 0.79216, 0.37255);
const vec3 ColMars		= vec3(0.88235, 0.81569, 0.61961);
const vec3 ColJupiter	= vec3(0.83922, 0.67059, 0.53333);
const vec3 ColSaturn	= vec3(0.89412, 0.87059, 0.63922);
const vec3 ColUranus	= vec3(0.00784, 0.87059, 0.95686);
const vec3 ColNeptune	= vec3(0.00000, 0.51765, 0.89020);
const vec3 ColPluto		= vec3(0.62745, 0.63137, 0.64706);


void drawCircle(vec2 uv, vec3 color, float radiusCircle, float blur, vec2 pos, inout vec3 image) {
    
    float d = length(uv - pos);

    if(d <= radiusCircle)
    {
		image.r = color.r * (1.0-smoothstep(radiusCircle-blur, radiusCircle, d));
        image.b = color.b * (1.0-smoothstep(radiusCircle-blur, radiusCircle, d));
        image.g = color.g * (1.0-smoothstep(radiusCircle-blur, radiusCircle, d));
    }
}

vec2 Rotation(float distance, float multi)
{
    float x1 = distance*(cos(iTime*multi));
    float y1 = distance*(sin(iTime*multi));
    
    return vec2(x1,y1);
}

vec3 createOrbit(float m, vec2 uv) 
{ 
    vec3 colorOrbit = vec3(0.0, 0.19, 0.51);
    return mix(vec3(.0),
               colorOrbit, 
               smoothstep(1.5/iResolution.y, 0., abs(length(uv)-(m)))); 
}

void main( )
{
    vec2 uv = ( xlv_TEXCOORD0.xy / iResolution.xy ) - 0.5;  // -0.5 <> 0.5
    uv.x *= iResolution.x / iResolution.y;
	vec4 backgroundImage;// = texture(iChannel0,uv);
    backgroundImage.r -= 0.4;
    backgroundImage.b -= 0.2;
    backgroundImage.g -= 0.4;
    backgroundImage /= 2.0;
    
    gl_FragColor = backgroundImage;
    
    vec3 c = backgroundColor;
    
    
    float diam = 0.00073; // Diameter
    float dist = 0.00062; // Distance from the Sun
    float vel  = 0.031;   // Mean orbital velocity 
    
    vec3 orbitMercury 	= createOrbit(dist*188.0, uv);
    vec3 orbitVenus 	= createOrbit(dist*242.0, uv);
    vec3 orbitEarth 	= createOrbit(dist*300.0, uv);
    vec3 orbitMars 		= createOrbit(dist*345.0, uv);
    vec3 orbitJupiter 	= createOrbit(dist*425.0, uv);
    vec3 orbitSaturn 	= createOrbit(dist*542.0, uv);
    vec3 orbitUranus 	= createOrbit(dist*632.0, uv);
    vec3 orbitNeptune 	= createOrbit(dist*710.0, uv);
    vec3 orbitPluto 	= createOrbit(dist*777.0, uv);

    c += orbitMercury;
    c += orbitVenus;
    c += orbitEarth;
    c += orbitMars;
    c += orbitJupiter;
    c += orbitSaturn;
    c += orbitUranus;
    c += orbitNeptune;
    c += orbitPluto;
    
    
    drawCircle(uv, YELLOW,		70.0  * diam, 0.005, CENTER, c);
    drawCircle(uv, ColMercury,	12.0  * diam, 0.005, Rotation(dist*188.0, vel*47.89), c);
    drawCircle(uv, ColVenus,  	26.0  * diam, 0.005, Rotation(dist*242.0, vel*35.03), c);
	drawCircle(uv, ColEarth,    28.0  * diam, 0.005, Rotation(dist*300.0, vel*29.79), c);
    drawCircle(uv, ColMars,  	14.0  * diam, 0.005, Rotation(dist*345.0, vel*24.13), c);
    drawCircle(uv, ColJupiter,  56.0  * diam, 0.005, Rotation(dist*425.0, vel*13.06), c);
    drawCircle(uv, ColSaturn,  	46.0  * diam, 0.005, Rotation(dist*542.0, vel*9.64),  c);
    drawCircle(uv, ColUranus,   29.0  * diam, 0.005, Rotation(dist*632.0, vel*6.81),  c);
    drawCircle(uv, ColNeptune,  31.0  * diam, 0.005, Rotation(dist*710.0, vel*5.43),  c);
    drawCircle(uv, ColPluto,  	14.0  * diam, 0.005, Rotation(dist*777.0, vel*4.74),  c);
    
    vec2 starPos = vec2(-0.8,0.3);
	drawCircle(uv, WHITE*(0.1/sin(iTime*2.0)), 34.0  * diam, 0.06, vec2(0.6,0.1),  c);
    drawCircle(uv, WHITE*(sin(iTime*2.0)), 34.0  * diam, 0.06, vec2(0.5,-0.4),  c);
    drawCircle(uv, WHITE*(cos(iTime*3.0)+0.2), 34.0  * diam, 0.06, vec2(-0.5,-0.2),  c);
    drawCircle(uv, WHITE*(sin(iTime)+cos(iTime))*1.2, 24.0  * diam, 0.06, vec2(-0.8,-0.3),  c);
    
    drawCircle(uv, WHITE*(sin(iTime)-cos(iTime))*1.2, 34.0  * diam, 0.06, starPos,  c);
    
    // Brightness ring around the Sun
    c[0] += .03/length((mod(length(uv),2.0)-.04));
    c[1] += .03/length((mod(length(uv),2.0)-.04));
    
	gl_FragColor += vec4(c, 0.0);
}
