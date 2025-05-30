struct VsIn2 {
    float2 position	: POSITION;
    float2 uv		: TEXCOORD0;
};
struct VsIn3 {
    float3 position	: POSITION;
    float3 Normal	: NORMAL;
    float3 Binormal	: BINORMAL;
    float3 Tangent	: TANGENT;
    float3 Color	: TEXCOORD0;
    float2 uv		: TEXCOORD1;
};
struct VsOut {
    float4 position	: POSITION;
    float2 uv		: TEXCOORD0;
};

/*
  Copyright (cos(angle)) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
[Vertex shader]
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main2(VsIn2 In)
{
	VsOut Out=(VsOut)0;
	Out.position = float4(In.position.x, In.position.y, 0.0 , 1.0);
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}
VsOut main3(VsIn3 In)
{
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position, 1.0));
	Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;
	return Out;
}

[Fragment shader]
sampler2D iChannel0;
sampler2D iChannel1;
float3      iResolution=float3(1,1,1);                  // viewport resolution (in pixels)
float     iTime=0;                        // shader playback time (in seconds)
float4      iMouse=float4(1,1,1,1);                       // mouse pixel coords
float mod(float x, float y)
{
  return x - y * floor(x / y);
}


float2 mod(float2 x, float2 y)
{
  return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
  return x - y * floor(x / y);
}
//Ocean shader with noise waves and slope based subsurface scattering.

//Based on:
//https://www.shadertoy.com/view/Ms2SD1
//https://unitywatershader.wordpress.com/
//https://www.alanzucconi.com/2017/08/30/fast-subsurface-scattering-1/
//https://www.fxguide.com/fxfeatured/assassins-creed-iii-the-tech-behind-or-beneath-the-action/

//Average the results of a 2x2 region of subpixel samples for antialiasing.
//#define AA

//Raymarching
int MAX_STEPS = 300;
float MAX_DIST = 1800.0;
float EPSILON = 1e-4;

//Wave extent and noise field scale.
float HEIGHT = 4.0;
float SCALE = 0.035;
float waveSpeed = 0.035;

//Octaves of FBM.
int shapeLimit = 3;
int normalLimit = 8;

float3 skyColour = float3(0.09, 0.33, 0.81);
float3 sunLightColour = float3(1,1,1);
float3 sunColour = float3(1,1,1);

float specularStrength = 100.0;
float shininess = 2048.0;
float3 specularColour = float3(1,1,1);

//In a circle of 2*PI
float sunLocation = 0.0;
//0: horizon, 1: zenith
float sunHeight = 0.35;

float diffuseStrength = 0.2;
float3 diffuseColour = float3(0.025,0.225,0.2*0.65);

float ambientStrength = 0.5;
float3 ambientColour = float3(0.5 * 0.025,0.5 * 0.225,0.5 * 0.2*0.65);

float3 scatterColour = float3(0.05, 0.8, 0.7);
float power = 8.0;
float scale = 0.4;
float distortion = 0.2;
float scatterStrength = 0.3;

float angle = 3.14;

//float sin(angle) = sin(angle);
//float cos(angle) = cos(angle);
//float2x2 rotation = float2x2(cos(angle), sin(angle), -sin(angle), cos(angle));

float saturate(float x){
	return clamp(x, 0.0, 1.0);
}

float3 rayDirection(float fieldOfView, float2 fragCoord) {
    float2 xy = fragCoord - iResolution.xy / 2.0;
    float z = (0.5 * iResolution.y) / tan(radians(fieldOfView) / 2.0);
    return normalize(float3(xy, -z));
}

//https://www.geertarien.com/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/
float3x3 lookAt(float3 camera, float3 targetDir, float3 up){
  float3 zaxis = normalize(targetDir);    
  float3 xaxis = normalize(cross(zaxis, up));
  float3 yaxis = cross(xaxis, zaxis);

  return float3x3(xaxis, yaxis, -zaxis);
}

//Darken sky when looking up and add a white haze at the horizon.
float3 getSkyColour(float3 rayDir){
    return lerp(float3(1,1,1), lerp(skyColour, 0.2*skyColour, rayDir.y), 
               smoothstep(-0.5, 0.25, rayDir.y));
}

//By iq
float noised( in float2 x ){
    float2 f = frac(x);
    float2 u = f*f*(3.0-2.0*f);
  
    float2 p = floor(x);
	float a = 0.0;//tex2D( iChannel1, (p+float2(0.5,0.5))*0.00390625).x;
	float b = 0.0;//tex2D( iChannel1, (p+float2(1.5,0.5))*0.00390625).x;
	float c = 0.0;//tex2D( iChannel1, (p+float2(0.5,1.5))*0.00390625).x;
	float d = 0.0;//tex2D( iChannel1, (p+float2(1.5,1.5))*0.00390625).x;
    
	float res = (a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y);
    res = res - 0.5;
    return res;
}

float fbm(float3 pos, int limit){
    float res = 0.0;
    float freq = 1.0;
    float amp = 1.0;
    
    for(int i = 0; i < 9; i++){ 
        if(i == limit){break;}

       	res += noised(freq*(pos.xz+iTime*(waveSpeed*float(9-i+1))))*amp;

        freq *= 1.75;
        amp *= 0.5;
        
        pos.xz = mul(float2x2(cos(angle), sin(angle), -sin(angle), cos(angle)), pos.xz);
    }
	return res;
}

// https://www.shadertoy.com/view/4dlGW2

// Tileable noise, for creating useful textures. By David Hoskins, Sept. 2013.
// It can be extrapolated to other types of randomised texture.

#define SHOW_TILING
#define TILES 2.0 // Use 1.0 for normal tiling across whole texture.

//----------------------------------------------------------------------------------------
float Hash(in float2 p, in float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return frac(sin(dot(p, float2(27.16898, 38.90563))) * 5151.5473453);
}

//----------------------------------------------------------------------------------------
float Noise(in float2 p, in float scale )
{
	float2 f;
	
	p *= scale;

	
	f = frac(p);		// Separate integer from fractional
    p = floor(p);
	
    f = f*f*(3.0-2.0*f);	// Cosine interpolation approximation
	
    float res = lerp(lerp(Hash(p, 				 scale),
						Hash(p + float2(1.0, 0.0), scale), f.x),
					lerp(Hash(p + float2(0.0, 1.0), scale),
						Hash(p + float2(1.0, 1.0), scale), f.x), f.y);
    return res;
}

//----------------------------------------------------------------------------------------
float fBm(in float2 p)
{
    p += float2(sin(iTime * .7), cos(iTime * .45))*(.1) + iMouse.xy*.1/iResolution.xy;
	float f = 0.0;
	// Change starting scale to any integer value...
	float scale = 10.;
    p = mod(p, scale);
	float amp   = 0.6;
	
	for (int i = 0; i < 5; i++)
	{
		f += Noise(p, scale) * amp;
		amp *= .5;
		// Scale must be multiplied by an integer value...
		scale *= 2.;
	}
	// Clamp it just in case....
	return min(f, 1.0);
}

//Get height of waves at xz coordinates.
float getHeight(float3 pos, int limit){
    return HEIGHT*fBm(SCALE*pos.xz);
    //return HEIGHT*fbm(SCALE*pos, limit);
}

//Binary search for 0 crossing given two points on either side of the surface.
float bisection(float3 start, float3 rayDir, float near_, float far_){
    float midpoint = (far_ + near_) * 0.5;
    //Sample point
    float3 p = float3(0,0,0);
    float near = near_;
    float far = far_;
    float height = 0.0;
    //Difference between sample point and water height.
    float diff = 0.0;
    
    for(int i = 0; i < 8; i++){
        p = start + rayDir * midpoint;
        height = getHeight(p, shapeLimit);
        diff = p.y - height;
        
        if(abs(diff) < EPSILON){
        	break;
        }else{
            
            if(diff < EPSILON){
                //Point is below waves
                //Search first half
                far = midpoint;
            }else{
                //Point is above waves
                //Search second half
                near = midpoint;
            }
            midpoint = (far + near) * 0.5;
        }
    }
    return midpoint;
}

//Assume normalised vectors.
bool getPlaneIntersection(float3 org, float3 ray, float3 planePoint, float3 normal, out float t){
    float denom = dot(normal, ray); 
    if (denom > 1e-6) { 
        float3 p0l0 = planePoint - org; 
        t = dot(p0l0, normal) / denom; 
        return (t >= 0.0); 
    } 
 
    return false; 
}

float getIntersection(float3 start, float3 rayDir, float maxDist){
	//Distance between sample points. Set according to previous sample.
    float stepSize = 0.0;
    //Height of the waves.
    float height = 0.0;
    //Length of the ray.
    float dist = 0.0;
    //Difference between sample point and wave heights.
    float diff = 0.0;
    
    //Start ray tracing from intersection with xz-plane at y == 1.1*HEIGHT.
    float distToStart = 0.0;
    bool hitsPlane = getPlaneIntersection(start, rayDir, float3(0.0, 1.1*HEIGHT, 0.0), 
                                          float3(0,-1,0), distToStart);
    if(hitsPlane){
        dist = distToStart;
    }
    
    for(int i = 0; i < MAX_STEPS; i++){
        //Sample point
        float3 p = start + rayDir * dist;
        
        //The height of the waves at the xz coordinates of the sample point.
        height = getHeight(p, shapeLimit);
        
        diff = abs(p.y - height);
        
        //If sample point is close enough to the water surface, return distance.
        if(diff < EPSILON){
            return dist;
        }
        //If height of sample point is less than the height of the waves,
        //the ray has hit the water. Use bisection to find the 0 crossing.
        if(p.y < height){
        	dist = bisection(start, rayDir, dist - stepSize, dist);
            return dist;
        }
        
        //Static step size misses features and leads to banding. 
        //Set the step size to a fraction of the distance above the waves.
        //Could also have a small step size which increases with distance, giving 
        //detailed results close to the camera and reaching far. However, 
        //this approach is used in many shaders and seems to give best results.
        stepSize = diff * 0.5;
        
        //Increment ray
        dist += stepSize;
        
        if(dist > MAX_DIST){
        	return MAX_DIST;
        }
    }
    return dist;
}

//http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
//https://stackoverflow.com/questions/33736199/calculating-normals-for-a-height-map
float3 getNormal(float3 p, float t, int limit){
    
	//Making the normal sample distance depend on the ray length and resolution
    //leads to less noise.
    float eps = (0.05 / iResolution.y) * pow(t, 1.55);
    
    //Central difference method for estimating the derivatives and normal of a surface.
    /*
    float left = getHeight(float3(p.x-eps, p.y, p.z), limit);
    float right = getHeight(float3(p.x+eps, p.y, p.z), limit);
    float top = getHeight(float3(p.x, p.y, p.z-eps), limit);
    float bottom = getHeight(float3(p.x, p.y, p.z+eps), limit);
    
    float uy = right-left;
    float3 u = normalize(float3(2.0*eps, uy, 0.0));

   	float vy = bottom-top;
    float3 v = normalize(float3(0.0, vy, 2.0*eps));
    
    return normalize(cross(v,u));
    */
    
    //The above is equivalent to the following:
    return normalize(float3( 
        getHeight(float3(p.x-eps, p.y, p.z), limit) 
        - getHeight(float3(p.x+eps, p.y, p.z), limit),
        
        2.0*eps,
        
        getHeight(float3(p.x, p.y, p.z-eps), limit) 
        - getHeight(float3(p.x, p.y, p.z+eps), limit) 
    ));
}

//https://learnopengl.com/PBR/Theory
float fresnelSchlick(float3 cameraPos, float3 position, float3 normal){
    float cosTheta = dot(normal, normalize(cameraPos - position));
	float F0 = 0.02;
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//Return colour of surface fragment based on light information.
float3 shading(float3 cameraPos, float3 position, float3 normal, float3 rayDir, 
             float dist, float3 lightDirection){
    
	float3 result = float3(0,0,0); 
    
	float3 halfwayDir = normalize(lightDirection - rayDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	//Colour of light sharply reflected into the camera.
	float3 specular = spec * specularColour * sunLightColour; 
	
	//How much a fragment faces the sun.
	float sun = max(dot(normal, lightDirection), 0.0);
    //Main sunlight contribution.
    float3 sunLight = sun * sunLightColour;
    
    //How much the fragment faces up.
    float sky = max(dot(normal, float3(0,1,0)), 0.0);
    //Sky light. A blue light from directly above.
	float3 skyLight = sky * skyColour;
    
    //Combine light
    result += 0.1 * sunLight;
    result += 0.1 * skyLight;
    
    //Sample point height in the wave.
    float heightFraction = (position.y + HEIGHT) / (2.0 * HEIGHT);
    
    //Lighten the water when looking towards the horizon and darken it straight down.
    float3 col = lerp(ambientColour, 0.5*scatterColour, pow(0.5+0.5*rayDir.y, 2.0));
    
    //Light and material interaction.
    result *= diffuseColour;
    result += ambientStrength * col + specularStrength * specular;
    
    //Fake subsurface scattering based on light direction and surface normal.
    //https://www.alanzucconi.com/2017/08/30/fast-subsurface-scattering-1/
    float3 h = normalize(-lightDirection + normal * distortion);
	float vDotH = pow(saturate(dot(rayDir, -h)), power) * scale;
    
    //Scattering in stronger closer to the camera and higher in the wave.
    result += 	scatterStrength * pow((1.0-dist/MAX_DIST), 4.0) * 
        		heightFraction * vDotH * scatterColour;
    
    //Reflection of the sky.
    float3 reflectedDir = normalize(reflect(rayDir, normal));
    float3 reflectedCol = getSkyColour(reflectedDir);
    float fresnel = saturate(fresnelSchlick(cameraPos, position, normal));
   	result = lerp(result, 0.5*reflectedCol, fresnel);

    return result;
}

float getGlow(float dist, float radius, float intensity){
    dist = max(dist, 1e-6);
	return pow(radius/dist, intensity);	
}

//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x){
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

float4 main(VsOut IN): COLOR 
{
	float4 fragColor;
	float2 fragCoord=IN.uv;
       
    //Camera position is persistent and calculated in BufferA.
    float3 cameraPos;// ??? = tex2D(iChannel0, int2(0.5, 1.5)).xyz;
    float3 targetDir = -cameraPos;
    float3 up = float3(0.0, 1.0, 0.0);
    //Get the view matrix from the camera orientation.
    float3x3 viewMatrix = lookAt(cameraPos, targetDir, up);
    
    float3 lightDirection = normalize(float3(sin(sunLocation), sunHeight, cos(sunLocation)));
    float3 col = float3(0,0,0);
    float3 rayDir;
    bool skyVisible = false;
    
    #ifdef AA
    
    for(int i = 0; i <= 1; i++) {
        for(int j = 0; j <= 1; j++) {

            //Get the default direction of the ray (along the negative Z direction).
            rayDir = rayDirection(40.0, fragCoord+float2(i,j)/2.0);
            
            #else
            
            rayDir = rayDirection(40.0, fragCoord);
            
            #endif


            //Transform the ray to point in the correct direction.
            rayDir = normalize(mul(viewMatrix , rayDir));

            float dist = MAX_DIST;
            
            //Only render water for rays pointing down.
            if(rayDir.y < 0.0){
                //Find the distance to where the ray stops.
                dist = getIntersection(cameraPos, rayDir, MAX_DIST);
            }

            if(dist == MAX_DIST){
                skyVisible = true;
                col += getSkyColour(rayDir);
            }else{
                float3 position = cameraPos + rayDir * dist;
                int limit = normalLimit;
				float xzDist = length(cameraPos.xz - position.xz);
                
                //Reduce normal detail after a certain distance.
                if(xzDist > 0.3*MAX_DIST){
                    limit = 4;
                }

                float3 normal = getNormal(position, xzDist, limit);
                col += shading(cameraPos, position, normal, rayDir, dist, lightDirection);
            }

            #ifdef AA
        }
    }

    col *= 0.25;
    
    #endif

    //Display the sun as a glow in the light direction.
    if(skyVisible){
        float mu = dot(rayDir, lightDirection);
        col += sunColour*getGlow(1.0-mu, 0.0005, 1.0);
    }
    
    //Tonemapping.
    col = ACESFilm(col);

    //Gamma correction 1.0/2.2 = 0.4545...
    col = pow(col, float3(0.4545,0.4545,0.4545));

    //Output to screen.
    fragColor = float4(col, 1.0);
}

