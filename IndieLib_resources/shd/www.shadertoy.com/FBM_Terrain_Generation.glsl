// HIGHER IS MORE
#define DEFINITION 8.
// HIGHER IS MORE
#define SHATTEREDNESS 2.75
// LOWER IS MORE, values far from 0.5 lower the definition
#define INLANDNESS 0.475

// RANDOM FUNCTIONS TAKEN FROM THE BOOK OF SHADERS

float random (in vec2 st) {
    return fract(cos(sin(dot(st.xy,
                         vec2(12.9898,78.233))))*
        43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

// UNUSED
float triangeWave( in float t )
{
    
    const float period = 20.;
    
    return 2. * abs( t/period - floor(t/period + 1./2.) );

}

const int NCOLORS = 7;

// blue color palette
/*
vec4 palette[int(NCOLORS)] = vec4[NCOLORS](
vec4(0.02745098, 0.40784314, 0.54901961, 1.0),
vec4(0.09803922, 0.5254902, 0.68627451, 1.0),
vec4(0.31372549, 0.71372549, 0.80392157, 1.0),
vec4(0.2745098, 0.80784314, 0.79215686, 1.0),
vec4(0.54117647, 0.91764706, 0.91372549, 1.0),
vec4(0.7372549, 0.97254902, 1.0, 1.0)
);
*/


// terrain color palette
vec4 palette[int(NCOLORS)] = vec4[NCOLORS](
vec4(0.89803922, 0.85098039, 0.76078431, 1.0),
vec4(0.44705882, 0.32941176, 0.15686275, 1.0),
vec4(0.25490196, 0.59607843, 0.03921569, 1.0),
vec4(0.96470588, 0.84313725, 0.69019608, 1.0),
vec4(0.2745098, 0.80784314, 0.79215686, 1.0),
vec4(0.09803922, 0.5254902 , 0.68627451, 1.0),
vec4(0.02745098, 0.40784314, 0.54901961, 1.  )
);


void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    
    // scale for aspect ratio
    uv.x *=  iResolution.x / iResolution.y;
    
    // zoom out a bit 
    uv *= 5.0;
    
    float density = 0.0;
    
    // shift to the upper-right
    uv += iTime / 5.;
    
    for(float i = 0.; i < DEFINITION; i++)
    {
        density += pow(INLANDNESS, i + 1.0) * noise(uv * pow(SHATTEREDNESS, i));
    }
    
    
    int p_indx= int(floor(density * float(NCOLORS)));
    float palette_lerp = pow(fract(density * float(NCOLORS)), 1.5); // smooth everything

   
    gl_FragColor = mix(palette[p_indx], palette[p_indx + 1], palette_lerp); 
}

