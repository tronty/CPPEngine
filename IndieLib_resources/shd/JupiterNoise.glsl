#ifdef GL_ES 
#define LOWP lowp
#define MED mediump
#define HIGH highp
precision highp float;
#else
#define MED
#define LOWP
#define HIGH
#endif

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise2(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

uniform vec3 u_planetColor0=vec3(1,0,0);
uniform vec3 u_planetColor1=vec3(0,1,0);
uniform vec3 u_planetColor2=vec3(0,0,1);
 
float u_random0=random(vec2(0));
float u_random1=random(vec2(0));
float u_random2=random(vec2(0));
float u_random3=random(vec2(0));
float u_random4=random(vec2(0));
float u_random5=random(vec2(0));
float u_random6=random(vec2(0));
float u_random7=random(vec2(0));
float u_random8=random(vec2(0));
float u_random9=random(vec2(0));
 
// INSERT HERE THE NOISE FUNCTIONS ...
 
float pnoise2(vec2 P, float period) {
    return noise2(P*period);// ??? , vec2(period, period));
}
 
float pnoise1(float x, float period) {
    return pnoise2(vec2(x, 0.0), period);
}
 
vec3 toColor(float value) {
    float r = clamp(-value, 0.0, 1.0);
    float g = clamp(value, 0.0, 1.0);
    float b = 0.0;
    return vec3(r, g, b);
}
 
float planetNoise(vec2 P) {
    vec2 rv1 = vec2(u_random0, u_random1);
    vec2 rv2 = vec2(u_random2, u_random3);
    vec2 rv3 = vec2(u_random4, u_random5);
    vec2 rv4 = vec2(u_random6, u_random7);
    vec2 rv5 = vec2(u_random8, u_random9);
 
    float r1 = u_random0 + u_random2;
    float r2 = u_random1 + u_random2;
    float r3 = u_random2 + u_random2;
    float r4 = u_random3 + u_random2;
    float r5 = u_random4 + u_random2;
 
    float noise = 0.0; 
    noise += pnoise2(P+rv1, 10.0) * (0.2 + r1 * 0.4);
    noise += pnoise2(P+rv2, 50.0) * (0.2 + r2 * 0.4);
    noise += pnoise2(P+rv3, 100.0) * (0.3 + r3 * 0.2);
    noise += pnoise2(P+rv4, 200.0) * (0.05 + r4 * 0.1);
    noise += pnoise2(P+rv5, 500.0) * (0.02 + r4 * 0.15);
 
    return noise;
}
 
float jupiterNoise(vec2 texCoords) {
    float r1 = u_random0;
    float r2 = u_random1;
    float r3 = u_random2;
    float r4 = u_random3;
    float r5 = u_random4;
    float r6 = u_random5;
    float r7 = u_random6;
    float distEquator = abs(texCoords.t - 0.5) * 2.0;
    float noise = planetNoise(vec2(texCoords.x+distEquator*0.6, texCoords.y));
 
    float distPol = 1.0 - distEquator;
    float disturbance = 0.0;
    disturbance += pnoise1(distPol+r1, 3.0+r4*3.0) * 1.0;
    disturbance += pnoise1(distPol+r2, 9.0+r5*5.0) * 0.5;
    disturbance += pnoise1(distPol+r3, 20.0+r6*10.0) * 0.1;
    disturbance = disturbance*disturbance*2.0;
    float noiseFactor = r7 * 0.3;
    float noiseDistEquator = distEquator + noise * noiseFactor * disturbance;
    return noiseDistEquator;
}
 
float jupiterHeight(float noise) {
    return noise * 5.0;
}
 
vec3 planetColor(float distEquator) {
    float r1 = u_random0 + u_random3;
    float r2 = u_random1 + u_random3;
    float r3 = u_random2 + u_random3;
    float r4 = u_random3 + u_random3;
    float r5 = u_random4 + u_random3;
    float r6 = u_random5 + u_random3;
 
    float r7 = u_random6 + u_random3;
    float r8 = u_random7 + u_random3;
 
    vec3 color1 = u_planetColor0; 
    vec3 color2 = u_planetColor1; 
    vec3 color3 = u_planetColor2; 
 
    float v1 = pnoise1(distEquator+r1, 2.0 + r4*15.0) * r7;
    float v2 = pnoise1(distEquator+r2, 2.0 + r5*15.0) * r8;
 
    vec3 mix1 = mix(color1, color2, v1);
    vec3 mix2 = mix(mix1, color3, v2);
    return mix2;
}
 
void main() {
    float noise = jupiterNoise(xlv_TEXCOORD0);
    vec3 color = planetColor(noise);
 
    gl_FragColor.rgb = color;
}

