uniform sampler2D iChannel0;
/* [Final author]: Twenkid (Todor "Tosh" Arnaudov)
                   Mapping the craters to the sphere + antialiasing and a bug fix (on Intel HD4000/Win).
                   Version: 29.12.2017

   [Author of the craters shader]: NickWest "Craters on craters": https://www.shadertoy.com/view/ldtXWf
   [Credits]: also to BigWing, DaveHoskins (noise, remap),
             tudordot (sample ray tracing and mapping to sphere coordinates),
             the mathematicians and graphics programmers from the past.

    [Future work]
         It will be way faster with one additional buffer: the craters should be rendered flat to a texture in the first frame,
     then sampled from there. I didn't have enough "passion" now to complete it, though.
                                     
 Antialiasing: AA3 - 3 samples, AA5 - 5 samples.
 If none is defined it's fast, but there's annoying "point noise" flickering in windowed mode.
 There is still noise/flickering in window, though.
 The edges should be smoother - maybe returning the angle/spherical coordinates and reducing the details?
 ...
 There was an annoying bug on an HD4000, maybe something with small numbers and precision. 
 The shader runs slowly, but looks smoother on an old WebGL 1.0-level integrated Intel GPU/Linux (late Core 2 Duo era).

 On an Intel HD4000/Windows/Chrome it requires the iTime to be incremented, say: (#54)
    pos.x += 1000. + iTime * animSpeed;
    pos.y -=  1000.. + iTime * animSpeed * 0.25;

 Otherwise the picture is a strange "griddy liney noise" or has "phantom" flickering black circles when the constants are not big enough.
 
+ Buggy also on: GF8600, Win7/Chrome
*/

//#define FLAT //original look by NickWest
//#define SHADOW //shadows from NickWest, commented in his shader
#define AA3


const vec3 circlePos = vec3(0.0);
const float circleRad = 0.9;

const vec3 lightPos = vec3(-0.3, -0.7, 2.5);
const vec3 viewerPos = vec3(-2, -0.7, 3.0);
const vec3 ambientCol = vec3(0.3);
const vec3 lightCol = vec3(0.23, 0.2, 0.22);



//BigWing's noise
float N1(float t) { 	return fract(sin(t*10234.324)*123423.23512); }
vec3 N31(float p) {
    //  3 out, 1 in... DAVE HOSKINS
   vec3 p3 = fract(vec3(p) * vec3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}


float N2(vec2 p)
{	// Dave Hoskins - https://www.shadertoy.com/view/4djSRW
	vec3 p3  = fract(vec3(p.xyx) * vec3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec2 N22(vec2 p)
{	// Dave Hoskins - https://www.shadertoy.com/view/4djSRW
    //modified to vec2, vec2
	vec2 p2  = fract(vec2(p.xyx) * vec2(443.897, 441.423)); // 437.195));
    p2 += dot(p2, p2.yx + 19.19);
    return fract(vec2((p2.x + p2.y)*p2.x, (p2.y+p2.y)*p2.x));
}

vec2 fbm(vec2 v){
    return N22(v)*0.5 + vec2(0.25*N1(v.x)) + vec2(0.25*N1(v.y));
}

// Crater in -1.0 to 1.0 space
float crater(in vec2 pos) {
    float len = length(pos);
    float pi = 3.1415926535;
    float x = clamp(pow(len, 4.0) * 8.0, pi*0.5, pi*2.5);
    float t = clamp(len, 0.0, 1.0);        
    //x=0.5; t=0.5;
    return sin(-x) + 0.5 - 0.5 * cos(t * pi);
}

vec2 pseudoRand(in vec2 uv) {
    // from http://gamedev.stackexchange.com/questions/32681/random-number-hlsl
    float noiseX = (fract(sin(dot(uv, vec2(12.9898,78.233)      )) * 43758.5453));
    float noiseY = (fract(sin(dot(uv, vec2(12.9898,78.233) * 2.0)) * 43758.5453));
    return vec2(noiseX, noiseY);}

vec2 pseudoRand1(in vec2 uv) {
   // uv/=abs(fract(uv));
   // return -texture(iChannel0, uv).rg*2.;
    // from http://gamedev.stackexchange.com/questions/32681/random-number-hlsl 
    //Changed consts:
    float noiseX = (fract(sin(dot(uv, vec2(12.9898,78.433)      )) * 4375894.5453));
    float noiseY = (fract(sin(dot(uv, vec2(12.9898,78.433) * 2.0)) * 4375894.5453));
  //  float noiseX = (fract(sin(dot(uv, vec2(12.9898,78.233)      )) * 43758.5453));
  //  float noiseY = (fract(sin(dot(uv, vec2(12.9898,78.233) * 2.0)) * 43758.5453));
    
    //noiseX = texture(iChannel0, vec2(noiseX, noiseY)).x;
    //noiseY = texture(iChannel0, vec2(noiseY, noiseX)).x;
    return vec2(noiseX, noiseY);
}

float repeatingCraters(in vec2 pos, in float repeat, in float scaleWeight) {
    vec2 pos01 = fract(pos * vec2(repeat));
    vec2 index = (pos * vec2(repeat) - pos01) / repeat;
    vec2 scale = pseudoRand(index);
    float craterY = crater(vec2(2.0) * (pos01 - vec2(0.5)));
    return mix(1.0, pow(0.5*(scale.x + scale.y), 4.0), scaleWeight) * craterY; 
}

float remap(float a, float b, float c, float d, float t) {
	return ((t-a) / (b-a)) * (d-c) + c;
}

float getY(in vec2 pos, float scale) {    
    float r = 0.5;
    float y = 0.5;
    y=0.;
       
    //WebGL 1.0       
    for(int i=0;i<int(100);++i) {
        float f01 = float(i) / float(99.);
        float magnitude = f01*f01*scale;
        vec2 offs = pseudoRand(vec2(float(i+2), float(i+7)*float(i+7)));       
        float repeat = 0.5 / (magnitude + 0.0001/scale);
        y += magnitude * repeatingCraters(pos+offs, repeat, 1.0);       
    }                  
  return y;
}

vec3 spherePos(vec2 pos, vec2 center, float rad)
{
    vec2 dir = pos - center;
    return vec3(dir, sqrt(rad * rad - dot(dir, dir)));
}

vec2 sphereMap(vec3 pos, float rad)
{
    return vec2(atan(pos.z, pos.x), acos(pos.y / rad));
}

vec3 light(vec3 pos, vec3 inColor)
{
    vec3 norm = normalize(pos);
    vec3 lightDir = normalize(lightPos - pos);
    
    float cosIncidence = dot(lightDir, norm);
    float diffuse = clamp(cosIncidence, 0., 1.);
    
    vec3 refl = reflect(lightDir, norm);
    vec3 viewDir = normalize(pos - viewerPos);

    float specularTerm = clamp(dot(viewDir, refl), 0., 1.);
    float specular = pow(specularTerm, 4.);
    
    return vec3(ambientCol * inColor.xyz + lightCol * (diffuse + specular));
}

vec4 craters(vec2 xlv_TEXCOORD0){
    float animSpeed = 0.22;
    vec2 pos = (xlv_TEXCOORD0.xy - iResolution.xy*0.5) / vec2(iResolution.y);
    pos += vec2(1.0); // avoid negative coords

    gl_FragColor = vec4(0.);
    gl_FragColor.a = 1.;
   // if ((pos.x > 0.5)||(pos.y > 1.)) return;
    
    //~Depth of craters/bumpyness
    vec2 offs = vec2(0.001, -0.001);
    //vec2(0.003 -0.003);
    
    //After certain tenths of seconds - glitches in the image
    //if iTime is not < .0 glitchy lines
    ///*
   // pos.x += fract(iTime) * animSpeed;
   // pos.y -= fract(iTime) * animSpeed * 0.25;
    
     float t = iTime+18000. ;//55555.;
    
     pos.x += t/5. * animSpeed;
     pos.y -= t/5. * animSpeed * 0.25;
        
    // pos.x +=  0.; //fract(iTime)*10. * animSpeed;
    // pos.y -= 0.0 ;//fract(iTime)*10.0 * animSpeed * 0.25;
    
    //Adjust the scale of the craters
    float scale=0.15+iMouse.x/iResolution.x; // + 0.5;//0.25;
        
    float y = getY(pos, scale);
    float y1 = getY(pos - offs, scale);
    
    //float y2 = getY(pos + offs);

    vec3 normal = normalize(vec3(0.002, y-y1, 0.01)); //0.002 - depth

    float d = 0.5 + 0.5 * dot(normal, normalize(vec3(2.0, 1.1, 2.0))); //2.0  1.1 - "bumpyness
    
    float shadeScale = 1.0;

    // shadows
    #ifdef SHADOWS
    {
        
        for(int i=0;i<int(40);++i) {
            float f01 = float(i+1) / float(40.0);
            f01 = pow(f01, 2.0);

            vec2 posTest = pos - vec2(f01, -f01) * 0.5;

            float yTest = getY(posTest, 1.);

            float over = yTest - (y + f01 * 3.0);
            
            if(over > 0.0)
                shadeScale = min(shadeScale, mix(1.0, 0.7, clamp(over*0.5,0.0,1.0)));
        }
    }
    
    d *= shadeScale;
    #endif
  
    
    float c = y * 0.02 - 0.5 + d * 1.3;
    c = max(0.2,c); //fixes a bit of the bad black circles #21-11-2017
    
    vec3 rgb = vec3(mix(mix(vec3(0.0,0.0,0.0), vec3(0.3,0.2,0.1), c), vec3(1.0,0.95,0.90), c));
    
    gl_FragColor = vec4(rgb,1.0);
    return gl_FragColor;
}

//#define FLAT 1

void main( )
{
    #ifdef FLAT   //original craters look
    gl_FragColor = craters(xlv_TEXCOORD0); return;
    #endif
    
    vec2 defaultFragCoord = xlv_TEXCOORD0;    
    vec2 pos = 2.0 * vec2(xlv_TEXCOORD0.xy - 0.5 * iResolution.xy) / iResolution.y;
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    float ln = length(circlePos.xy - pos);
    float bgMask = smoothstep(ln, ln + 0.015, circleRad); //second param - blur on the edge    
    vec3 sphereP = spherePos(circlePos.xy, pos, circleRad);
    vec4 sphereColor = texture(iChannel0, sphereMap(sphereP, circleRad));    
    vec4 craterColor = craters(sphereMap(sphereP, circleRad)*iResolution.y); //OK!
        
    float aa = 1.0/iResolution.x; // 0.002; //shift for the antialiasing calculation
         
    #ifdef AA3
    craterColor +=craters(sphereMap(sphereP+vec3(aa,0.,0.)*0.5, circleRad)*iResolution.y);
    craterColor +=craters(sphereMap(sphereP+vec3(00.,aa,0.)*0.5, circleRad)*iResolution.y);
    craterColor/=3.;
    #endif
    
    #ifdef AA5
     //vec4 anti = craters(sphereMap(sphereP, circleRad)*iResolution.y)
    craterColor+= craters(sphereMap(sphereP+vec3(aa,0.,0.), circleRad)*iResolution.y)    
       + craters(sphereMap(sphereP-vec3(-aa,0.,0.), circleRad)*iResolution.y)         
       + craters(sphereMap(sphereP+vec3(0.,aa,0.), circleRad)*iResolution.y) 
       + craters(sphereMap(sphereP-vec3(0.,-aa,0.), circleRad)*iResolution.y);
       craterColor /=5.;
    #endif
  
    sphereColor = craterColor; //craters(xlv_TEXCOORD0);
	gl_FragColor =  gl_FragColor * (1.0 - bgMask) + sphereColor * bgMask; 
                     
    //Some stars
    if (gl_FragColor.r < 0.001)
    {
        vec2 n1 = N22(pos);
        float star = n1.x < 0.06 ? 0.1 : 0.;
        //star+= n1.y > 0.9 ? 0.15 : 0.0;
        //star+= n1.y > 0.97 ? N1(n1.x)/1.0*(max(star, sin(iTime))) : 0.0;
        float vary=1.0;
        star+= n1.y > 0.97 ? N1(n1.x)/1.0*vary : 0.0;
        vec2 fb = fbm(pos);
        star*=max(fb.x, fb.y);
        
    gl_FragColor += star;
    }

}

