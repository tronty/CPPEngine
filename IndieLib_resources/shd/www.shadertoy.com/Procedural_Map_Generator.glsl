//----------------------------------------------------
// TETXURE_SETTINGS
//----------------------------------------------------
#define TEXTURE_ROWS 100.0f
#define GAMMA_CORRECTION 0

// Gets the UV coordinate for Image
vec2 get_uv(vec2 xlv_TEXCOORD0)
{
    vec2 uv = xlv_TEXCOORD0 / iResolution.xy;
    uv *= TEXTURE_ROWS;
    // uv = (uv * 2.0f) - 1.0f;
    uv.x *= (iResolution.x / iResolution.y);
    return uv;
}

// Inverse Lerp Function
float linear_step(float l, float u, float a)
{
    return ((a - l) / (u - l));
}

//----------------------------------------------------
//----------------------------------------------------


//----------------------------------------------------
// PERLIN_NOISE_GENERATOR
//----------------------------------------------------

float fade(float t)
{
    return t * t * t * (t * ((t * 6.0f) - 15.0f) + 10.0f);
}

float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    float u = (h < 8) ? x : y,
           v = ((h < 4) ? y : (h == 12 || h == 14 ? x : z));
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

int[512] p=int[512](151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
             8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
             35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
             134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
             55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
             18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
             250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
             189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167,
             43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246,
             97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
             107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
             138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
             151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
             8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
             35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
             134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
             55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
             18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
             250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
             189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167,
             43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246,
             97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
             107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
             138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180);

// Returns noise value for position
float noise(float x, float y, float z)
{  
    // Find the unit cube that contains the point
    int X = int(floor(x)) & 255;
    int Y = int(floor(y)) & 255;
    int Z = int(floor(z)) & 255;

    // Find relative x, y,z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    
    // Compute fade curves for each of x, y, z
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    float res = mix(
        mix(
            mix(grad(p[AA], x, y, z),
                 grad(p[BA], x - 1.0f, y, z),
                 u),
            mix(grad(p[AB], x, y - 1.0f, z),
                 grad(p[BB], x - 1.0f, y - 1.0f, z),
                 u),
            v),
        mix(
            mix(grad(p[AA + 1], x, y, z - 1.0f),
                 grad(p[BA + 1], x - 1.0f, y, z - 1.0f),
                 u),
            mix(grad(p[AB + 1], x, y - 1.0f, z - 1.0f),
                 grad(p[BB + 1], x - 1.0f, y - 1.0f, z - 1.0f),
                 u),
            v),
        w);
    return (res + 1.0f) / 2.0f;
}

//----------------------------------------------------
//----------------------------------------------------


//----------------------------------------------------
// NOISE_GENERATOR
//----------------------------------------------------
#define NOISE_FUNCTION get_octave_noise
#define SMOOTH_IL 1
#define SCALE_FROM_CENTER 1
#define LEVEL_OF_DETAIL 0
#define NOISE_SCALE 20
#define NUMBER_OCTAVES 4
#define PERSISTANCE 0.5f
#define LACUNARITY 2.0f
#define NORMALIZE_OFFSET 0
#define FALLOFF_CURVE 5
#define FALLOFF_SHIFT 10

// Generates a random noise between 0.0 and 1.0
float rand(vec2 pos)
{
    return fract(sin(dot(pos, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

// Gets the Random Noise Height
float get_noise(vec2 pos,float time)
{
    pos *= float(TEXTURE_ROWS);
#if LEVEL_OF_DETAIL
    pos /= float(LEVEL_OF_DETAIL);
    pos = vec2(floor(pos.x), floor(pos.y));
    pos *= float(LEVEL_OF_DETAIL);
#endif
    return rand(pos);
}

// Gets the perlin noise for point b/w 0.0 and 1.0
float perlin(vec2 pos)
{
    return noise(pos.x, pos.y, 0.0f);
}

// Gets the scaled perlin noise for point
float get_perlin_noise(vec2 pos, float time)
{
    float rows = float(TEXTURE_ROWS);
    float columns = rows * (iResolution.x / iResolution.y);
    pos *= rows;
    float scale = float(NOISE_SCALE);
    if(scale <= 0.0f)
    {
        scale = 0.001f;
    }
    
    float halfX = 0.0f;
    float halfY = 0.0f;
#if SCALE_FROM_CENTER
    halfX = columns / 2.0f;
    halfY = rows / 2.0f;
#endif

#if LEVEL_OF_DETAIL
    pos /= float(LEVEL_OF_DETAIL);
    pos = vec2(floor(pos.x), floor(pos.y));
    pos *= float(LEVEL_OF_DETAIL);
#endif
     return perlin((pos - vec2(halfX,halfY)) / scale);
}

// Gets the scaled octave noise for point
float get_octave_noise(vec2 pos, float timeRate)
{
    float rows = float(TEXTURE_ROWS);
    pos *= rows;
    float columns = rows*(iResolution.x / iResolution.y);
    float scale = float(NOISE_SCALE);
    if(scale <= 0.0f)
    {
        scale += 0.001f;
    }
    
    int octaves = int(NUMBER_OCTAVES);
    float lacunarity = max(LACUNARITY, 1.0f);
    float persistence = min(PERSISTANCE, 1.0f);
    
    float halfX = 0.0f;
    float halfY = 0.0f;
#if SCALE_FROM_CENTER
    halfX = columns / 2.0f;
    halfY = rows / 2.0f;
#endif

    float[20] offsets=float[20](
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    );
    for(int i = 0; i < octaves * 2; i++)
    {
        offsets[i] = rand(vec2(pos.x, pos.y)) * 0.0f;
    }
    
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float noiseVal = 0.0f;
    
#if LEVEL_OF_DETAIL
    pos = vec2(floor(pos.x), floor(pos.y));
    pos /= float(LEVEL_OF_DETAIL);
#endif

    vec2 offset = timeRate * vec2(iTime * 1.25f, iTime * 1.25f);
    scale += sin(iTime * 0.85f) * 0.0f;
    
    for (int i = 0; i < octaves; i++)
    {
#if NORMALIZE_OFFSET
        float sampleX = (((pos.x-halfX) / scale) * frequency) + offset.x + offsets[i*2];
        float sampleY = (((pos.y-halfY) / scale) * frequency) + offset.y + offsets[(i*2)+1];
#else
        float sampleX = (((pos.x-halfX + offset.x*scale) / scale) * frequency)  + offsets[i*2];
        float sampleY = (((pos.y-halfY + offset.y*scale) / scale) * frequency)  + offsets[(i*2)+1];
#endif
        float noise = (perlin(vec2(sampleX, sampleY)) * 2.0f) - 1.0f;
        noiseVal += noise * amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }     
#if SMOOTH_IL
    noiseVal = smoothstep(-0.95f, 1.1f, noiseVal);
#else
    noiseVal = linear_step(-0.7f,0.85f,noiseVal);
#endif
    return noiseVal;
}

// Evaluates the falloff curve to match the border
float eval_curve(float val)
{
    float a = float(FALLOFF_CURVE);
    float b = float(FALLOFF_SHIFT);
    val = pow(val, a) / (pow(val, a) + pow(b - (b * val), a));
    return val;
}

// Returns the falloff map for map
float falloff(vec2 pos,float time)
{
    pos.x /= (iResolution.x / iResolution.y);
    pos = pos * 2.0f - 1.0f;
    float val = max(abs(pos.x), abs(pos.y));
    return clamp(eval_curve(val), 0.0f, 1.0f);
}

// Gets a Cloud Map
float cloud_map(vec2 pos)
{
    vec2 offset=vec2(500.0f);
    //offset+=vec2(iTime*0.0f);
    float h = get_octave_noise(pos+offset,1.25f);
    float th=0.75f;
    h=(h<th)?0.0f:h;
    return h;
}
//----------------------------------------------------
//----------------------------------------------------


//----------------------------------------------------
// MAP_SETTINGS
//----------------------------------------------------

// Color for all regions
vec3[8] regions = vec3[8](
vec3(0.15f, 0.15f, 0.75f), // DARK BLUE
vec3(0.2f, 0.5f, 1.0f), // OCEAN BLUE
vec3(0.98f, 0.88f, 0.2f), // SAND YELLOW
vec3(0.2f, 0.8f, 0.2f), // LIGHT GREEN
vec3(0.2f, 0.60f, 0.2f), // DARK GREEN
vec3(0.6f, 0.35f, 0.1f), // LIGHT BROWN
vec3(0.25f, 0.1f, 0.05f), // DARK BROWN
vec3(0.85f, 0.85f, 1.0f) // SNOW BLUE
);

// Start height for all regions
float[8] heights = float[8](
0.00f, // DEEP OCEAN
0.025f, // OCEAN
0.35f, // COAST
0.365f, // PLAINS
0.565f, // FORESTS
0.695f, // HILLS
0.82f, // MOUNTAINS
0.93f // SNOW
);

// Gets the index of the region height lies in
int get_region_index(float h)
{
    int index = -1;
    for(int i = 0; i < 8; i++)
    {
        if(h >= heights[i])
        {
            index++;
        }
        else
        {
            break;
        }
    }
    return index;
}
//----------------------------------------------------
//----------------------------------------------------
#define APPLY_FALLOFF 1
#define COLOR_MAP 1

#define BLEND_REGIONS 1
#define MIX_FACTOR 0.65f
#define MIX_THRESHOLD 0.85f

#define DRAW_CLOUDS 1
#define CLOUD_BLEND 0.75f

#define ENABLE_LIGHT 1
#define LIGHT_ANGLE 240.0f
#define LIGHT_ELEVATION_ANGLE 30.0f
#define LIGHT_CHECK 0.015f
#define LIGHT_INTENSITY 5.0f

vec3 get_color(vec2 uv)
{
    bool insideMap=true;
    float timeFac=0.85f;
    vec3 shadowMap=vec3(1.0f);
    bool applyShadow=false;
    
    // Get Height
    float h= NOISE_FUNCTION(uv,timeFac);
    vec3 col=vec3(h);
    
#if ENABLE_LIGHT
    vec2 lightDir=vec2(1.0f,0.0f);
    float theta=float(LIGHT_ANGLE+iTime*45.0f)*(3.1416f/180.0f);
    mat2 rot=mat2(cos(theta), -sin(theta),
                  sin(theta), cos(theta));
    lightDir=normalize(lightDir*rot);
    
    float checkDist=float(LIGHT_CHECK);
    float hP=NOISE_FUNCTION(uv-lightDir*checkDist,timeFac);
    
    float xDiff=checkDist;
    float yDiff=(hP-h);
    float delH=tan(LIGHT_ELEVATION_ANGLE*(3.1416f/180.0f))*xDiff;
    
    if(insideMap)
    {
        if(yDiff<delH)
        {
            yDiff=0.0f;
        }
        yDiff=1.0f-yDiff;
        if(yDiff>=0.0f)
        {
            yDiff=pow(yDiff,LIGHT_INTENSITY);
        }
        shadowMap=vec3(yDiff);
    }
#endif
    
#if APPLY_FALLOFF 
    // Apply Falloff
    float diff = falloff(uv,0.0f);
    if(h-diff<0.0f)
    {
        insideMap=false;
    }
    h = clamp(h-diff, 0.0f, 1.0f);
    col=vec3(h);
#endif

#if COLOR_MAP
    // Get Color from Region
    int index = get_region_index(h);
    col = regions[index];
#endif

    // Blend Region Colors
#if BLEND_REGIONS
    float h2 = ((index+1)<8?heights[index+1]:1.0f);
    float off = linear_step(heights[index],h2,h);
    //off=smoothstep(heights[index],h2,h);
    if(off>=MIX_THRESHOLD/2.0f)
    {
       applyShadow=true;
    }
    if(off>=MIX_THRESHOLD)
    {
        col = mix(col, mix(col, regions[index+1], off), MIX_FACTOR);
    }
#endif
    
    // Apply Cloud Cover
#if DRAW_CLOUDS
    if(insideMap)
    {
        float cH=cloud_map(uv);
        vec3 cloud = vec3(cH);
        if(cH>0.0f)
        {
            col = (cloud*CLOUD_BLEND)+col*(1.0f-CLOUD_BLEND);
        }
    }
#endif

    if(insideMap && index>2 && applyShadow)
    {
        col*=shadowMap;
    }
    
    return col;
}
//----------------------------------------------------
//----------------------------------------------------

void main( )
{
    vec2 uv = xlv_TEXCOORD0/iResolution.y;
    vec3 col = get_color(uv);
    
#if GAMMA_CORRECTION
    col = pow(col, vec3(1.0f / 2.2f));
#endif
    // Output to screen
    gl_FragColor = vec4(col, 1.0);
}
